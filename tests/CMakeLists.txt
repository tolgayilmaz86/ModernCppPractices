include_directories(${CMAKE_SOURCE_DIR}/src)

# vcpkg toolchain is inherited from parent CMakeLists.txt

find_package(GTest CONFIG REQUIRED)

add_executable(unit_tests test.cpp 
    ../src/01_RAII/RAIISample.cpp 
    ../src/02_SFINAE/SFINAESample.cpp 
    ../src/03_CRTP/CRTPSample.cpp 
    ../src/04_PIMPL/PimplSample.cpp 
    ../src/05_RuleOfFive/RuleOfFiveSample.cpp 
    ../src/06_TypeErasure/TypeErasureSample.cpp 
    ../src/07_VariantVisitor/VariantVisitorSample.cpp 
    ../src/08_SmartPointers/SmartPointersSample.cpp 
    ../src/09_ExceptionSafety/ExceptionSafetySample.cpp 
    ../src/10_MoveSemantics/MoveSemanticsSample.cpp 
    ../src/11_TagDispatching/TagDispatchingSample.cpp 
    ../src/12_DeepShallowCopy/DeepShallowCopySample.cpp 
    ../src/13_CopyAndSwap/CopyAndSwapSample.cpp 
    ../src/14_CastingTypes/CastingTypesSample.cpp 
    ../src/15_ThreadSafety/ThreadSafetySample.cpp 
    ../src/16_Concepts/ConceptsSample.cpp 
    ../src/17_Coroutines/CoroutinesSample.cpp
    ../src/18_SRP/SRPSample.cpp
    ../src/19_OCP/OCPSample.cpp
    ../src/20_LSP/LSPSample.cpp
    ../src/21_ISP/ISPSample.cpp
    ../src/22_DIP/DIPSample.cpp
    ../src/23_UMLRelationships/UMLRelationshipsSample.cpp
)

# Enable strict compile warnings for tests
target_compile_options(unit_tests PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/W4 /permissive- /FS /Zi /Od>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<NOT:$<CONFIG:Debug>>>:/W4 /permissive- /FS>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Wconversion -Wshadow>
)

# Only link gtest, not gtest_main since we define our own main()
target_link_libraries(unit_tests GTest::gtest)

add_test(NAME unit_tests COMMAND unit_tests)