#include "../common/ratana_ui.h"

#include <QApplication>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QProgressBar>
#include <QButtonGroup>
#include <QRadioButton>
#include <QFileDialog>
#include <QTimer>

class InstallerWizard : public QMainWindow {
  QStackedWidget *stackedWidget;
  QProgressBar   *progressBar;
  QLabel         *installStatusLabel;
  QList<QPushButton*> sidebarBtns;
  int installStep = 0;

 public:
  InstallerWizard() {
    setWindowTitle("RatanaOS Installer — v13 Ultimate");
    resize(1200, 800);

    auto *surface = new QWidget;
    setCentralWidget(surface);
    auto *mainLayout = new QHBoxLayout(surface);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // ── Sidebar ───────────────────────────────────────────────────
    auto *sidebar = new QFrame;
    sidebar->setObjectName("DesktopPanel");
    sidebar->setFixedWidth(260);
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(12, 24, 12, 12);

    auto *logo = new QLabel("🔥 RatanaOS");
    logo->setObjectName("HeroTitle");
    sidebarLayout->addWidget(logo);
    auto *version = new QLabel("v13 Ultimate");
    sidebarLayout->addWidget(version);
    sidebarLayout->addSpacing(16);

    // Step list — 13 steps matching v13.0 spec
    QStringList steps = {
      "1. Welcome", "2. Language", "3. Keyboard",
      "4. Network", "5. Timezone", "6. Disk", 
      "7. User Account", "8. Avatar Upload", "9. Desktop",
      "10. Edition", "11. Optional Packages",
      "12. Installation", "13. Finish"
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
      l->addWidget(RatanaUI::makePanel("Welcome to RatanaOS v13 Ultimate",
        "RatanaOS is a modern, privacy-first Linux distribution.\n\n"
        "This wizard will guide you through installation.\n"
        "It only takes a few minutes to complete.\n\n"
        "Choose an option to begin:"));
      auto *btnTry = RatanaUI::makePillButton("Try Live");
      auto *btnInstall = RatanaUI::makePillButton("Install RatanaOS", true);
      l->addWidget(btnTry);
      l->addWidget(btnInstall);
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 1: Language ─────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Language", "Choose your system language."));
      auto *combo = new QComboBox;
      combo->addItems({
        "English (US)", "English (UK)",
        "ភាសាខ្មែរ (Khmer)",
        "Français", "Deutsch", "Español", "Italiano",
        "Português", "日本語", "中文(简体)", "한국어",
        "ภาษาไทย", "Tiếng Việt", "Bahasa Indonesia"
      });
      l->addWidget(combo); l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 2: Keyboard ─────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Keyboard Layout",
        "Select your keyboard layout. The system will auto-detect your keyboard."));
      auto *combo = new QComboBox;
      combo->addItems({
        "English (US)", "English (UK)", "Khmer",
        "French (AZERTY)", "German (QWERTZ)", "Spanish",
        "Japanese", "Chinese", "Korean", "Thai"
      });
      l->addWidget(combo);
      auto *testLine = new QLineEdit;
      testLine->setPlaceholderText("Type here to test your keyboard layout...");
      l->addWidget(new QLabel("Test keyboard input:"));
      l->addWidget(testLine);
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 3: Network ───────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Network Setup",
        "Connect to a network to download updates during installation."));

      auto *wifiPanel = RatanaUI::makePanel("WiFi Networks");
      auto *wpl = qobject_cast<QVBoxLayout*>(wifiPanel->layout());
      auto *grpWifi = new QButtonGroup(p);
      for (const QString &net : {"RatanaOS-Network", "HomeWifi-5G", "Office_WiFi", "GuestNetwork"}) {
        auto *r = new QRadioButton("📶  " + net);
        grpWifi->addButton(r);
        wpl->addWidget(r);
      }

      auto *ethPanel = RatanaUI::makePanel("Wired Ethernet");
      auto *epl = qobject_cast<QVBoxLayout*>(ethPanel->layout());
      epl->addWidget(new QLabel("✅  Ethernet: Connected (eth0 — 1Gbps)"));

      l->addWidget(wifiPanel);
      l->addWidget(ethPanel);
      l->addWidget(RatanaUI::makePillButton("Continue Without Network"));
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 4: Timezone ──────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Timezone Selection",
        "Select your regional timezone."));
      auto *combo = new QComboBox;
      combo->addItems({
        "UTC", "America/New_York", "America/Los_Angeles", 
        "Europe/London", "Europe/Paris", "Europe/Berlin",
        "Asia/Phnom_Penh", "Asia/Tokyo", "Asia/Shanghai", "Australia/Sydney"
      });
      l->addWidget(combo);
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 5: Disk Setup ────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Disk Configuration",
        "Choose how to partition your disk."));

      auto *grpMode = new QButtonGroup(p);
      auto *rAuto = new QRadioButton("Automatic — Erase disk and install RatanaOS");
      auto *rManual = new QRadioButton("Manual — Partition editor");
      rAuto->setChecked(true);
      grpMode->addButton(rAuto); grpMode->addButton(rManual);
      l->addWidget(rAuto); l->addWidget(rManual);
      l->addSpacing(12);

      auto *fsPanel = RatanaUI::makePanel("Filesystem Options");
      auto *fspl = qobject_cast<QVBoxLayout*>(fsPanel->layout());
      auto *grpFS = new QButtonGroup(p);
      auto *rExt4 = new QRadioButton("ext4 — Stable, widely supported (Default)");
      auto *rBtrfs = new QRadioButton("Btrfs — Atomic snapshots and rollback");
      rExt4->setChecked(true);
      grpFS->addButton(rExt4); grpFS->addButton(rBtrfs);
      fspl->addWidget(rExt4); fspl->addWidget(rBtrfs);
      auto *luks = new QCheckBox("Enable full-disk encryption (LUKS2)");
      auto *swap = new QCheckBox("Create swap partition");
      swap->setChecked(true);
      fspl->addWidget(luks); fspl->addWidget(swap);
      fspl->addWidget(new QLabel("Partition Table: GPT (UEFI) / MBR (BIOS Legacy)"));

      l->addWidget(fsPanel);
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 6: User Creation ─────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Create Your Account",
        "Set up your user account and computer name."));

      auto *grid = new QGridLayout;
      int row = 0;
      for (const QString &lbl : {"Full Name:", "Username:", "Password:", "Confirm Password:", "Computer Name:"}) {
        grid->addWidget(new QLabel(lbl), row, 0);
        auto *e = new QLineEdit;
        if (lbl == "Computer Name:") e->setText("ratanaos");
        if (lbl.contains("Password")) e->setEchoMode(QLineEdit::Password);
        grid->addWidget(e, row, 1);
        ++row;
      }
      l->addLayout(grid);

      auto *autoLogin = new QCheckBox("☑  Auto Login");
      autoLogin->setChecked(false);
      auto *adminAccount = new QCheckBox("☑  Administrator account");
      adminAccount->setChecked(true);
      l->addWidget(autoLogin);
      l->addWidget(adminAccount);
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 7: Avatar Upload ─────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Profile Avatar",
        "Upload a picture for your user profile (Optional)."));
      
      auto *avatarPreview = new QLabel("No Avatar Selected");
      avatarPreview->setFixedSize(128, 128);
      avatarPreview->setStyleSheet("background-color: #333; color: white; border-radius: 64px;");
      avatarPreview->setAlignment(Qt::AlignCenter);
      l->addWidget(avatarPreview, 0, Qt::AlignCenter);

      auto *avatarBtn = RatanaUI::makePillButton("Upload Profile Picture");
      l->addWidget(avatarBtn, 0, Qt::AlignCenter);
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 8: Desktop Selection ─────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Select Desktop Environment",
        "Choose your desktop. You can change this after installation."));
      auto *grp = new QButtonGroup(p);
      struct DE { QString label; QString desc; };
      QList<DE> desktops = {
        {"KDE Plasma",  "Feature-rich, modern, highly customizable (Recommended)"},
        {"GNOME",       "Clean, minimal, touch-friendly workflow"},
        {"XFCE",        "Lightweight and fast — ideal for older hardware"},
        {"LXQt",        "Ultra-lightweight Qt desktop"}
      };
      for (const auto &de : desktops) {
        auto *r = new QRadioButton(de.label + " — " + de.desc);
        if (de.label == "KDE Plasma") r->setChecked(true);
        grp->addButton(r);
        l->addWidget(r);
      }
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 9: Edition Selection ─────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Select Edition",
        "Choose the edition that best fits your use case."));
      auto *grp = new QButtonGroup(p);
      struct Ed { QString name; QString desc; QString size; };
      QList<Ed> editions = {
        {"Lite",      "XFCE desktop — for old or low-end hardware",         "≈2 GB"},
        {"Standard",  "KDE Plasma — everyday desktop use",                  "≈3 GB"},
        {"Developer", "KDE + full dev toolchain (GCC, Python, Rust, Go…)",  "≈3.5 GB"},
        {"Cyber",     "KDE + cybersecurity tools (nmap, nikto, Wireshark…)","≈4 GB"}
      };
      for (const auto &ed : editions) {
        auto *r = new QRadioButton(
          QString("RatanaOS %1  [%2]  — %3").arg(ed.name, ed.size, ed.desc));
        if (ed.name == "Standard") r->setChecked(true);
        grp->addButton(r);
        l->addWidget(r);
      }
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 10: Optional Package Groups ──────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Optional Package Groups",
        "Select additional software to install alongside RatanaOS."));
      
      auto *devPanel = RatanaUI::makePanel("Developer Profile");
      auto *devLayout = qobject_cast<QVBoxLayout*>(devPanel->layout());
      devLayout->addWidget(new QCheckBox("Install Dev Tools (GCC, Clang, CMake, Git)"));
      devLayout->addWidget(new QCheckBox("Install Programming Languages (Python, Rust, Go, Java)"));
      devLayout->addWidget(new QCheckBox("Install Containers (Docker / Podman)"));
      l->addWidget(devPanel);

      auto *cyberPanel = RatanaUI::makePanel("Cyber Profile");
      auto *cyberLayout = qobject_cast<QVBoxLayout*>(cyberPanel->layout());
      cyberLayout->addWidget(new QCheckBox("Network Analysis (Wireshark, Nmap, TCPDump)"));
      cyberLayout->addWidget(new QCheckBox("Web App Testing (BurpSuite, Nikto, Dirb)"));
      cyberLayout->addWidget(new QCheckBox("Digital Forensics (Autopsy, Volatility)"));
      cyberLayout->addWidget(new QCheckBox("Reverse Engineering (Ghidra, Radare2)"));
      l->addWidget(cyberPanel);

      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 11: Installation ──────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Installing RatanaOS v13 Ultimate",
        "Please wait while RatanaOS is installed to your disk."));
      progressBar = new QProgressBar;
      progressBar->setRange(0, 100);
      progressBar->setValue(0);
      progressBar->setTextVisible(true);
      installStatusLabel = new QLabel("Preparing installation environment...");
      l->addWidget(progressBar);
      l->addWidget(installStatusLabel);
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 12: Finish ───────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Installation Complete! 🎉",
        "RatanaOS v13 Ultimate has been installed successfully.\n\n"
        "Remove the installation media and restart your computer\n"
        "to boot into your new system."));
      l->addWidget(RatanaUI::makePillButton("Restart Now", true));
      l->addWidget(RatanaUI::makePillButton("Continue Live Session"));
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
    QObject::connect(btnNext, &QPushButton::clicked, [this, btnNext]() {
      int next = stackedWidget->currentIndex() + 1;
      if (next < stackedWidget->count()) {
        stackedWidget->setCurrentIndex(next);
        updateSidebar(next);
        // Kick off mock installation progress on page 11
        if (next == 11) startInstallProgress();
        if (next == 12) btnNext->setEnabled(false);
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

  void startInstallProgress() {
    installStep = 0;
    QStringList tasks = {
      "Copying system files…",
      "Installing base packages…",
      "Installing desktop environment…",
      "Configuring user account…",
      "Installing optional package groups…",
      "Installing bootloader (GRUB)…",
      "Applying theme and icons…",
      "Configuring system services…",
      "Installation complete!"
    };
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this, timer, tasks]() {
      if (installStep < tasks.size()) {
        int pct = (installStep * 100) / tasks.size();
        progressBar->setValue(pct);
        installStatusLabel->setText(tasks[installStep]);
        ++installStep;
      } else {
        progressBar->setValue(100);
        timer->stop();
      }
    });
    timer->start(800);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  InstallerWizard window;
  window.show();
  return app.exec();
}
