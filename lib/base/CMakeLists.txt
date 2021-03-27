list(APPEND SRCS  slice.cc)
list(APPEND LIBS gtest)
add_library(lib_base STATIC ${SRCS})
target_link_libraries(lib_base
                    ${LIBS})
add_library(lib_base_ut STATIC ${SRCS})
target_link_libraries(lib_base_ut
                    ${LIBS})
lib_test("slice_test.cc" lib_base_ut)
