cmake_minimum_required(VERSION 3.13)
project(fastcgilite VERSION 3.9 LANGUAGES CXX)

string(TIMESTAMP BUILD_TIME UTC)

set(CMAKE_CXX_STANDARD 17)

# All our starting point lists
set(SRC_FILES
        "src/log.cpp"
        "src/block.cpp"
        "src/http.cpp"
        "src/protocol.cpp"
        "src/poll.cpp"
        "src/sockets.cpp"
        "src/transceiver.cpp"
        "src/fcgistreambuf.cpp"
        "src/webstreambuf.cpp"
        "src/request.cpp"
        "src/manager.cpp"
        "src/address.cpp")
set(TESTS
        "protocol"
        "http"
        "sockets"
        "transceiver"
        "fcgistreambuf")
set(EXAMPLES
        "helloworld"
        "echo"
        "timer")

# Set up our log level for fastcgi++/log.hpp
if(NOT LOG_LEVEL)
    if(CMAKE_BUILD_TYPE STREQUAL "RELEASE")
        set(LOG_LEVEL 1)
    elseif(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
        set(LOG_LEVEL 3)
    else()
        set(LOG_LEVEL 2)
    endif()
endif()

# We'll need this stuff for sockets stuff
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(SYSTEM "LINUX")
elseif(UNIX)
    set(SYSTEM "UNIX")
elseif(WIN32)
    message(FATAL_ERROR "Windows systems not supported")
    set(SYSTEM "WINDOWS")
else()
    message(FATAL_ERROR "Unknown operating system")
endif()


# Set compile flags for gcc and clang
if(UNIX)
    set(CMAKE_THREAD_PREFER_PTHREAD ON)
endif()

#Set RPATH
if(APPLE)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEFAULT_FASTCGIPP_SKIP_RPATH OFF)
    else()
        set(DEFAULT_FASTCGIPP_SKIP_RPATH ON)
    endif()
    option(FASTCGIPP_SKIP_RPATH "Skip setting up RPATH to full" ${DEFAULT_FASTCGIPP_SKIP_RPATH})
    if(NOT FASTCGIPP_SKIP_RPATH)
        set(CMAKE_SKIP_BUILD_RPATH FALSE)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
        set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
        list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
        if("${isSystemDir}" STREQUAL "-1")
            set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
        endif()
    endif()
endif()

# Our configuration
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp.in"
        "${CMAKE_CURRENT_BINARY_DIR}/include/fastcgi++/config.hpp" @ONLY)

# Build the library itself
add_library(fastcgilite STATIC ${SRC_FILES})
target_compile_options(fastcgilite PUBLIC -fPIC)

set_target_properties(fastcgilite PROPERTIES VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR})

option(FCGILITE_ENABLE_SHARED "Enable building as a shared library" OFF)

if(FCGILITE_ENABLE_SHARED)
    add_library(fastcgipp SHARED ${SRC_FILES})

    add_library(Fastcgipp::fastcgipp ALIAS fastcgipp)

    set_target_properties(fastcgipp PROPERTIES VERSION ${PROJECT_VERSION}
            SOVERSION ${PROJECT_VERSION_MAJOR})
endif()

find_package(Threads REQUIRED)
target_link_libraries(fastcgilite PUBLIC Threads::Threads)
if(FCGILITE_ENABLE_SHARED)
    target_link_libraries(fastcgipp PUBLIC Threads::Threads)
endif()
if(UNIX)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(fastcgilite PRIVATE -Wall -Werror -fno-omit-frame-pointer)
        if(FCGILITE_ENABLE_SHARED)
            target_compile_options(fastcgipp PRIVATE -Wall -Werror -fno-omit-frame-pointer)
        endif()
    endif()
endif(UNIX)

include(GNUInstallDirs)
target_include_directories(fastcgilite PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
if(FCGILITE_ENABLE_SHARED)
    target_include_directories(fastcgipp PUBLIC
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
endif()

set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/fastcgi++)

if(FCGILITE_ENABLE_SHARED)
    install(TARGETS fastcgipp
            EXPORT FastcgippTargets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

    install(EXPORT FastcgippTargets
            FILE FastcgippTargets.cmake
            NAMESPACE Fastcgipp::
            DESTINATION ${INSTALL_CONFIGDIR})

    GNUInstallDirs_get_absolute_install_dir(FASTCGIPP_INCLUDE_DIR CMAKE_INSTALL_INCLUDEDIR)

    #Create a ConfigVersion.cmake file
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/FastcgippConfigVersion.cmake
            COMPATIBILITY SameMajorVersion)

    configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/FastcgippConfig.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/FastcgippConfig.cmake
            INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
            PATH_VARS FASTCGIPP_INCLUDE_DIR)

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/FastcgippConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/FastcgippConfigVersion.cmake
            DESTINATION ${INSTALL_CONFIGDIR})

    install(DIRECTORY
            "${CMAKE_CURRENT_SOURCE_DIR}/include/fastcgi++"
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    # Install the config header file
    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/include/fastcgi++/config.hpp"
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fastcgi++)
endif()

# All the test stuff
option(FCGILITE_ADDITIONAL_TARGETS "Enable targets for examples, tests, and docs" OFF)
if(FCGILITE_ADDITIONAL_TARGETS)
    enable_testing()
    foreach(UNITTEST IN LISTS TESTS)
        add_executable(${UNITTEST}_fcgitest EXCLUDE_FROM_ALL tests/${UNITTEST}.cpp)
        target_link_libraries(${UNITTEST}_fcgitest PRIVATE fastcgilite)
        add_test("${UNITTEST}" ${UNITTEST}_fcgitest)
        list(APPEND TEST_TARGET ${UNITTEST}_fcgitest)
    endforeach()
    add_custom_target(fcgitests DEPENDS ${TEST_TARGET})

    # Examples
    foreach(EXAMPLE IN LISTS EXAMPLES)
        add_executable(${EXAMPLE}.fcgi EXCLUDE_FROM_ALL examples/${EXAMPLE}.cpp)
        target_link_libraries(${EXAMPLE}.fcgi PRIVATE fastcgilite)
        list(APPEND EXAMPLE_TARGETS ${EXAMPLE}.fcgi)
    endforeach()
    add_custom_target(fcgiexamples DEPENDS ${EXAMPLE_TARGETS})

    # And finally the documentation
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        configure_file(
                "${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in"
                "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)
        add_custom_target(
                fcgidoc
                "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                COMMENT "Generating API documentation with Doxygen" VERBATIM)
        install(
                DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc/"
                DESTINATION ${CMAKE_INSTALL_DOCDIR}
                OPTIONAL)
    endif(DOXYGEN_FOUND)
endif()
