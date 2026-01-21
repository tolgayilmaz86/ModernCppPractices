#include <iostream>
#include <vector>
#include <memory>
#include "Testable.hpp"
#include "01_RAII/RAIISample.hpp"
#include "02_SFINAE/SFINAESample.hpp"
#include "03_CRTP/CRTPSample.hpp"
#include "04_PIMPL/PimplSample.hpp"
#include "05_RuleOfFive/RuleOfFiveSample.hpp"
#include "06_TypeErasure/TypeErasureSample.hpp"
#include "07_VariantVisitor/VariantVisitorSample.hpp"
#include "08_SmartPointers/SmartPointersSample.hpp"
#include "09_ExceptionSafety/ExceptionSafetySample.hpp"
#include "10_MoveSemantics/MoveSemanticsSample.hpp"
#include "11_TagDispatching/TagDispatchingSample.hpp"
#include "12_DeepShallowCopy/DeepShallowCopySample.hpp"
#include "13_CopyAndSwapIdiom/CopyAndSwapIdiomSample.hpp"
#include "14_Cast/CastSample.hpp"

int main(int argc, char* argv[]) {
    std::vector<std::unique_ptr<Testable>> samples;
    samples.push_back(std::make_unique<RAIISample>());
    samples.push_back(std::make_unique<SFINAESample>());
    samples.push_back(std::make_unique<CRTPSample>());
    samples.push_back(std::make_unique<PimplSample>());
    samples.push_back(std::make_unique<RuleOfFiveSample>());
    samples.push_back(std::make_unique<TypeErasureSample>());
    samples.push_back(std::make_unique<VariantVisitorSample>());
    samples.push_back(std::make_unique<SmartPointersSample>());
    samples.push_back(std::make_unique<ExceptionSafetySample>());
    samples.push_back(std::make_unique<MoveSemanticsSample>());
    samples.push_back(std::make_unique<TagDispatchingSample>());
    samples.push_back(std::make_unique<DeepShallowCopySample>());
    samples.push_back(std::make_unique<CopyAndSwapIdiomSample>());
    samples.push_back(std::make_unique<CastSample>());

    if (argc > 1) {
        try {
            int num = std::stoi(argv[1]);
            if (num >= 1 && num <= static_cast<int>(samples.size())) {
                samples[num - 1]->run();
            } else {
                std::cout << "Invalid sample number. Available samples: 1-" << samples.size() << std::endl;
            }
        } catch (const std::exception&) {
            std::cout << "Invalid argument. Please provide a number." << std::endl;
        }
    } else {
        std::cout << "Available samples:" << std::endl;
        for (size_t i = 0; i < samples.size(); ++i) {
            std::cout << i + 1 << ": " << samples[i]->name() << std::endl;
        }
        std::cout << "Run with: ./main <number>" << std::endl;
    }

    return 0;
}