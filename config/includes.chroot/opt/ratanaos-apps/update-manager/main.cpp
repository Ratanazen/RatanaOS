#include "../common/ratana_ui.h"

#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QComboBox>
#include <QListWidget>

class UpdateManagerWindow : public QMainWindow {
 public:
  UpdateManagerWindow() {
    setWindowTitle("RatanaOS Update Manager");
    resize(1320, 840);

    auto *surface = new QWidget;
    setCentralWidget(surface);
    auto *mainLayout = new QHBoxLayout(surface);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Sidebar
    auto *sidebar = new QFrame;
    sidebar->setObjectName("DesktopPanel");
    sidebar->setFixedWidth(250);
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(18, 18, 18, 18);
    auto *title = new QLabel("Update Manager");
    title->setObjectName("HeroTitle");
    sidebarLayout->addWidget(title);
    
    QStringList steps = {"Pending", "History", "Channels", "Rollback"};
    for (const QString &step : steps) {
      sidebarLayout->addWidget(RatanaUI::makePillButton(step, step == "Pending"));
    }
    sidebarLayout->addStretch();
    mainLayout->addWidget(sidebar);

    // Content Area
    auto *contentArea = new QWidget;
    auto *contentLayout = new QVBoxLayout(contentArea);
    
    auto *stackedWidget = new QStackedWidget;
    
    // Page 1: Pending Updates
    auto *pagePending = new QWidget;
    auto *pendingLayout = new QVBoxLayout(pagePending);
    
    auto *status = RatanaUI::makePanel("Pending Rollout", "RatanaOS update daemon has found updates.");
    auto *statusLayout = qobject_cast<QVBoxLayout *>(status->layout());
    statusLayout->addWidget(RatanaUI::makeMetricRow("Packages", "3", "1 Security, 2 Standard"));
    statusLayout->addWidget(RatanaUI::makeMetricRow("Estimated downtime", "4 min", "single reboot"));
    auto *progress = new QProgressBar;
    progress->setValue(0);
    statusLayout->addWidget(progress);
    statusLayout->addWidget(RatanaUI::makePillButton("Install Updates", true));
    pendingLayout->addWidget(status);
    
    auto *timeline = RatanaUI::makePanel("Next Actions");
    auto *timelineLayout = qobject_cast<QVBoxLayout *>(timeline->layout());
    timelineLayout->addWidget(new QLabel("1. Snapshot user space and root via Btrfs"));
    timelineLayout->addWidget(new QLabel("2. Apply security patches"));
    timelineLayout->addWidget(new QLabel("3. Refresh Ratana-Terminal"));
    timelineLayout->addStretch();
    pendingLayout->addWidget(timeline);
    stackedWidget->addWidget(pagePending);

    // Page 2: History (Mock dpkg.log)
    auto *pageHistory = new QWidget;
    auto *histLayout = new QVBoxLayout(pageHistory);
    histLayout->addWidget(RatanaUI::makePanel("Update History", "Recent changes from /var/log/dpkg.log"));
    auto *histList = new QListWidget;
    histList->addItem("2026-07-17 10:00:01 install ratana-desktop 2.0.1");
    histList->addItem("2026-07-17 10:00:05 upgrade linux-image-amd64 6.1.76-2");
    histList->addItem("2026-07-16 08:30:00 install flatpak 1.14.4");
    histLayout->addWidget(histList);
    stackedWidget->addWidget(pageHistory);

    // Page 3: Channels
    auto *pageChannels = new QWidget;
    auto *chanLayout = new QVBoxLayout(pageChannels);
    chanLayout->addWidget(RatanaUI::makePanel("Release Channel", "Select how quickly you receive updates."));
    auto *chanCombo = new QComboBox;
    chanCombo->addItems({"Stable (Recommended)", "Beta (Early Access)", "Nightly (Developers)"});
    chanLayout->addWidget(new QLabel("Current Channel:"));
    chanLayout->addWidget(chanCombo);
    chanLayout->addWidget(RatanaUI::makePillButton("Apply Channel Change", true));
    chanLayout->addStretch();
    stackedWidget->addWidget(pageChannels);

    // Page 4: Rollback
    auto *pageRollback = new QWidget;
    auto *rollLayout = new QVBoxLayout(pageRollback);
    rollLayout->addWidget(RatanaUI::makePanel("System Rollback", "Restore your system to a previous state using Btrfs snapshots."));
    auto *snapList = new QListWidget;
    snapList->addItem("Snapshot 45 - Before ratana-desktop update (2026-07-17)");
    snapList->addItem("Snapshot 44 - Weekly backup (2026-07-14)");
    snapList->addItem("Snapshot 43 - Base Installation (2026-07-10)");
    rollLayout->addWidget(snapList);
    rollLayout->addWidget(RatanaUI::makePillButton("Restore Selected Snapshot", true));
    stackedWidget->addWidget(pageRollback);

    contentLayout->addWidget(stackedWidget);
    mainLayout->addWidget(contentArea, 1);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  UpdateManagerWindow window;
  window.show();
  return app.exec();
}
