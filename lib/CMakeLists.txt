include_directories(${PROJECT_SOURCE_DIR}/lib/
                    ${PROJECT_SOURCE_DIR})


include("cmake/utils.cmake")
set(CMAKE_CXX_FLAGS "std=c++11 -Wall -O2 -lpthread ${CMAKE_CXX_FLAGS}")

list(APPEND LIB_SRC
    "base/slice.cc")
list(APPEND LIB_INCLUDE
    "include/slice.h"
    "include/macros.h"
    "include/assert.h")

configure_headers(${LIB_INCLUDE})
add_library(lib ${LIB_SRC})
target_link_libraries(lib)

enable_testing()
function(lib_test test_file)
    set(extra_args ${ARGN})
    get_filename_component(test_target_name "${test_file}" NAME_WE)
    add_executable("${test_target_name}" "${test_file}")
    target_link_libraries("${test_target_name}" gtest_main "${extra_args}")
    add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
endfunction(lib_test)

lib_test("base/slice_test.cc")
