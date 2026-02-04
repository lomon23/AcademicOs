#include "src/ui/factory/PageFactory.h"

// --- PAGE HEADERS (Тільки тут!) ---
#include "src/modules/dashboard/Dashboard.h"
#include "src/modules/dashboard/DailyPage.h"
#include "src/modules/calendar/ui/CalendarPage.h"
#include "src/modules/todo/ui/ToDoPage.h"
#include "src/modules/analytics/ui/AnalyticsPage.h"
#include "src/modules/finance/ui/FinanceFullPage.h"

// --- MODULE HEADERS ---
#include "src/modules/todo/core/ToDoModule.h"

PageFactory::PageFactory(ToDoModule *todoModule) 
    : m_todoModule(todoModule) {}

QWidget* PageFactory::createPage(const QString &pageId, QWidget *parent) {
    if (pageId == "dashboard") {
        Dashboard *page = new Dashboard(parent);
        // Ін'єкція залежностей
        if (m_todoModule) {
            page->setToDoModule(m_todoModule);
        }
        return page;
    }
    else if (pageId == "calendar") {
        return new CalendarPage(parent);
    }
    else if (pageId == "daily") {
        return new DailyPage(parent);
    }
    else if (pageId == "todo") {
        ToDoPage *page = new ToDoPage(parent);
        if (m_todoModule) {
            page->setModule(m_todoModule);
        }
        return page;
    }
    else if (pageId == "analytics") {
        return new AnalyticsPage(parent);
    }
    else if (pageId == "wallet") {
        return new FinanceFullPage(parent);
    }

    return nullptr;
}