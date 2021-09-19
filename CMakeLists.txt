cmake_minimum_required(VERSION 3.14)
project(reflection LANGUAGES CXX)

# Add binary dir to CMake paths, to support Conan
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

# Root dir of current project, for e.g. include dirs
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

# Check if called as top-level
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  # Ensure -std=c++xx instead of -std=g++xx
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Tidy IDE project tree
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Enable testing
  include(CTest)

  # Create documentation target
  find_package(Doxygen)
  if(Doxygen_FOUND)
    add_subdirectory(docs)
  else()
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()

add_subdirectory(src)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
