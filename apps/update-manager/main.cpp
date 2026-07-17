#include "../common/ratana_ui.h"

#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QVBoxLayout>

class UpdateManagerWindow : public QMainWindow {
 public:
  UpdateManagerWindow() {
    setWindowTitle("RatanaOS Update Manager");
    resize(1320, 840);

    auto *surface = new QWidget;
    setCentralWidget(surface);

    auto *sidebar = new QFrame;
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(18, 18, 18, 18);
    auto *title = new QLabel("Update Manager");
    title->setObjectName("HeroTitle");
    sidebarLayout->addWidget(title);
    for (const QString &entry : {"Overview", "Pending", "History", "Policies"}) {
      sidebarLayout->addWidget(RatanaUI::makePillButton(entry, entry == "Pending"));
    }
    sidebarLayout->addStretch();

    auto *content = new QWidget;
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(18);

    auto *status = RatanaUI::makePanel("Pending Rollout", "RatanaOS coordinates OS, firmware, and application updates from one queue.");
    auto *statusLayout = qobject_cast<QVBoxLayout *>(status->layout());
    statusLayout->addWidget(RatanaUI::makeMetricRow("Packages", "12", "security + desktop"));
    statusLayout->addWidget(RatanaUI::makeMetricRow("Estimated downtime", "4 min", "single reboot"));
    statusLayout->addWidget(RatanaUI::makeMetricRow("Risk score", "Low", "staged on preview ring"));
    auto *progress = new QProgressBar;
    progress->setValue(72);
    statusLayout->addWidget(progress);
    statusLayout->addWidget(RatanaUI::makePillButton("Install Updates", true));
    contentLayout->addWidget(status);

    auto *timeline = RatanaUI::makePanel("Next Actions");
    auto *timelineLayout = qobject_cast<QVBoxLayout *>(timeline->layout());
    timelineLayout->addWidget(new QLabel("1. Snapshot user space and restore point"));
    timelineLayout->addWidget(new QLabel("2. Apply desktop shell and compositor fixes"));
    timelineLayout->addWidget(new QLabel("3. Refresh bundled applications"));
    timelineLayout->addWidget(new QLabel("4. Schedule reboot outside focus hours"));
    timelineLayout->addStretch();
    contentLayout->addWidget(timeline);

    auto *aside = RatanaUI::makeSectionList("Policy Highlights", {
      "Critical fixes bypass idle window after approval",
      "Firmware flashes require AC power",
      "Nightly channel disabled on battery-only devices"
    });

    RatanaUI::installWindowScaffold(surface, sidebar, content, aside);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  UpdateManagerWindow window;
  window.show();
  return app.exec();
}
