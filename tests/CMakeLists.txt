include_directories(${CMAKE_SOURCE_DIR}/src)

# vcpkg toolchain is inherited from parent CMakeLists.txt

find_package(GTest CONFIG REQUIRED)

add_executable(unit_tests test.cpp ../src/01_RAII/RAIISample.cpp ../src/02_SFINAE/SFINAESample.cpp ../src/03_CRTP/CRTPSample.cpp ../src/04_PIMPL/PimplSample.cpp ../src/05_RuleOfFive/RuleOfFiveSample.cpp ../src/06_TypeErasure/TypeErasureSample.cpp ../src/07_VariantVisitor/VariantVisitorSample.cpp ../src/08_SmartPointers/SmartPointersSample.cpp ../src/09_ExceptionSafety/ExceptionSafetySample.cpp ../src/10_MoveSemantics/MoveSemanticsSample.cpp ../src/11_TagDispatching/TagDispatchingSample.cpp ../src/12_DeepShallowCopy/DeepShallowCopySample.cpp ../src/13_CopyAndSwapIdiom/CopyAndSwapIdiomSample.cpp ../src/14_Cast/CastSample.cpp ../src/15_ThreadSafety/ThreadSafetySample.cpp ../src/16_Concepts/ConceptsSample.cpp ../src/17_Coroutines/CoroutinesSample.cpp)

target_link_libraries(unit_tests GTest::gtest GTest::gtest_main)

add_test(NAME unit_tests COMMAND unit_tests)