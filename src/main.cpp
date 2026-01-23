#include <iostream>
#include <vector>
#include <memory>
#include "Testable.hpp"
#include "SampleRegistry.hpp"

// Include all sample headers to ensure static registration occurs
// (Without these includes, the object files may not be linked)
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
#include "13_CopyAndSwap/CopyAndSwapSample.hpp"
#include "14_CastingTypes/CastingTypesSample.hpp"
#include "15_ThreadSafety/ThreadSafetySample.hpp"
#include "16_Concepts/ConceptsSample.hpp"
#include "17_Coroutines/CoroutinesSample.hpp"
#include "18_SRP/SRPSample.hpp"
#include "19_OCP/OCPSample.hpp"
#include "20_LSP/LSPSample.hpp"
#include "21_ISP/ISPSample.hpp"
#include "22_DIP/DIPSample.hpp"
#include "23_UMLRelationships/UMLRelationshipsSample.hpp"
#include "24_ThreeWayComparison/ThreeWayComparisonSample.hpp"
#include "25_Projections/ProjectionsSample.hpp"

int main(int argc, char* argv[]) {
    auto& registry = SampleRegistry::instance();
    auto samples = registry.createAll();

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