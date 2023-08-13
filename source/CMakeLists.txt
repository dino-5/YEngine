add_subdirectory(sandbox)
add_subdirectory(third_party)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE third_party/glfw/include)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE third_party)