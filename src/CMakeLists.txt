add_custom_target(copy_data ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data)

add_executable(CppTest
    Main.cpp)
add_dependencies(CppTest copy_data)
target_link_libraries(CppTest PRIVATE glfw glad glm)
