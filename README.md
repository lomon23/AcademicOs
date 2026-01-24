# 🎓 Academic OS

> **Your personal operating system for productivity, learning, and life management.**
> *Built with C++ & Qt.*

![Dashboard Screenshot](assets/dashboard_preview.png)
*(Note: Please update the path above with your actual screenshot)*

## About The Project

**Academic OS** is not just another to-do list. It is a comprehensive desktop ecosystem designed for students who want to treat their life and studies as a managed project. 

Born from the need to have a fast, native, and distraction-free tool, Academic OS combines task management, financial tracking, and personal analytics into one cohesive interface.

### ✨ Key Features

* **📊 Dashboard Hub:** A customizable overview of your day. See your tasks, financial balance, and activity charts at a glance.
* **✅ Advanced To-Do System:** * Infinite task nesting (sub-tasks).
    * Color-coded categories.
    * **Live Progress Tracking:** Visual bars that update in real-time.
* **📈 Analytics Engine:** Visualizes your productivity. Track your habits, sleep, or study hours with dynamic graphs (powered by QCustomPlot).
* **💳 Wallet Manager:** Keep track of your finances, set goals, and monitor expenses.
* **📅 Academic Calendar:** A clean, dark-themed calendar to keep track of exams and deadlines.
* **📝 Daily Check-in:** A routine module to log your state, budget, and mood.

---

## Gallery

| **Task Management** | **Analytics & Progress** |
|:---:|:---:|
| ![ToDo](assets/todo.png) | ![Analytics](assets/analytics.png) |
| *Deep nesting & color coding* | *Real-time data visualization* |

---

## Tech Stack

* **Core:** C++17
* **GUI Framework:** Qt 6 (Widgets)
* **Build System:** CMake
* **Data Storage:** JSON (Custom serialization)
* **Charting:** QCustomPlot

## Getting Started

### Prerequisites
* C++ Compiler (GCC/Clang/MSVC)
* Qt 6 SDK
* CMake

### Installation

1.  Clone the repository:
    ```bash
    git clone [https://github.com/your-username/AcademicOS.git](https://github.com/your-username/AcademicOS.git)
    cd AcademicOS
    ```

2.  Create build directory:
    ```bash
    mkdir build && cd build
    ```

3.  Build the project:
    ```bash
    cmake ..
    make
    ```

4.  Run:
    ```bash
    ./AcademicOS
    ```

## 🗺️ Roadmap

- [x] **Core Architecture** (Module System, JSON Storage)
- [x] **ToDo Module** (CRUD, Nesting, Progress Bars)
- [x] **Dashboard** (Widget Grid, Shortcuts)
- [ ] **Analytics 2.0** (Link Daily Inputs to Charts)
- [ ] **Wallet 2.0** (Transaction History)
- [ ] **Settings Page**
- [ ] **MacOS & Windows Deploy**

---

## 👤 Author

**Lomon** - [GitHub Profile](https://github.com/lomon23)

---
*Developed with ❤️ and C++*