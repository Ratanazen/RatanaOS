#include "../../apps/common/ratana_ui.h"

#include <QApplication>
#include <QDate>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>

class DesktopShellWindow : public QMainWindow {
 public:
  DesktopShellWindow() {
    setWindowTitle("RatanaOS Desktop");
    resize(1600, 960);

    auto *surface = new QFrame;
    surface->setObjectName("ShellSurface");
    setCentralWidget(surface);

    auto *sidebar = new QFrame;
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(18, 18, 18, 18);
    sidebarLayout->setSpacing(12);
    auto *title = new QLabel("RatanaOS");
    title->setObjectName("HeroTitle");
    sidebarLayout->addWidget(title);
    sidebarLayout->addWidget(new QLabel("Wayland-first workspace"));
    for (const QString &entry : {"Launcher", "Files", "Settings", "Software", "Monitor", "Assistant"}) {
      sidebarLayout->addWidget(RatanaUI::makePillButton(entry, entry == "Launcher"));
    }
    sidebarLayout->addStretch();
    sidebarLayout->addWidget(new QLabel("Session: Focus"));

    auto *content = new QWidget;
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(20);

    auto *hero = RatanaUI::makePanel(
        "Desktop Overview",
        QString("Friday, %1").arg(QDate::currentDate().toString("MMMM d, yyyy")));
    auto *heroLayout = qobject_cast<QVBoxLayout *>(hero->layout());
    heroLayout->addWidget(RatanaUI::makeMetricRow("Notifications", "3", "all low priority"));
    heroLayout->addWidget(RatanaUI::makeMetricRow("Windows", "8", "2 pinned workspaces"));
    heroLayout->addWidget(RatanaUI::makeMetricRow("Updates", "12", "ready for tonight"));
    contentLayout->addWidget(hero);

    auto *grid = new QGridLayout;
    grid->setSpacing(18);
    grid->addWidget(RatanaUI::makeCard(
        "Workspace",
        "Project Studio",
        "Pinned coding session with terminal, editor, docs, and assistant stacked for fast context switching.",
        QColor("#b96f31")), 0, 0);
    grid->addWidget(RatanaUI::makeCard(
        "System health",
        "Compositor stable",
        "Frame pacing and memory pressure are in the expected range for a full workday load.",
        QColor("#2b7a6e")), 0, 1);
    grid->addWidget(RatanaUI::makeCard(
        "Quick actions",
        "Prepare for presentation",
        "One action can mute alerts, raise brightness, and switch to external display layout.",
        QColor("#4f6fa3")), 1, 0);
    grid->addWidget(RatanaUI::makeCard(
        "Activity",
        "Software Center synced",
        "Three productivity tools and one firmware patch were validated this morning.",
        QColor("#87516e")), 1, 1);
    contentLayout->addLayout(grid);

    auto *aside = RatanaUI::makeSectionList("Today", {
      "09:30 Design review in Focus Shield",
      "13:00 Install preview desktop updates",
      "16:45 Export session report"
    });

    RatanaUI::installWindowScaffold(surface, sidebar, content, aside);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  DesktopShellWindow window;
  window.show();
  return app.exec();
}
