cmake_minimum_required(VERSION 3.16)

project(AcademicOS VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. ВАЖЛИВО: Додали 'PrintSupport' (потрібен для QCustomPlot, щоб зберігати PDF/PNG)
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql PrintSupport)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(AcademicOS
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h

    libs/qcustomplot/qcustomplot.cpp
    libs/qcustomplot/qcustomplot.h

    src/ui/components/Sidebar.cpp
    src/ui/components/Sidebar.h
)

# 3. Додаємо шлях до папки, щоб можна було писати #include "qcustomplot.h"
target_include_directories(AcademicOS PRIVATE libs/qcustomplot)

# 4. ВАЖЛИВО: Лінкуємо модуль PrintSupport
target_link_libraries(AcademicOS PRIVATE 
    Qt6::Widgets 
    Qt6::Sql 
    Qt6::PrintSupport
)

if(WIN32)
    set_target_properties(AcademicOS PROPERTIES WIN32_EXECUTABLE ON)
endif()