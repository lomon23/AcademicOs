cmake_minimum_required(VERSION 3.16)

# Назва твого проекту
project(AcademicOS VERSION 0.1 LANGUAGES CXX)

# Вказуємо стандарт C++ (C++17 - хороший вибір, сучасний і стабільний)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Шукаємо Qt6 (обов'язкові модулі: Widgets та Sql)
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

# Вмикаємо автоматичну обробку файлів Qt (MOC, UIC, RCC)
# Це магія, яка дозволяє Qt працювати (сигнали, слоти, форми)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Вказуємо всі файли коду (можна вручну, або через file(GLOB...))
# Для початку пропишемо явно, щоб ти бачив що де:
add_executable(AcademicOS
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
    src/ui/MainWindow.ui
    # Тут потім допишеш інші файли, наприклад src/core/DatabaseManager.cpp
)

# Підключаємо бібліотеки Qt до твого додатку
target_link_libraries(AcademicOS PRIVATE Qt6::Widgets Qt6::Sql)

# Це для того, щоб на Windows воно запускалось не як консоль, а як вікно
# (На Linux це ігнорується, але корисно мати)
set_target_properties(AcademicOS PROPERTIES WIN32_EXECUTABLE ON)