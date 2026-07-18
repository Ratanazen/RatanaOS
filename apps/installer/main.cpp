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

class InstallerWizard : public QMainWindow {
 public:
  InstallerWizard() {
    setWindowTitle("RatanaOS Installer");
    resize(1024, 768);

    auto *surface = new QWidget;
    setCentralWidget(surface);
    auto *mainLayout = new QHBoxLayout(surface);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Sidebar
    auto *sidebar = new QFrame;
    sidebar->setObjectName("DesktopPanel");
    sidebar->setFixedWidth(250);
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->addWidget(new QLabel("<b>Steps</b>"));
    
    QStringList steps = {"Welcome", "Language", "Keyboard", "Timezone", "Disk Selection", "User Creation", "Package Profile", "Installation", "Finish"};
    for (const QString &step : steps) {
      sidebarLayout->addWidget(RatanaUI::makePillButton(step, step == "Welcome"));
    }
    sidebarLayout->addStretch();
    mainLayout->addWidget(sidebar);

    // Content Area
    auto *contentArea = new QWidget;
    auto *contentLayout = new QVBoxLayout(contentArea);
    
    auto *stackedWidget = new QStackedWidget;
    
    // Page 0: Welcome
    auto *pageWelcome = new QWidget;
    auto *welcomeLayout = new QVBoxLayout(pageWelcome);
    welcomeLayout->addWidget(RatanaUI::makePanel("Welcome", "Start installing RatanaOS on your system."));
    stackedWidget->addWidget(pageWelcome);
    
    // Page 1: Language
    auto *pageLang = new QWidget;
    auto *langLayout = new QVBoxLayout(pageLang);
    langLayout->addWidget(new QLabel("Select Language:"));
    auto *langCombo = new QComboBox;
    langCombo->addItems({"English (US)", "English (UK)", "French", "German", "Spanish", "Khmer"});
    langLayout->addWidget(langCombo);
    langLayout->addStretch();
    stackedWidget->addWidget(pageLang);

    // Page 2: Keyboard
    auto *pageKey = new QWidget;
    auto *keyLayout = new QVBoxLayout(pageKey);
    keyLayout->addWidget(new QLabel("Select Keyboard Layout:"));
    auto *keyCombo = new QComboBox;
    keyCombo->addItems({"US", "UK", "FR", "DE", "ES", "KH"});
    keyLayout->addWidget(keyCombo);
    keyLayout->addStretch();
    stackedWidget->addWidget(pageKey);

    // Page 3: Timezone
    auto *pageTime = new QWidget;
    auto *timeLayout = new QVBoxLayout(pageTime);
    timeLayout->addWidget(new QLabel("Select Timezone:"));
    auto *timeCombo = new QComboBox;
    timeCombo->addItems({"UTC", "America/New_York", "Europe/London", "Asia/Phnom_Penh"});
    timeLayout->addWidget(timeCombo);
    timeLayout->addStretch();
    stackedWidget->addWidget(pageTime);

    // Page 4: Disk Selection
    auto *pageDisk = new QWidget;
    auto *diskLayout = new QVBoxLayout(pageDisk);
    diskLayout->addWidget(RatanaUI::makePanel("Disk Selection", "Choose where to install RatanaOS"));
    auto *autoPartBtn = RatanaUI::makePillButton("Auto Partition (Erase Disk)", true);
    auto *manualPartBtn = RatanaUI::makePillButton("Manual Partition");
    diskLayout->addWidget(autoPartBtn);
    diskLayout->addWidget(manualPartBtn);
    
    auto *btrfsCheck = new QCheckBox("Use Btrfs filesystem (Default is EXT4)");
    auto *luksCheck = new QCheckBox("Encrypt disk with LUKS");
    diskLayout->addWidget(btrfsCheck);
    diskLayout->addWidget(luksCheck);
    diskLayout->addStretch();
    stackedWidget->addWidget(pageDisk);

    // Page 5: User Creation
    auto *pageUser = new QWidget;
    auto *userLayout = new QVBoxLayout(pageUser);
    userLayout->addWidget(new QLabel("Name:"));
    userLayout->addWidget(new QLineEdit);
    userLayout->addWidget(new QLabel("Username:"));
    userLayout->addWidget(new QLineEdit);
    userLayout->addWidget(new QLabel("Password:"));
    userLayout->addWidget(new QLineEdit);
    userLayout->addWidget(new QLabel("Hostname:"));
    userLayout->addWidget(new QLineEdit("ratanaos"));
    userLayout->addStretch();
    stackedWidget->addWidget(pageUser);

    // Page 6: Package Profile (New for Cyber Edition)
    auto *pagePackages = new QWidget;
    auto *pkgLayout = new QVBoxLayout(pagePackages);
    pkgLayout->addWidget(RatanaUI::makePanel("Package Profile", "Select additional security and development toolkits to install."));
    
    QStringList categories = {
        "Network analysis", "Packet capture", "Web application testing", 
        "Digital forensics", "Reverse engineering", "Password auditing", 
        "Wireless analysis", "Malware analysis", "Incident response"
    };
    
    for (const QString &cat : categories) {
        pkgLayout->addWidget(new QCheckBox(cat));
    }
    pkgLayout->addStretch();
    stackedWidget->addWidget(pagePackages);

    // Page 7: Installation
    auto *pageInstall = new QWidget;
    auto *installLayout = new QVBoxLayout(pageInstall);
    installLayout->addWidget(RatanaUI::makePanel("Installing", "Please wait while RatanaOS is installed..."));
    auto *progressBar = new QProgressBar;
    progressBar->setValue(0);
    installLayout->addWidget(progressBar);
    installLayout->addStretch();
    stackedWidget->addWidget(pageInstall);

    // Page 8: Finish
    auto *pageFinish = new QWidget;
    auto *finishLayout = new QVBoxLayout(pageFinish);
    finishLayout->addWidget(RatanaUI::makePanel("Success!", "RatanaOS has been installed."));
    auto *rebootBtn = RatanaUI::makePillButton("Reboot Now", true);
    finishLayout->addWidget(rebootBtn);
    finishLayout->addStretch();
    stackedWidget->addWidget(pageFinish);

    contentLayout->addWidget(stackedWidget);

    // Navigation Controls
    auto *navLayout = new QHBoxLayout;
    auto *btnBack = new QPushButton("Back");
    auto *btnNext = new QPushButton("Next");
    navLayout->addStretch();
    navLayout->addWidget(btnBack);
    navLayout->addWidget(btnNext);
    contentLayout->addLayout(navLayout);
    
    // Quick wireup for Next button logic (mockup)
    QObject::connect(btnNext, &QPushButton::clicked, [stackedWidget, progressBar]() {
      int next = stackedWidget->currentIndex() + 1;
      if (next < stackedWidget->count()) {
        stackedWidget->setCurrentIndex(next);
        if (next == 7) { // Installation page is now index 7
           progressBar->setValue(50); // Mock progress
        }
      }
    });
    QObject::connect(btnBack, &QPushButton::clicked, [stackedWidget]() {
      int prev = stackedWidget->currentIndex() - 1;
      if (prev >= 0) stackedWidget->setCurrentIndex(prev);
    });

    mainLayout->addWidget(contentArea, 1);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  InstallerWizard window;
  window.show();
  return app.exec();
}
