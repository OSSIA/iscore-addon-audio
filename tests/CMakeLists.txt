cmake_minimum_required(VERSION 3.0)
project(AudioTests)

enable_testing()
find_package(Qt5 5.3 REQUIRED COMPONENTS Core Test)

function(addAudioTest TESTNAME TESTSRCS)
    add_executable(Audio_${TESTNAME} ${TESTSRCS})
    setup_iscore_common_test_features(Audio_${TESTNAME})
    target_link_libraries(Audio_${TESTNAME} PRIVATE Qt5::Test iscore_plugin_audio dl)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        target_compile_options(Audio_${TESTNAME} PRIVATE "-Wno-error=return-type-c-linkage")
    endif()
    add_test(Audio_${TESTNAME}_target Audio_${TESTNAME})

endFunction()


# Commands
addAudioTest(Test1 "${CMAKE_CURRENT_SOURCE_DIR}/test1.cpp")
addAudioTest(Test2 "${CMAKE_CURRENT_SOURCE_DIR}/test2.cpp")
addAudioTest(Test3 "${CMAKE_CURRENT_SOURCE_DIR}/test3.cpp")
