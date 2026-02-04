cmake_minimum_required(VERSION 3.16)

project(AcademicOS VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Qt Packages
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql PrintSupport)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 2. Includes (Важливо для нової структури)
include_directories(
    ${CMAKE_SOURCE_DIR}
    libs/qcustomplot
)

# --- 3. SOURCES DEFINITIONS ---

set(MAIN_SOURCES
    src/main.cpp
)

set(GLOBAL_CORE_SOURCES
    src/core/StorageManager.cpp
    src/core/StorageManager.h
    src/core/MetricManager.cpp
    src/core/MetricManager.h
    src/modules/Module.h
)

set(GLOBAL_UI_SOURCES
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
    
    # Factory
    src/ui/factory/PageFactory.cpp
    src/ui/factory/PageFactory.h
    
    # Components (Sidebar & Widgets)
    src/ui/components/sidebar/Sidebar.cpp
    src/ui/components/sidebar/Sidebar.h
    src/ui/components/sidebar/SidebarItem.cpp
    src/ui/components/sidebar/SidebarItem.h
    src/ui/components/widgets/WelcomeWidget.cpp
    src/ui/components/widgets/WelcomeWidget.h
)

# === MODULES ===

set(MODULE_TODO_SOURCES
    # Core
    src/modules/todo/core/ToDoModule.cpp
    src/modules/todo/core/ToDoModule.h
    src/modules/todo/core/Task.h
    # UI
    src/modules/todo/ui/ToDoPage.cpp
    src/modules/todo/ui/ToDoPage.h
    src/modules/todo/ui/ToDoRightBar.cpp
    src/modules/todo/ui/ToDoRightBar.h
    src/modules/todo/ui/ToDoListWidget.cpp
    src/modules/todo/ui/ToDoListWidget.h
    # Widgets
    src/modules/todo/widgets/TaskItemWidget.cpp
    src/modules/todo/widgets/TaskItemWidget.h
    src/modules/todo/widgets/ToDoSmallWidget.cpp
    src/modules/todo/widgets/ToDoSmallWidget.h
)

set(MODULE_FINANCE_SOURCES
    # Core
    src/modules/finance/core/FinanceModule.cpp
    src/modules/finance/core/FinanceModule.h
    # UI
    src/modules/finance/ui/FinanceFullPage.cpp
    src/modules/finance/ui/FinanceFullPage.h
    src/modules/finance/ui/FinanceSidebar.cpp
    src/modules/finance/ui/FinanceSidebar.h
    src/modules/finance/ui/FinanceWorkspace.cpp
    src/modules/finance/ui/FinanceWorkspace.h
    # Widgets
    src/modules/finance/widgets/FinanceSmallWidget.cpp
    src/modules/finance/widgets/FinanceSmallWidget.h
)

set(MODULE_ANALYTICS_SOURCES
    # Core
    src/modules/analytics/core/AnalyticsService.cpp
    src/modules/analytics/core/AnalyticsService.h
    # UI
    src/modules/analytics/ui/AnalyticsPage.cpp
    src/modules/analytics/ui/AnalyticsPage.h
    src/modules/analytics/ui/AnalyticsHeader.cpp
    src/modules/analytics/ui/AnalyticsHeader.h
    src/modules/analytics/ui/ChartPanel.cpp
    src/modules/analytics/ui/ChartPanel.h
    src/modules/analytics/ui/MetricsPanel.cpp
    src/modules/analytics/ui/MetricsPanel.h
)

set(MODULE_DASHBOARD_SOURCES
    src/modules/dashboard/Dashboard.cpp
    src/modules/dashboard/Dashboard.h
    src/modules/dashboard/DailyPage.cpp
    src/modules/dashboard/DailyPage.h
)

set(MODULE_CALENDAR_SOURCES
    src/modules/calendar/ui/CalendarPage.cpp
    src/modules/calendar/ui/CalendarPage.h
)

set(LIB_SOURCES
    libs/qcustomplot/qcustomplot.cpp
    libs/qcustomplot/qcustomplot.h
)

set(STYLE_RESOURCES
    resources/resources.qrc
    resources/style.qss # Додається в QRC, але можна і тут тримати
)

# 4. Executable
add_executable(AcademicOS
    ${MAIN_SOURCES}
    ${GLOBAL_CORE_SOURCES}
    ${GLOBAL_UI_SOURCES}
    ${MODULE_TODO_SOURCES}
    ${MODULE_FINANCE_SOURCES}
    ${MODULE_ANALYTICS_SOURCES}
    ${MODULE_DASHBOARD_SOURCES}
    ${MODULE_CALENDAR_SOURCES}
    ${LIB_SOURCES}
    ${STYLE_RESOURCES}
)

# 5. Libraries
target_link_libraries(AcademicOS PRIVATE 
    Qt6::Widgets 
    Qt6::Sql 
    Qt6::PrintSupport
)

if(WIN32)
    set_target_properties(AcademicOS PROPERTIES WIN32_EXECUTABLE ON)
endif()