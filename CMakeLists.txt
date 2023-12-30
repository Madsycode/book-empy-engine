cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# build output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/lib)
set(CMAKE_PDB_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH $<0:>${CMAKE_BINARY_DIR}/bin)

# cpp standard options
set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 17)
project(EmpyEngine)

# enable console logging
if(${CMAKE_BUILD_TYPE} MATCHES Debug)
  add_definitions(-DEMPY_ENABLE_LOG)
endif()

# project subdirectories
add_subdirectory(EmpyEngine)
add_subdirectory(EmpyEditor)
#add_subdirectory(EmpyGame)

