cmake_minimum_required(VERSION 3.18.0)

project(NanotubesAnalysis VERSION 1.0 LANGUAGES C CXX)

set(BUILD_SHARED_LIBS NO)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR}bin)

set(PROJECT_SRC
    main.cpp app.cpp
    )

list(TRANSFORM PROJECT_SRC PREPEND "src/")

add_subdirectory("external/raylib")

add_executable(${PROJECT_NAME} ${PROJECT_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -d)

target_link_libraries(${PROJECT_NAME} PUBLIC raylib)

target_include_directories(${PROJECT_NAME} PUBLIC "external/raylib-cpp/include")
target_include_directories(${PROJECT_NAME} PUBLIC "external/raylib/src")
target_include_directories(${PROJECT_NAME} PUBLIC "external/raylib/src/extras")


