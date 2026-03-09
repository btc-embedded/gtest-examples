cmake_minimum_required(VERSION 3.16)
project(seat_heating_controller_cpp LANGUAGES C CXX)

# C/C++ standard and compiler settings
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler warnings
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
  add_compile_options(/W4)
endif()

# Source and include directories
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Sources and headers
set(SOURCES
  "${SRC_DIR}/seat_heating_controller.cpp"
)

set(PUBLIC_HEADERS
  "${INC_DIR}/seat_heating_controller.h"
)

# --- Static library target ---
add_library(seat_heating STATIC
  ${SOURCES}
  ${PUBLIC_HEADERS}
)

# Include paths
# BUILD_INTERFACE: used while building the project
# INSTALL_INTERFACE: used after installation
target_include_directories(seat_heating
  PUBLIC
    $<BUILD_INTERFACE:${SRC_DIR}>
    $<BUILD_INTERFACE:${INC_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# PIC is useful even for static libraries on some platforms
set_target_properties(seat_heating PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Provide an alias target for cleaner linking in other projects
add_library(seat_heating::seat_heating ALIAS seat_heating)

# --- Installation rules ---
include(GNUInstallDirs)

# Install the library itself
install(TARGETS seat_heating
  EXPORT seat_heatingTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install public headers only (not private headers)
install(FILES ${PUBLIC_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export target for find_package(seat_heating CONFIG REQUIRED)
install(EXPORT seat_heatingTargets
  NAMESPACE seat_heating::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/seat_heating
)

# ============================================================
# Google Test Integration
# ============================================================

option(BUILD_TESTING "Build the testing tree" ON)

if(BUILD_TESTING)
  enable_testing()
  
  # Fetch GoogleTest from GitHub
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
    GIT_SHALLOW    TRUE
    GIT_SUBMODULES ""
  )
  
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  
  # Source and include directories
  set(TST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tst")

  # Make GoogleTest available
  FetchContent_MakeAvailable(googletest)
  
  # Include GoogleTest CMake functions
  include(GoogleTest)
  
  # Create test executable
  add_executable(seat_heating_tests
    ${TST_DIR}/test_seat_heating_controller.cpp
  )
  
  # Link test executable with the library and Google Test
  target_link_libraries(seat_heating_tests
    PRIVATE
      seat_heating::seat_heating
      gtest
      gmock
      gtest_main
  )
  
  # Include directories for tests
  target_include_directories(seat_heating_tests
    PRIVATE
      ${SRC_DIR}
      ${INC_DIR}
  )
  
  # Discover tests for CTest
  gtest_discover_tests(seat_heating_tests)
  
  # Add custom target for running tests
  add_custom_target(run_cpp_tests
    COMMAND seat_heating_tests
    DEPENDS seat_heating_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running Google Tests..."
  )
  
  # Optional: Add coverage support (requires GCC/Clang)
  option(ENABLE_COVERAGE "Enable code coverage analysis" OFF)
  
  if(ENABLE_COVERAGE)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
      target_compile_options(seat_heating PRIVATE --coverage)
      target_link_options(seat_heating PRIVATE --coverage)
      target_compile_options(seat_heating_tests PRIVATE --coverage)
      target_link_options(seat_heating_tests PRIVATE --coverage)
      
      # Add custom target for generating coverage report
      find_program(GCOVR_PATH gcovr)
      if(GCOVR_PATH)
        add_custom_target(coverage
          COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
          COMMAND ${GCOVR_PATH} -r ${CMAKE_SOURCE_DIR}
                  --html --html-details
                  -o ${CMAKE_BINARY_DIR}/coverage/index.html
                  --exclude-unreachable-branches
                  --exclude-throw-branches
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
          DEPENDS run_cpp_tests
          COMMENT "Generating code coverage report..."
        )
      endif()
    else()
      message(WARNING "Code coverage is only supported with GCC or Clang")
    endif()
  endif()
  
endif()

# Print build information
message(STATUS "========================================")
message(STATUS "Seat Heating Controller - Build Configuration")
message(STATUS "========================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build testing: ${BUILD_TESTING}")
if(BUILD_TESTING AND ENABLE_COVERAGE)
  message(STATUS "Code coverage: ENABLED")
endif()
message(STATUS "========================================")

