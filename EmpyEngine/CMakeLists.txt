project(Engine)

# gather all source files
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_library(${PROJECT_NAME} SHARED ${sources} ${headers})

# export engine symbols
target_compile_definitions(${PROJECT_NAME} PUBLIC 
    -DEMPY_EXPORT
)

# includes directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CONAN_INCLUDE_DIRS}
)

# link directories
target_link_directories(${PROJECT_NAME} PUBLIC 
    ${CONAN_LIB_DIRS}  
)

# link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    ${CONAN_LIBS}     
)

