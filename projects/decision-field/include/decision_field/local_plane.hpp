#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace df::functions {

enum class LocalPlaneStage : unsigned char {
    Receive = 0,
    Contextualize = 1,
    Represent = 2,
    Integrate = 3,
};

struct LocalPlaneKey {
    std::string context_signature;
    std::string representation_signature;

    friend bool operator==(const LocalPlaneKey&, const LocalPlaneKey&) = default;
};

struct LocalPlaneKeyHash {
    std::size_t operator()(const LocalPlaneKey& key) const noexcept {
        const auto a = std::hash<std::string>{}(key.context_signature);
        const auto b = std::hash<std::string>{}(key.representation_signature);
        return a ^ (b + (a << 6U) + (a >> 2U));
    }
};

struct LocalPlaneResidual {
    std::size_t occurrence{};
    LocalPlaneKey key;
    std::string proposed_route_id;
    std::vector<std::string> missing_obligations;
    std::string reason;
};

struct LocalPlaneTransition {
    std::size_t occurrence{};
    std::size_t revision_before{};
    std::size_t revision_after{};
    std::array<LocalPlaneStage, 4> stages{
        LocalPlaneStage::Receive,
        LocalPlaneStage::Contextualize,
        LocalPlaneStage::Represent,
        LocalPlaneStage::Integrate,
    };
    LocalPlaneKey key;
    std::string route_id;
    bool novel{};
    bool integrated{};
    bool reused{};
    std::vector<std::string> missing_obligations;
    std::optional<std::size_t> residual_index;
};

class LocalPlane {
    struct Route {
        std::string id;
        std::size_t integrated_revision{};
        std::size_t reuse_count{};
    };

    std::string id_;
    std::vector<std::string> obligations_;
    std::unordered_set<std::string> obligation_index_;
    std::unordered_map<LocalPlaneKey, Route, LocalPlaneKeyHash> routes_;
    std::vector<LocalPlaneResidual> residuals_;
    std::size_t revision_{};
    std::size_t next_occurrence_{};

    static void require_nonempty(std::string_view value, const char* message) {
        if (value.empty()) {
            throw std::invalid_argument(message);
        }
    }

    [[nodiscard]] std::vector<std::string> missing_from(
        const std::vector<std::string>& satisfied_obligations) const {
        std::unordered_set<std::string> satisfied;
        satisfied.reserve(satisfied_obligations.size());
        for (const auto& id : satisfied_obligations) {
            require_nonempty(id, "satisfied obligation id must not be empty");
            satisfied.insert(id);
        }

        std::vector<std::string> missing;
        for (const auto& id : obligations_) {
            if (!satisfied.contains(id)) {
                missing.push_back(id);
            }
        }
        return missing;
    }

    std::size_t retain_residual(LocalPlaneResidual residual) {
        residuals_.push_back(std::move(residual));
        return residuals_.size() - 1U;
    }

public:
    explicit LocalPlane(std::string id, std::vector<std::string> obligations)
        : id_(std::move(id)), obligations_(std::move(obligations)) {
        require_nonempty(id_, "local plane id must not be empty");
        if (obligations_.size() < 2U) {
            throw std::invalid_argument("local plane requires multiple obligations");
        }
        obligation_index_.reserve(obligations_.size());
        for (const auto& obligation : obligations_) {
            require_nonempty(obligation, "obligation id must not be empty");
            if (!obligation_index_.insert(obligation).second) {
                throw std::invalid_argument("duplicate local-plane obligation");
            }
        }
    }

    [[nodiscard]] const std::string& id() const noexcept { return id_; }
    [[nodiscard]] const std::vector<std::string>& obligations() const noexcept { return obligations_; }
    [[nodiscard]] std::size_t revision() const noexcept { return revision_; }
    [[nodiscard]] std::size_t route_count() const noexcept { return routes_.size(); }
    [[nodiscard]] const std::vector<LocalPlaneResidual>& residuals() const noexcept { return residuals_; }

    // Average O(1) local routing lookup after a relation has been admitted and integrated.
    // This is a routing bound only: it does not cache outputs, bypass execution, or prove
    // a complexity bound for the underlying computation.
    [[nodiscard]] std::optional<std::string> resolve(
        std::string_view context_signature,
        std::string_view representation_signature) const {
        LocalPlaneKey key{std::string(context_signature), std::string(representation_signature)};
        const auto it = routes_.find(key);
        if (it == routes_.end()) {
            return std::nullopt;
        }
        return it->second.id;
    }

    [[nodiscard]] LocalPlaneTransition observe(
        std::string context_signature,
        std::string representation_signature,
        std::string proposed_route_id,
        std::vector<std::string> satisfied_obligations,
        bool admitted) {
        require_nonempty(context_signature, "context signature must not be empty");
        require_nonempty(representation_signature, "representation signature must not be empty");
        require_nonempty(proposed_route_id, "route id must not be empty");

        LocalPlaneTransition transition;
        transition.occurrence = next_occurrence_++;
        transition.revision_before = revision_;
        transition.revision_after = revision_;
        transition.key = {std::move(context_signature), std::move(representation_signature)};
        transition.route_id = std::move(proposed_route_id);
        transition.missing_obligations = missing_from(satisfied_obligations);

        const auto existing = routes_.find(transition.key);
        transition.novel = existing == routes_.end();

        if (!admitted) {
            transition.residual_index = retain_residual({
                transition.occurrence,
                transition.key,
                transition.route_id,
                transition.missing_obligations,
                "execution-not-admitted",
            });
            return transition;
        }

        if (!transition.missing_obligations.empty()) {
            transition.residual_index = retain_residual({
                transition.occurrence,
                transition.key,
                transition.route_id,
                transition.missing_obligations,
                "obligation-coverage-incomplete",
            });
            return transition;
        }

        if (existing != routes_.end()) {
            if (existing->second.id != transition.route_id) {
                transition.residual_index = retain_residual({
                    transition.occurrence,
                    transition.key,
                    transition.route_id,
                    {},
                    "integrated-route-conflict",
                });
                return transition;
            }
            ++existing->second.reuse_count;
            transition.reused = true;
            transition.route_id = existing->second.id;
            return transition;
        }

        ++revision_;
        routes_.emplace(transition.key, Route{transition.route_id, revision_, 0U});
        transition.integrated = true;
        transition.revision_after = revision_;
        return transition;
    }
};

} // namespace df::functions
