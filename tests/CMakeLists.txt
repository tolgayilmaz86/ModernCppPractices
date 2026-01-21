include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(unit_tests test.cpp ../src/01_RAII/RAIISample.cpp ../src/02_SFINAE/SFINAESample.cpp ../src/03_CRTP/CRTPSample.cpp ../src/04_PIMPL/PimplSample.cpp ../src/05_RuleOfFive/RuleOfFiveSample.cpp ../src/06_TypeErasure/TypeErasureSample.cpp ../src/07_VariantVisitor/VariantVisitorSample.cpp)

target_link_libraries(unit_tests GTest::gtest)

add_test(NAME unit_tests COMMAND unit_tests)