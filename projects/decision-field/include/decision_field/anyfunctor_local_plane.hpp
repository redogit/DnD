#pragma once

#include "decision_field/anyfunctor_mirror.hpp"
#include "decision_field/local_plane.hpp"

#include <string>
#include <utility>
#include <vector>

namespace df::functions {

inline std::vector<std::string> LocalObligationIds(const ObligationSet& obligations) {
    std::vector<std::string> ids;
    ids.reserve(obligations.items.size());
    for (const auto& obligation : obligations.items) {
        if (obligation.id.empty()) {
            throw std::invalid_argument("AnyFunctor obligation id must not be empty");
        }
        ids.push_back(obligation.id);
    }
    return ids;
}

template<class T>
LocalPlane LocalPlaneFor(std::string plane_id, const FunctionObject<T>& function) {
    return LocalPlane(std::move(plane_id), LocalObligationIds(function.obligations()));
}

inline std::string LocalContextSignature(
    std::string_view execution_space,
    std::string_view configuration_fingerprint) {
    if (execution_space.empty()) {
        throw std::invalid_argument("local AnyFunctor requires execution space");
    }
    return std::to_string(execution_space.size()) + ":" + std::string(execution_space) +
        std::to_string(configuration_fingerprint.size()) + ":" +
        std::string(configuration_fingerprint);
}

template<class T>
struct LocalAnyFunctorOutcome {
    AttackReceipt<T> execution;
    LocalPlaneTransition transition;
};

template<class T>
LocalAnyFunctorOutcome<T> LocalAnyFunctor(
    LocalPlane& plane,
    DecisionFieldEngine<T>& engine,
    const AnyInvocation<T>& invocation,
    std::string representation_signature) {
    const auto required = LocalObligationIds(invocation.function.obligations());
    if (plane.obligations() != required) {
        throw std::invalid_argument("local plane obligations do not match FunctionObject obligations");
    }

    auto execution = AnyFunctor(engine, invocation);
    std::vector<std::string> satisfied;
    if (execution.admission == ExecutionStatus::Succeeded) {
        satisfied = required;
    }

    auto transition = plane.observe(
        LocalContextSignature(
            invocation.execution_space,
            invocation.context.configuration_fingerprint),
        std::move(representation_signature),
        invocation.function.id(),
        std::move(satisfied),
        execution.admission == ExecutionStatus::Succeeded);

    return {std::move(execution), std::move(transition)};
}

} // namespace df::functions
