function(mcli_compute_version OUT_VERSION OUT_IS_DIRTY)
    # Default if git is not available or no tags exist
    set(_version "0.0.0")
    set(_dirty "0")

    find_package(Git QUIET)
    if (GIT_FOUND)
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" describe --tags --abbrev=0
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE _tag
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )

        if (_tag)
            string(REGEX REPLACE "^v" "" _version "${_tag}")
        endif()

        # Detect dirty working tree (uncommitted changes)
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" status --porcelain
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE _status
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if (_status)
            set(_dirty "1")
        endif()
    endif()

    set(${OUT_VERSION} "${_version}" PARENT_SCOPE)
    set(${OUT_IS_DIRTY} "${_dirty}" PARENT_SCOPE)
endfunction()
