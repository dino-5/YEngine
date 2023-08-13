add_subdirectory(glfw)
add_subdirectory(glm)	
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE glm)