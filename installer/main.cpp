#include "../apps/common/ratana_ui.h"

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
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QStringList>

QStringList scanAssets(const QString &folder) {
    QDir dir("/home/ratana/RatanaOS/assets/" + folder);
    if (!dir.exists()) {
        return QStringList{"(Default)"};
    }
    QStringList items = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    if (items.isEmpty()) return QStringList{"(Default)"};
    return items;
}

class InstallerWizard : public QMainWindow {
  QStackedWidget *stackedWidget;
  QProgressBar   *progressBar;
  QLabel         *installStatusLabel;
  QList<QPushButton*> sidebarBtns;
  int installStep = 0;

 public:
  InstallerWizard() {
    setWindowTitle("RatanaOS Installer — v16.0 Modular");
    resize(1150, 780);

    auto *surface = new QWidget;
    setCentralWidget(surface);
    auto *mainLayout = new QHBoxLayout(surface);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Sidebar
    auto *sidebar = new QFrame;
    sidebar->setObjectName("DesktopPanel");
    sidebar->setFixedWidth(240);
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(12, 24, 12, 12);

    auto *logo = new QLabel("🔥 RatanaOS");
    logo->setObjectName("HeroTitle");
    sidebarLayout->addWidget(logo);
    auto *version = new QLabel("v16.0 Modular UI");
    sidebarLayout->addWidget(version);
    sidebarLayout->addSpacing(16);

    QStringList steps = {
      "1. Welcome", "2. Language", "3. Keyboard",
      "4. Network", "5. Timezone", "6. Disk",
      "7. User", "8. Desktop", "9. Personalization",
      "10. Summary", "11. Install", "12. Finish"
    };

    for (const QString &step : steps) {
      auto *btn = RatanaUI::makePillButton(step, step.startsWith("1."));
      sidebarBtns.append(btn);
      sidebarLayout->addWidget(btn);
    }
    sidebarLayout->addStretch();
    mainLayout->addWidget(sidebar);

    // Content Area
    auto *contentArea = new QWidget;
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(32, 24, 32, 16);

    stackedWidget = new QStackedWidget;

    // ── Page 0: Welcome ─────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Welcome to RatanaOS v16",
        "RatanaOS v16 introduces a fully modular architecture.\n"
        "Customize your experience without modifying core code."));
      l->addWidget(RatanaUI::makePillButton("Install RatanaOS", true));
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 1: Language ─────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Language", "Choose your system language."));
      auto *combo = new QComboBox;
      combo->addItems({"English (US)", "Khmer", "French", "Japanese"});
      l->addWidget(combo); l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 2: Keyboard ─────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Keyboard Layout", "Select keyboard layout."));
      auto *combo = new QComboBox;
      combo->addItems({"US English", "UK English", "Khmer", "French"});
      l->addWidget(combo); l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 3: Network ──────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Network Setup", "Connect to WiFi or Ethernet."));
      l->addWidget(new QLabel("✅ Ethernet Connected"));
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 4: Timezone ─────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Timezone", "Select your region."));
      auto *combo = new QComboBox;
      combo->addItems({"UTC", "America/New_York", "Asia/Phnom_Penh"});
      l->addWidget(combo); l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 5: Disk ─────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Disk Setup", "Choose how to partition disk."));
      auto *autoBtn = new QRadioButton("Erase Disk (ext4)");
      autoBtn->setChecked(true);
      auto *btrfsBtn = new QRadioButton("Erase Disk (Btrfs with Snapshots)");
      l->addWidget(autoBtn); l->addWidget(btrfsBtn);
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 6: User ─────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Create Your Account", "Set username and computer name."));
      auto *grid = new QGridLayout;
      int row = 0;
      for (const QString &lbl : {"Full Name:", "Username:", "Password:", "Computer Name:"}) {
        grid->addWidget(new QLabel(lbl), row, 0);
        auto *e = new QLineEdit;
        if (lbl.contains("Password")) e->setEchoMode(QLineEdit::Password);
        grid->addWidget(e, row, 1);
        ++row;
      }
      l->addLayout(grid);
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 7: Desktop ──────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Select Desktop", "Choose your desktop environment."));
      for (const QString &de : {"KDE Plasma", "GNOME", "XFCE", "LXQt"}) {
        auto *r = new QRadioButton(de);
        if (de == "KDE Plasma") r->setChecked(true);
        l->addWidget(r);
      }
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 8: Personalization (Dynamic Assets) ─────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Personalization", "Customize your experience. Assets are loaded dynamically from /assets/."));

      auto *grid = new QGridLayout;

      grid->addWidget(new QLabel("Wallpaper:"), 0, 0);
      auto *wallCombo = new QComboBox;
      wallCombo->addItems(scanAssets("wallpapers"));
      grid->addWidget(wallCombo, 0, 1);

      grid->addWidget(new QLabel("Icon Pack:"), 1, 0);
      auto *iconCombo = new QComboBox;
      iconCombo->addItems(scanAssets("icons"));
      grid->addWidget(iconCombo, 1, 1);

      grid->addWidget(new QLabel("Profile Avatar:"), 2, 0);
      auto *avatarCombo = new QComboBox;
      avatarCombo->addItems(scanAssets("avatars"));
      grid->addWidget(avatarCombo, 2, 1);

      grid->addWidget(new QLabel("Cursor Theme:"), 3, 0);
      auto *cursorCombo = new QComboBox;
      cursorCombo->addItems(scanAssets("cursor"));
      grid->addWidget(cursorCombo, 3, 1);
      
      grid->addWidget(new QLabel("Accent Color:"), 4, 0);
      auto *colorCombo = new QComboBox;
      colorCombo->addItems({"Ratana Blue", "Emerald", "Ruby", "Amber"});
      grid->addWidget(colorCombo, 4, 1);

      l->addLayout(grid);
      l->addWidget(new QLabel("\n* Configuration will be saved to config/ratanaos-config.yaml"));
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 9: Summary ──────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Summary", "Review your settings before installing."));
      l->addWidget(new QLabel("- Language: English\n- Disk: Btrfs\n- Desktop: KDE Plasma\n- Assets: Customized"));
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 10: Install ─────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Installing RatanaOS v16", "Please wait..."));
      progressBar = new QProgressBar;
      progressBar->setRange(0, 100);
      progressBar->setValue(0);
      installStatusLabel = new QLabel("Preparing environment...");
      l->addWidget(progressBar);
      l->addWidget(installStatusLabel);
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    // ── Page 11: Finish ──────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Installation Complete!", "RatanaOS is ready."));
      l->addWidget(RatanaUI::makePillButton("Restart Now", true));
      l->addStretch();
      stackedWidget->addWidget(p);
    }

    contentLayout->addWidget(stackedWidget);

    // Navigation
    auto *navLayout = new QHBoxLayout;
    auto *btnBack = new QPushButton("← Back");
    auto *btnNext = new QPushButton("Next →");
    btnNext->setDefault(true);
    navLayout->addStretch();
    navLayout->addWidget(btnBack);
    navLayout->addWidget(btnNext);
    contentLayout->addLayout(navLayout);

    QObject::connect(btnNext, &QPushButton::clicked, [this, btnNext]() {
      int next = stackedWidget->currentIndex() + 1;
      if (next < stackedWidget->count()) {
        stackedWidget->setCurrentIndex(next);
        updateSidebar(next);
        if (next == 10) startInstallProgress();
        if (next == 11) btnNext->setEnabled(false);
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
      "Validating configuration...",
      "Copying dynamic assets (/assets/)...",
      "Installing themes and branding...",
      "Copying system files...",
      "Configuring GRUB and Plymouth...",
      "Writing BUILD_REPORT.md...",
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
