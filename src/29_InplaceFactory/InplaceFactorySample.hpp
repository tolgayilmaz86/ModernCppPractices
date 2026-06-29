#pragma once

#include "../Testable.hpp"
#include <concepts>
#include <string>
#include <type_traits>
#include <utility>

// Reusable, concept-constrained in-place factory wrapper.
// Wrapping a factory callable in this type and handing it to a destination
// (emplace, optional::emplace, aggregate/member init, direct init) lets the
// destination construct the object via the wrapper's conversion operator.
// Because that conversion returns a prvalue, C++17 guaranteed copy elision
// constructs the object directly at its final location: no temporary, no
// move, no extra destructor call.
template <class F>
concept Factory = std::invocable<F&> && !std::is_void_v<std::invoke_result_t<F&>>;

template <Factory F>
class InPlace {
public:
    explicit InPlace(F f) noexcept(std::is_nothrow_move_constructible_v<F>)
        : make_(std::move(f)) {}

    operator std::invoke_result_t<F&>() noexcept(std::is_nothrow_invocable_v<F&>) {
        return make_();
    }

private:
    F make_;
};

template <class F> InPlace(F) -> InPlace<F>;

class InplaceFactorySample : public Testable {
public:
    void run() override;
    std::string name() const override { return "In-Place Factory (Lambda Factories vs std::move)"; }

private:
    void demonstrate_move_overhead();
    void demonstrate_emplace_misconception();
    void demonstrate_lambda_factory();
    void demonstrate_inplace_wrapper();
    void demonstrate_immovable_types();
    void demonstrate_vector_caveat();

    // Helper class for demonstration
    class Tracer;
    static Tracer makeTracer(std::string id);
};
