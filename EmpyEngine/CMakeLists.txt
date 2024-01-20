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

# copy .dll files
if (MSVC)
    foreach(directory ${CONAN_BIN_DIRS})
        file(GLOB dllfiles ${directory}/*.dll)
        foreach(dllfile ${dllfiles})
            if (EXISTS ${dllfile})
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                    ${dllfile} ${EXECUTABLE_OUTPUT_PATH})
            endif()
        endforeach()
    endforeach()
endif(MSVC)

# copy resources
if(EXISTS ${CMAKE_SOURCE_DIR}/Resources)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND 
        ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_SOURCE_DIR}/Resources
        ${EXECUTABLE_OUTPUT_PATH}/Resources
    )
else()
    message(WARNING "[WARNING] no resource directory!")
endif()

# copy script direcotory
if(EXISTS ${CMAKE_SOURCE_DIR}/EmpyScript)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND 
        ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_SOURCE_DIR}/EmpyScript
        ${EXECUTABLE_OUTPUT_PATH}/Resources/Scripts
    )
else()
    message(WARNING "[WARNING] no script directory!")
endif()
