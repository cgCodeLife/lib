set(CMAKE_SHARED_LINKER_FLAGS "-Wl, --version-script=${CMAKE_CURRENT_SOURCE_DIR}/exported_symbols.lds")

include_directories(
    ${PROJECT_SOURCE_DIR}/lib
    ${PROJECT_BINARY_DIR}/include
    ${PROJECT_BINARY_DIR}/include/lib
    ${PROJECT_SOURCE_DIR}/third/gtest
    ${PROJECT_SOURCE_DIR}/third/gtet/googletest/include
    ${PROJECT_SOURCE_DIR}/third/gtet/googlemock/include
    ${PROJECT_SOURCE_DIR}/third
    )

#add_subdirectory(algorithm)
add_subdirectory(base)
#add_subdirectory(container)
#add_subdirectory(crontab)
#add_subdirectory(io)
#add_subdirectory(log)
#add_subdirectory(mem)
#add_subdirectory(net)
#add_subdirectory(string)
#add_subdirectory(system)
#add_subdirectory(thread)
