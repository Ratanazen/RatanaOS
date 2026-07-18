#include "../common/ratana_ui.h"

#include <QApplication>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QProgressBar>
#include <QButtonGroup>
#include <QRadioButton>

class InstallerWizard : public QMainWindow {
  QStackedWidget *stackedWidget;
  QProgressBar   *progressBar;
  QList<QPushButton*> sidebarBtns;

 public:
  InstallerWizard() {
    setWindowTitle("RatanaOS Installer — v5.0 Phoenix");
    resize(1100, 750);

    auto *surface = new QWidget;
    setCentralWidget(surface);
    auto *mainLayout = new QHBoxLayout(surface);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // ── Sidebar ───────────────────────────────────────────────────
    auto *sidebar = new QFrame;
    sidebar->setObjectName("DesktopPanel");
    sidebar->setFixedWidth(240);
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(12, 24, 12, 12);

    auto *logo = new QLabel("🔥 RatanaOS");
    logo->setObjectName("HeroTitle");
    sidebarLayout->addWidget(logo);
    sidebarLayout->addSpacing(16);

    // Step list — 11 steps for v5.0
    QStringList steps = {
      "1. Welcome", "2. Language", "3. Keyboard", "4. Timezone",
      "5. Disk", "6. Users", "7. Edition", "8. Desktop",
      "9. Update Strategy", "10. Install", "11. Finish"
    };
    for (const QString &step : steps) {
      auto *btn = RatanaUI::makePillButton(step, step.startsWith("1."));
      sidebarBtns.append(btn);
      sidebarLayout->addWidget(btn);
    }
    sidebarLayout->addStretch();
    mainLayout->addWidget(sidebar);

    // ── Content Area ──────────────────────────────────────────────
    auto *contentArea = new QWidget;
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(32, 24, 32, 16);

    stackedWidget = new QStackedWidget;

    // ── Page 0: Welcome ───────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Welcome to RatanaOS v5.0 Phoenix",
        "This wizard will guide you through installing RatanaOS on your computer.\n"
        "Click Next to begin."));
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 1: Language ─────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Language", "Choose your system language."));
      auto *combo = new QComboBox;
      combo->addItems({"English (US)", "English (UK)", "French", "German", "Spanish", "Khmer", "Japanese", "Chinese (Simplified)"});
      l->addWidget(combo); l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 2: Keyboard ─────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Keyboard Layout", "Select your keyboard layout."));
      auto *combo = new QComboBox;
      combo->addItems({"US", "UK", "FR", "DE", "ES", "KH", "JP"});
      l->addWidget(combo); l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 3: Timezone ─────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Timezone", "Select your region and timezone."));
      auto *combo = new QComboBox;
      combo->addItems({"UTC", "America/New_York", "America/Los_Angeles", "Europe/London", "Europe/Paris", "Asia/Tokyo", "Asia/Phnom_Penh", "Asia/Bangkok"});
      l->addWidget(combo); l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 4: Disk ──────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Disk Configuration", "Choose how to partition your disk."));
      auto *autoBtn  = RatanaUI::makePillButton("Automatic Partitioning (Erase Disk)", true);
      auto *manBtn   = RatanaUI::makePillButton("Manual Partitioning");
      l->addWidget(autoBtn); l->addWidget(manBtn);
      l->addSpacing(12);
      auto *btrfs = new QCheckBox("Use Btrfs filesystem (enables atomic snapshots)");
      auto *luks  = new QCheckBox("Enable full-disk encryption (LUKS2)");
      l->addWidget(btrfs); l->addWidget(luks); l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 5: Users ─────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Create Your Account", "Set up your user and machine name."));
      for (const QString &lbl : {"Full Name:", "Username:", "Password:", "Confirm Password:", "Hostname:"}) {
        l->addWidget(new QLabel(lbl));
        auto *e = new QLineEdit;
        if (lbl == "Hostname:") e->setText("ratanaos");
        if (lbl.contains("Password")) e->setEchoMode(QLineEdit::Password);
        l->addWidget(e);
      }
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 6: Edition ───────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Select Edition", "Choose the edition that fits your use case."));
      auto *grp = new QButtonGroup(p);
      QStringList editions = {
        "Lite — XFCE, ≈2 GB, for older hardware",
        "Standard — KDE Plasma, ≈3 GB, everyday desktop",
        "Developer — KDE + full dev toolchain, ≈3.5 GB",
        "Cyber — KDE + optional security tools, ≈4 GB",
        "Server — Headless, Docker, ≈800 MB"
      };
      for (const QString &ed : editions) {
        auto *r = new QRadioButton(ed);
        if (ed.startsWith("Standard")) r->setChecked(true);
        grp->addButton(r);
        l->addWidget(r);
      }
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 7: Desktop ───────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Select Desktop Environment", "Choose your desktop."));
      auto *grp = new QButtonGroup(p);
      QStringList desktops = {
        "KDE Plasma — Feature-rich, modern (Recommended)",
        "XFCE — Lightweight, fast (Lite Edition default)",
        "GNOME — Clean, minimal workflow",
        "LXQt — Ultra-lightweight",
        "Hyprland — Wayland tiling (Experimental, Developer/Cyber only)"
      };
      for (const QString &d : desktops) {
        auto *r = new QRadioButton(d);
        if (d.startsWith("KDE")) r->setChecked(true);
        grp->addButton(r);
        l->addWidget(r);
      }
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 8: Update Strategy ────────────────────────────────── (NEW v5.0)
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Update Strategy",
        "Choose how your system will receive updates."));
      auto *grp = new QButtonGroup(p);
      auto *rApt = new QRadioButton("Traditional APT — Standard Debian package updates");
      auto *rAtomic = new QRadioButton("Atomic Updates — Btrfs snapshots with one-click rollback (Recommended)");
      rAtomic->setChecked(true);
      grp->addButton(rApt); grp->addButton(rAtomic);
      l->addWidget(rApt); l->addWidget(rAtomic);
      l->addSpacing(12);
      auto *autoUpdate = new QCheckBox("Enable automatic security updates");
      autoUpdate->setChecked(true);
      auto *telemetry = new QCheckBox("Share anonymized usage data (opt-in, helps improve RatanaOS)");
      l->addWidget(autoUpdate); l->addWidget(telemetry);
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 9: Install ───────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Installing RatanaOS",
        "Please wait while RatanaOS is installed to your disk."));
      progressBar = new QProgressBar;
      progressBar->setValue(0);
      progressBar->setTextVisible(true);
      auto *statusLabel = new QLabel("Preparing installation...");
      l->addWidget(progressBar);
      l->addWidget(statusLabel);
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 10: Finish ───────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Installation Complete!",
        "🎉 RatanaOS v5.0 Phoenix has been installed successfully.\n"
        "Remove the installation media and click Reboot Now."));
      l->addWidget(RatanaUI::makePillButton("Reboot Now", true));
      l->addWidget(RatanaUI::makePillButton("Continue Testing Live Session"));
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    contentLayout->addWidget(stackedWidget);

    // ── Navigation ────────────────────────────────────────────────
    auto *navLayout = new QHBoxLayout;
    auto *btnBack = new QPushButton("← Back");
    auto *btnNext = new QPushButton("Next →");
    btnNext->setDefault(true);
    navLayout->addStretch();
    navLayout->addWidget(btnBack);
    navLayout->addWidget(btnNext);
    contentLayout->addLayout(navLayout);

    // Navigation logic
    QObject::connect(btnNext, &QPushButton::clicked, [this]() {
      int next = stackedWidget->currentIndex() + 1;
      if (next < stackedWidget->count()) {
        stackedWidget->setCurrentIndex(next);
        if (next == 9) progressBar->setValue(50); // mock progress on install page
        updateSidebar(next);
      }
    });
    QObject::connect(btnBack, &QPushButton::clicked, [this]() {
      int prev = stackedWidget->currentIndex() - 1;
      if (prev >= 0) { stackedWidget->setCurrentIndex(prev); updateSidebar(prev); }
    });

    mainLayout->addWidget(contentArea, 1);
  }

 private:
  void updateSidebar(int index) {
    for (int i = 0; i < sidebarBtns.size(); ++i)
      sidebarBtns[i]->setProperty("active", i == index);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  InstallerWizard window;
  window.show();
  return app.exec();
}
