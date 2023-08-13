message("${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB src "*.cpp")
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${src})
