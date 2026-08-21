cmake_minimum_required(VERSION 3.19)
project(tray LANGUAGES C CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(CMAKE_AUTORCC ON)

qt_add_executable(fw-fanctrl-tray
    WIN32 MACOSX_BUNDLE
    main.cpp
    fanctrl.c
    assets/resources.qrc
)

target_link_libraries(fw-fanctrl-tray
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS fw-fanctrl-tray
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET fw-fanctrl-tray
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

