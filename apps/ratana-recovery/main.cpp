#include "../common/ratana_ui.h"

#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QListWidget>

class RecoveryApp : public QMainWindow {
 public:
  RecoveryApp() {
    setWindowTitle("Ratana Recovery");
    resize(960, 640);

    auto *surface = new QWidget;
    setCentralWidget(surface);
    auto *root = new QHBoxLayout(surface);
    root->setContentsMargins(0, 0, 0, 0);

    auto *sidebar = new QFrame;
    sidebar->setObjectName("DesktopPanel");
    sidebar->setFixedWidth(200);
    auto *sbl = new QVBoxLayout(sidebar);
    sbl->setContentsMargins(12, 20, 12, 12);
    auto *t = new QLabel("Recovery");
    t->setObjectName("HeroTitle");
    sbl->addWidget(t);
    sbl->addSpacing(8);
    sbl->addWidget(RatanaUI::makePillButton("Snapshots", true));
    sbl->addWidget(RatanaUI::makePillButton("Restore System"));
    sbl->addWidget(RatanaUI::makePillButton("Repair Boot"));
    sbl->addWidget(RatanaUI::makePillButton("Disk Tools"));
    sbl->addStretch();
    root->addWidget(sidebar);

    auto *content = new QWidget;
    auto *cl = new QVBoxLayout(content);
    cl->setContentsMargins(32, 24, 32, 24);

    auto *snapPanel = RatanaUI::makePanel("Btrfs Snapshots",
      "Each system update creates a snapshot. Roll back to any point below.");
    auto *spl = qobject_cast<QVBoxLayout*>(snapPanel->layout());
    auto *snapList = new QListWidget;
    snapList->addItems({
      "📸 2026-07-18 15:23 — Before system update (automatic)",
      "📸 2026-07-17 09:11 — Before system update (automatic)",
      "📸 2026-07-16 14:05 — Manual snapshot: 'pre-experiment'",
      "📸 2026-07-15 08:00 — System install snapshot"
    });
    spl->addWidget(snapList);
    spl->addWidget(RatanaUI::makePillButton("Roll Back to Selected Snapshot", true));

    cl->addWidget(snapPanel);
    cl->addWidget(RatanaUI::makePanel("Quick Actions",
      "• Repair GRUB bootloader\n"
      "• Check filesystem integrity (fsck)\n"
      "• Reset user password\n"
      "• Launch root shell"));
    cl->addStretch();
    root->addWidget(content, 1);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  RecoveryApp window;
  window.show();
  return app.exec();
}
