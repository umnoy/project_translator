cmake_minimum_required(VERSION 3.16)

project(c_project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Пути к ONNX Runtime (уточните пути в зависимости от вашей системы)
set(ONNXRUNTIME_LIB_DIR "/home/kali/Desktop/project_translator/src/core/lib") # Замените на актуальный путь
set(ONNXRUNTIME_INCLUDE_DIR "/home/kali/Desktop/project_translator/src/core/include") # Замените на актуальный путь

# Проверка существования директорий
if(NOT EXISTS "${ONNXRUNTIME_INCLUDE_DIR}")
    message(FATAL_ERROR "ONNX Runtime include directory not found: ${ONNXRUNTIME_INCLUDE_DIR}")
endif()
if(NOT EXISTS "${ONNXRUNTIME_LIB_DIR}")
    message(FATAL_ERROR "ONNX Runtime lib directory not found: ${ONNXRUNTIME_LIB_DIR}")
endif()

# Подключаем nlohmann/json
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ./../core/src/tokenizer/tokenizer.cpp
    ./../core/src/translator/translator.cpp
)

add_executable(c_project ${PROJECT_SOURCES})

target_include_directories(c_project PRIVATE
    ${CMAKE_SOURCE_DIR}/../../core
    ${ONNXRUNTIME_INCLUDE_DIR}
)

target_link_libraries(c_project PRIVATE
    Qt5::Widgets
    nlohmann_json::nlohmann_json
)

# Ищем библиотеку ONNX Runtime
find_library(ONNXRUNTIME_LIBRARY
    NAMES onnxruntime libonnxruntime
    PATHS ${ONNXRUNTIME_LIB_DIR}
    NO_DEFAULT_PATH
)
if(NOT ONNXRUNTIME_LIBRARY)
    message(FATAL_ERROR "ONNX Runtime library not found in ${ONNXRUNTIME_LIB_DIR}")
endif()

target_link_libraries(c_project PRIVATE ${ONNXRUNTIME_LIBRARY})

set_target_properties(c_project PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS c_project
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
