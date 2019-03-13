cmake_minimum_required(VERSION 3.6)
project(liteav)

set(CMAKE_CXX_STANDARD 11)

include(basic.cmake)

include_directories(${BASIC_INC})


add_library(basic STATIC ${BASIC_SRC})

target_link_libraries(basic z)


set(LOG_TEST_FILES
		test/log.cpp
		test/unix_impl.cpp
		)

add_executable(log_test ${LOG_TEST_FILES})

target_link_libraries(log_test
		basic
		z
		)