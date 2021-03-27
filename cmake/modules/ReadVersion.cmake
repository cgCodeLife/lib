# Read version from version.h header file.

function(read_version version_var)
    file(READ "${CMAKE_BINARY_DIR}/include/lib/version.h" version_header_file)
    foreach(component MAJOR MINOR PATCH)
        string(REGEX MATCH "#define LIB_${component} ([0-9]+)" _ ${version_header_file})
        set(LIB_VERSION_${component} ${CMAKE_MATCH_1})
    endforeach()
    set(${version_var} "${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}" PARENT_SCOPE)
endfunction()
