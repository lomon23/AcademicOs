cmake_minimum_required(VERSION 3.16)

project(AcademicOS VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. ВАЖЛИВО: Додали 'PrintSupport' (потрібен для QCustomPlot, щоб зберігати PDF/PNG)
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql PrintSupport)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(MAIN_SOURCES
    src/main.cpp
)
set(CORE_SOURCES
    src/core/StorageManager.cpp
    src/core/StorageManager.h
    src/core/MetricManager.cpp
    src/core/MetricManager.h
    src/core/todo/Task.h
    src/core/todo/ToDoModule.cpp
    src/core/todo/ToDoModule.h
    
)
set(MODULES_SOURCES
    src/modules/Module.h
    
    src/modules/analytics/AnalyticsSmallWidget.h
    src/modules/analytics/AnalyticsModule.h
    src/modules/analytics/AnalyticsFullPage.h
    src/modules/analytics/AnalyticsSmallWidget.cpp
    src/modules/analytics/AnalyticsModule.cpp
    src/modules/analytics/AnalyticsFullPage.cpp

    src/modules/finance/FinanceModule.cpp
    src/modules/finance/FinanceModule.h
    src/modules/finance/FinanceFullPage.cpp
    src/modules/finance/FinanceFullPage.h
    src/modules/finance/FinanceSmallWidget.cpp
    src/modules/finance/FinanceSmallWidget.h



)
set(UI_SOURCES
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
)
set(PAGE_SOURCES
    src/ui/page/Dashboard.cpp
    src/ui/page/Dashboard.h
    src/ui/page/DailyPage.cpp
    src/ui/page/DailyPage.h

    src/ui/page/todo/TaskItemWidget.cpp
    src/ui/page/todo/TaskItemWidget.h
    src/ui/page/todo/CategoryWidget.cpp
    src/ui/page/todo/CategoryWidget.h
    src/ui/page/todo/ToDoPage.cpp
    src/ui/page/todo/ToDoPage.h
    src/ui/page/todo/ToDoRightBar.cpp
    src/ui/page/todo/ToDoRightBar.h
    src/ui/page/todo/ToDoSmallWidget.cpp
    src/ui/page/todo/ToDoSmallWidget.h

    src/ui/page/calendar/CalendarPage.cpp
    src/ui/page/calendar/CalendarPage.h
)
set(COMPONENT_SOURCES
    src/ui/components/Sidebar.cpp
    src/ui/components/Sidebar.h
    src/ui/components/SidebarItem.cpp
    src/ui/components/SidebarItem.h

    src/ui/components/WelcomeWidget.cpp
    src/ui/components/WelcomeWidget.h
    
)
set(STYLE_RESOURCES
    resources/resources.qrc
    resources/style.qss
)
set(LIB_SOURCES
    libs/qcustomplot/qcustomplot.cpp
    libs/qcustomplot/qcustomplot.h
)

add_executable(AcademicOS
    ${MAIN_SOURCES}
    ${UI_SOURCES}
    ${COMPONENT_SOURCES}
    ${LIB_SOURCES}
    ${STYLE_RESOURCES}
    ${PAGE_SOURCES}
    ${CORE_SOURCES}
    ${MODULES_SOURCES}
)

target_include_directories(AcademicOS PRIVATE libs/qcustomplot)

target_link_libraries(AcademicOS PRIVATE 
    Qt6::Widgets 
    Qt6::Sql 
    Qt6::PrintSupport
)

if(WIN32)
    set_target_properties(AcademicOS PROPERTIES WIN32_EXECUTABLE ON)
endif()