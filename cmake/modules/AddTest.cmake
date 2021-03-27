# Add test functions.

function(lib_test test_file lib)
    set(extra_args ${ARGN})
    get_filename_component(test_target_name ${test_file} NAME_WE)
    set_source_files_properties(${test_file} PROPERTIES COMPILE_FLAGS "-fno-access-control")

    add_executable(${test_target_name} ${test_file})
    target_link_libraries(${test_target_name}
                          ${lib}
                          gtest_main
                          gmock_main)
    add_test(NAME ${test_target_name} COMMAND ${test_target_name})
endfunction()
