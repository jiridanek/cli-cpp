PROJECT(dtests-messaging-cpp-tests)
   
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/target/tests/bin)

cmake_policy(SET CMP0003 OLD)
add_executable(ut_qpid_decoder_test
	ut_qpid_decoder_test.cpp
)

target_link_libraries(ut_qpid_decoder_test
           dtests-qpid-messaging-common)

add_test(NAME QpidDecoderTest 
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/target/tests/bin
    COMMAND ut_qpid_decoder_test)

cmake_policy(SET CMP0003 OLD)
add_executable(ut_qpid_dict_writer
	ut_qpid_dict_writer.cpp
)

target_link_libraries(ut_qpid_dict_writer
           dtests-qpid-messaging-common)

add_test(NAME DictWriterTest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/target/tests/bin
    COMMAND ut_qpid_dict_writer)


cmake_policy(SET CMP0003 OLD)
add_executable(ut_qpid_upstream_writer
	ut_qpid_upstream_writer.cpp
)

target_link_libraries(ut_qpid_upstream_writer
           dtests-qpid-messaging-common)

add_test(NAME UpstreamWriterTest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/target/tests/bin
    COMMAND ut_qpid_upstream_writer)

cmake_policy(SET CMP0003 OLD)
add_executable(ut_qpid_dict_statistics
	ut_qpid_dict_statistics.cpp
)

target_link_libraries(ut_qpid_dict_statistics
           dtests-qpid-messaging-common)

add_test(NAME QpidStatisticsTest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/target/tests/bin
    COMMAND ut_qpid_dict_statistics)