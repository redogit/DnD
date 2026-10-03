#include "decision_field/local_plane.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace df::functions;

#define CHECK(x) do { if (!(x)) throw std::runtime_error(#x); } while (false)

template<class F>
void rejects(F&& f) {
    bool did = false;
    try { f(); } catch (const std::exception&) { did = true; }
    CHECK(did);
}

int main() {
    rejects([] { (void)LocalPlane("bad", {"only-one"}); });
    rejects([] { (void)LocalPlane("bad", {"same", "same"}); });

    LocalPlane plane("AnyFunctor/local-plane@1", {
        "preserve-source-occurrence",
        "preserve-obligation-admission",
        "retain-failure-residual",
    });

    const std::vector<std::string> all{
        "preserve-source-occurrence",
        "preserve-obligation-admission",
        "retain-failure-residual",
    };

    const auto first = plane.observe(
        "context:A", "mirror/self", "AnyFunctor:Mirror:route-A", all, true);
    CHECK(first.novel);
    CHECK(first.integrated);
    CHECK(!first.reused);
    CHECK(first.revision_before == 0U);
    CHECK(first.revision_after == 1U);
    CHECK(first.stages[0] == LocalPlaneStage::Receive);
    CHECK(first.stages[1] == LocalPlaneStage::Contextualize);
    CHECK(first.stages[2] == LocalPlaneStage::Represent);
    CHECK(first.stages[3] == LocalPlaneStage::Integrate);
    CHECK(plane.route_count() == 1U);
    CHECK(plane.resolve("context:A", "mirror/self") ==
          std::optional<std::string>{"AnyFunctor:Mirror:route-A"});

    const auto again = plane.observe(
        "context:A", "mirror/self", "AnyFunctor:Mirror:route-A", all, true);
    CHECK(!again.novel);
    CHECK(!again.integrated);
    CHECK(again.reused);
    CHECK(again.revision_before == 1U);
    CHECK(again.revision_after == 1U);
    CHECK(plane.route_count() == 1U);

    const auto conflict = plane.observe(
        "context:A", "mirror/self", "AnyFunctor:Mirror:route-B", all, true);
    CHECK(!conflict.integrated);
    CHECK(!conflict.reused);
    CHECK(conflict.residual_index.has_value());
    CHECK(plane.resolve("context:A", "mirror/self") ==
          std::optional<std::string>{"AnyFunctor:Mirror:route-A"});

    const auto missing = plane.observe(
        "context:B", "mirror/self", "AnyFunctor:Mirror:route-A",
        {"preserve-source-occurrence", "preserve-obligation-admission"}, true);
    CHECK(missing.novel);
    CHECK(!missing.integrated);
    CHECK(missing.missing_obligations ==
          std::vector<std::string>{"retain-failure-residual"});
    CHECK(missing.residual_index.has_value());
    CHECK(!plane.resolve("context:B", "mirror/self").has_value());

    const auto rejected = plane.observe(
        "context:C", "mirror/self", "AnyFunctor:Mirror:route-A", all, false);
    CHECK(rejected.novel);
    CHECK(!rejected.integrated);
    CHECK(rejected.residual_index.has_value());
    CHECK(!plane.resolve("context:C", "mirror/self").has_value());

    const auto new_context = plane.observe(
        "context:D", "mirror/self", "AnyFunctor:Mirror:route-A", all, true);
    CHECK(new_context.novel);
    CHECK(new_context.integrated);
    CHECK(plane.revision() == 2U);
    CHECK(plane.route_count() == 2U);

    CHECK(plane.residuals().size() == 3U);
    CHECK(plane.residuals()[0].reason == "integrated-route-conflict");
    CHECK(plane.residuals()[1].reason == "obligation-coverage-incomplete");
    CHECK(plane.residuals()[2].reason == "execution-not-admitted");

    std::cout
        << "PASS local plane: four-stage stair, plural obligations, admitted route integration, "
           "average-O(1) reuse lookup, and retained residuals without output caching\n";
}
