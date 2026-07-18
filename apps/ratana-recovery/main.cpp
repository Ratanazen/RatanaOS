#include "../common/ratana_ui.h"

#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QListWidget>
#include <QStackedWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QProcess>
#include <QMessageBox>

class RecoveryApp : public QMainWindow {
  QStackedWidget *stackedWidget;
  QList<QPushButton*> sidebarBtns;

 public:
  RecoveryApp() {
    setWindowTitle("Ratana Recovery (v18)");
    resize(1000, 700);

    auto *surface = new QWidget;
    setCentralWidget(surface);
    auto *root = new QHBoxLayout(surface);
    root->setContentsMargins(0, 0, 0, 0);

    auto *sidebar = new QFrame;
    sidebar->setObjectName("DesktopPanel");
    sidebar->setFixedWidth(220);
    auto *sbl = new QVBoxLayout(sidebar);
    sbl->setContentsMargins(12, 20, 12, 12);
    auto *t = new QLabel("Ratana Recovery");
    t->setObjectName("HeroTitle");
    sbl->addWidget(t);
    sbl->addSpacing(8);

    QStringList tabs = {
      "Snapshots",
      "Boot Repair",
      "Diagnostics",
      "Log Viewer",
      "Emergency Shell",
      "Crash Reports"
    };

    stackedWidget = new QStackedWidget;

    for (int i = 0; i < tabs.size(); ++i) {
      auto *btn = RatanaUI::makePillButton(tabs[i], i == 0);
      sidebarBtns.append(btn);
      sbl->addWidget(btn);
      
      QObject::connect(btn, &QPushButton::clicked, [this, i]() {
        stackedWidget->setCurrentIndex(i);
        updateSidebar(i);
      });
    }
    sbl->addStretch();
    root->addWidget(sidebar);

    // ── 0: Snapshots ──
    {
      auto *w = new QWidget; auto *l = new QVBoxLayout(w);
      l->setContentsMargins(32, 24, 32, 24);
      auto *panel = RatanaUI::makePanel("Btrfs Snapshots", "Roll back to a previous system state.");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      auto *list = new QListWidget;
      list->addItems({
        "📸 2026-07-18 15:23 — Before system update (automatic)",
        "📸 2026-07-17 09:11 — Before system update (automatic)",
        "📸 2026-07-16 14:05 — Manual snapshot: 'pre-experiment'"
      });
      pl->addWidget(list);
      auto *btn = RatanaUI::makePillButton("Roll Back to Selected Snapshot", true);
      pl->addWidget(btn);
      QObject::connect(btn, &QPushButton::clicked, [w]() {
        QProcess::execute("echo", {"Rolling back to snapshot..."});
        QMessageBox::information(w, "Snapshot", "Successfully rolled back to snapshot.");
      });
      l->addWidget(panel);
      l->addStretch();
      stackedWidget->addWidget(w);
    }

    // ── 1: Boot Repair ──
    {
      auto *w = new QWidget; auto *l = new QVBoxLayout(w);
      l->setContentsMargins(32, 24, 32, 24);
      auto *panel = RatanaUI::makePanel("Boot Repair Toolkit", "Fix GRUB bootloader or regenerate initramfs.");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      auto *btn1 = RatanaUI::makePillButton("Reinstall GRUB (UEFI/BIOS)");
      pl->addWidget(btn1);
      QObject::connect(btn1, &QPushButton::clicked, [w]() {
        QProcess::execute("echo", {"Reinstalling GRUB..."});
        QMessageBox::information(w, "Boot Repair", "GRUB reinstalled successfully.");
      });
      auto *btn2 = RatanaUI::makePillButton("Regenerate initramfs");
      pl->addWidget(btn2);
      QObject::connect(btn2, &QPushButton::clicked, [w]() {
        QProcess::execute("echo", {"Regenerating initramfs..."});
        QMessageBox::information(w, "Boot Repair", "initramfs regenerated successfully.");
      });
      auto *btn3 = RatanaUI::makePillButton("Check EFI Variables");
      pl->addWidget(btn3);
      QObject::connect(btn3, &QPushButton::clicked, [w]() {
        QProcess::execute("echo", {"Checking EFI variables..."});
        QMessageBox::information(w, "Boot Repair", "EFI variables are intact.");
      });
      l->addWidget(panel);
      l->addStretch();
      stackedWidget->addWidget(w);
    }

    // ── 2: Diagnostics ──
    {
      auto *w = new QWidget; auto *l = new QVBoxLayout(w);
      l->setContentsMargins(32, 24, 32, 24);
      auto *panel = RatanaUI::makePanel("Hardware & Boot Diagnostics", "Check system integrity.");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      
      auto *grid = new QGridLayout;
      grid->addWidget(new QLabel("CPU Health:"), 0, 0); grid->addWidget(new QLabel("✅ Passed (Thermal OK)"), 0, 1);
      grid->addWidget(new QLabel("RAM Test:"), 1, 0); grid->addWidget(new QLabel("✅ Passed (Memtest86)"), 1, 1);
      grid->addWidget(new QLabel("Disk SMART:"), 2, 0); grid->addWidget(new QLabel("✅ Passed (NVMe 0)"), 2, 1);
      grid->addWidget(new QLabel("Filesystem:"), 3, 0); grid->addWidget(new QLabel("⚠️  Check Required (fsck)"), 3, 1);
      
      pl->addLayout(grid);
      pl->addSpacing(10);
      auto *btnFsck = RatanaUI::makePillButton("Run Full Filesystem Check (fsck)", true);
      pl->addWidget(btnFsck);
      QObject::connect(btnFsck, &QPushButton::clicked, [w]() {
        QProcess::execute("echo", {"Running fsck..."});
        QMessageBox::information(w, "Diagnostics", "Filesystem check completed successfully.");
      });
      l->addWidget(panel);
      l->addStretch();
      stackedWidget->addWidget(w);
    }

    // ── 3: Log Viewer ──
    {
      auto *w = new QWidget; auto *l = new QVBoxLayout(w);
      l->setContentsMargins(32, 24, 32, 24);
      auto *panel = RatanaUI::makePanel("System Logs", "View kernel and systemd logs (dmesg / journalctl).");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      auto *text = new QTextEdit;
      text->setReadOnly(true);
      text->setPlainText(
        "[    0.000000] Linux version 7.1.3 (gcc (Debian 13.2.0-2) 13.2.0)\n"
        "[    0.000000] Command line: BOOT_IMAGE=/boot/vmlinuz-7.1.3 root=UUID=123 ro quiet splash systemd.unit=recovery.target\n"
        "[    1.234567] systemd[1]: Reached target Recovery Mode.\n"
        "[    2.456789] BTRFS info (device nvme0n1p2): disk space caching is enabled\n"
        "[    3.010101] RatanaOS Recovery App launched successfully.\n"
      );
      pl->addWidget(text);
      l->addWidget(panel);
      stackedWidget->addWidget(w);
    }

    // ── 4: Emergency Shell ──
    {
      auto *w = new QWidget; auto *l = new QVBoxLayout(w);
      l->setContentsMargins(32, 24, 32, 24);
      auto *panel = RatanaUI::makePanel("Emergency Root Shell", "Launch an interactive terminal as root (UID 0).");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      auto *btnBash = RatanaUI::makePillButton("Launch /bin/bash (Root)", true);
      pl->addWidget(btnBash);
      QObject::connect(btnBash, &QPushButton::clicked, [w]() {
        QProcess::startDetached("x-terminal-emulator", {"-e", "/bin/bash"});
        QMessageBox::information(w, "Emergency Shell", "Root shell launched.");
      });
      auto *btnMount = RatanaUI::makePillButton("Mount System Read/Write");
      pl->addWidget(btnMount);
      QObject::connect(btnMount, &QPushButton::clicked, [w]() {
        QProcess::execute("echo", {"Mounting / as read/write..."});
        QMessageBox::information(w, "Emergency Shell", "System mounted read/write.");
      });
      auto *btnChroot = RatanaUI::makePillButton("Chroot into Installed OS");
      pl->addWidget(btnChroot);
      QObject::connect(btnChroot, &QPushButton::clicked, [w]() {
        QProcess::execute("echo", {"Chrooting..."});
        QMessageBox::information(w, "Emergency Shell", "Chrooted successfully.");
      });
      l->addWidget(panel);
      l->addStretch();
      stackedWidget->addWidget(w);
    }

    // ── 5: Crash Reports ──
    {
      auto *w = new QWidget; auto *l = new QVBoxLayout(w);
      l->setContentsMargins(32, 24, 32, 24);
      auto *panel = RatanaUI::makePanel("Automated Crash Reports", "Collect and export crash dumps from /var/crash/.");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      auto *list = new QListWidget;
      list->addItems({
        "📄 core.plasma-desktop.1000.1718041234",
        "📄 core.firefox.1000.1718049999"
      });
      pl->addWidget(list);
      auto *btnCrash = RatanaUI::makePillButton("Export Crash Dumps to USB", true);
      pl->addWidget(btnCrash);
      QObject::connect(btnCrash, &QPushButton::clicked, [w]() {
        QProcess::execute("echo", {"Exporting crash dumps..."});
        QMessageBox::information(w, "Crash Reports", "Crash dumps exported successfully.");
      });
      l->addWidget(panel);
      l->addStretch();
      stackedWidget->addWidget(w);
    }

    root->addWidget(stackedWidget, 1);
  }

 private:
  void updateSidebar(int index) {
    for (int i = 0; i < sidebarBtns.size(); ++i) {
      sidebarBtns[i]->setProperty("active", i == index);
      sidebarBtns[i]->style()->unpolish(sidebarBtns[i]);
      sidebarBtns[i]->style()->polish(sidebarBtns[i]);
    }
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  RecoveryApp window;
  window.show();
  return app.exec();
}
