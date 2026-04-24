#pragma once

#include "../Testable.hpp"
#include <string>

class TMPSample : public Testable {
public:
    void run() override;
    std::string name() const override { return "Template Meta Programming (TMP)"; }

private:
    void demonstrate_compile_time_values();
    void demonstrate_type_traits();
    void demonstrate_variadic_templates();
    void demonstrate_if_constexpr();
    void demonstrate_fold_expressions();
    void demonstrate_template_specialization();
    void demonstrate_sfinae_vs_concepts();
    void demonstrate_cpp20_tmp();
};
