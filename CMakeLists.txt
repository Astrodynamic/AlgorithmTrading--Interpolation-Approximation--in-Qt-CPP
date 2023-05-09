cmake_minimum_required(VERSION 3.15)

project(AlgorithmTrading VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS
    Widgets
    LinguistTools
    Charts
    Concurrent
    Core
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets
    LinguistTools
    Charts
    Concurrent
    Core
)

set(TS_FILES translate/${PROJECT_NAME}_ru_RU.ts)

set(CMAKE_AUTOUIC_SEARCH_PATHS forms)

include_directories(
    ${CMAKE_SOURCE_DIR}/model/interpolation
    ${CMAKE_SOURCE_DIR}/model/approximation
    ${CMAKE_SOURCE_DIR}/model/abstractmodel
    ${CMAKE_SOURCE_DIR}/controller
    ${CMAKE_SOURCE_DIR}/controller/date_time_dialog
    ${CMAKE_SOURCE_DIR}/charts
    ${CMAKE_SOURCE_DIR}/parser
    ${CMAKE_SOURCE_DIR}/table
)

set(HEADERS
    ${CMAKE_SOURCE_DIR}/model/abstractmodel/abstractmodel.h
    ${CMAKE_SOURCE_DIR}/model/approximation/approximation.h
    ${CMAKE_SOURCE_DIR}/model/interpolation/interpolation.h
    ${CMAKE_SOURCE_DIR}/table/tablecontroller.h
    ${CMAKE_SOURCE_DIR}/controller/controller.h
    ${CMAKE_SOURCE_DIR}/controller/date_time_dialog/datetimedialog.h
    ${CMAKE_SOURCE_DIR}/charts/chartarea.h
    ${CMAKE_SOURCE_DIR}/charts/simplechart.h
    ${CMAKE_SOURCE_DIR}/parser/parser.h
)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/model/interpolation/interpolation.cc
    ${CMAKE_SOURCE_DIR}/model/approximation/approximation.cc
    ${CMAKE_SOURCE_DIR}/model/abstractmodel/abstractmodel.cc
    ${CMAKE_SOURCE_DIR}/table/tablecontroller.cc
    ${CMAKE_SOURCE_DIR}/controller/controller.cc
    ${CMAKE_SOURCE_DIR}/controller/date_time_dialog/datetimedialog.cc
    ${CMAKE_SOURCE_DIR}/charts/chartarea.cc
    ${CMAKE_SOURCE_DIR}/charts/simplechart.cc
    ${CMAKE_SOURCE_DIR}/parser/parser.cc
)

set(FORMS
    ${CMAKE_SOURCE_DIR}/forms/view.ui
)

set(PROJECT_SOURCES
    ${HEADERS}
    ${SOURCES}
    ${FORMS}
    ${TS_FILES}
    ${CMAKE_SOURCE_DIR}/main.cc
)

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE appicon.png)
    set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/data/icon/appicon.png")
    set_source_files_properties(${app_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${app_icon_macos}
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
        set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
    -Wall
    -Werror
    -Wextra
    -Wpedantic
    -fsanitize=address
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Core
    -fsanitize=address
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

# Add cppcheck
find_program(CPPCHECK cppcheck)

if(CPPCHECK)
    message(STATUS "cppcheck found: ${CPPCHECK}")
    list(
        APPEND CPPCHECK_ARGS
        "--enable=all"
        "--inconclusive"
        "--quiet"
        "--language=c++"
        "--std=c++17"
        "--suppress=unusedFunction"
        "--suppress=missingInclude"
        "--suppress=unusedStructMember"
        "--verbose"
    )
    add_custom_target(
        cppcheck
        COMMAND ${CPPCHECK} ${CPPCHECK_ARGS} ${HEADERS} ${SOURCES}
    )
else()
    message(STATUS "cppcheck not found")
endif()

# Add clang-format
find_program(CLANG_FORMAT clang-format)

if(CLANG_FORMAT)
    message(STATUS "clang-format found: ${CLANG_FORMAT}")
    list(
        APPEND CLANG_ARGS
        "-i"
        "-style=Google"
        "--verbose"
    )
    add_custom_target(
        clang-format
        COMMAND ${CLANG_FORMAT} ${CLANG_ARGS} ${HEADERS} ${SOURCES}
    )
else()
    message(STATUS "clang-format not found")
endif()