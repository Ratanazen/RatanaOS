#include "../common/ratana_ui.h"

#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QRadioButton>
#include <QCheckBox>
#include <QButtonGroup>

class WelcomeApp : public QMainWindow {
  QStackedWidget *pages;
  QList<QPushButton*> stepBtns;

 public:
  WelcomeApp() {
    setWindowTitle("Welcome to RatanaOS");
    resize(960, 660);

    auto *surface = new QWidget;
    setCentralWidget(surface);
    auto *root = new QHBoxLayout(surface);
    root->setContentsMargins(0, 0, 0, 0);

    // Sidebar
    auto *sidebar = new QFrame;
    sidebar->setObjectName("DesktopPanel");
    sidebar->setFixedWidth(210);
    auto *sbl = new QVBoxLayout(sidebar);
    sbl->setContentsMargins(14, 28, 14, 14);
    auto *logo = new QLabel("🔥 RatanaOS");
    logo->setObjectName("HeroTitle");
    sbl->addWidget(logo);
    sbl->addSpacing(16);

    QStringList stepNames = {"Welcome", "Profile Setup", "Privacy", "RatanaAI", "Theme", "Wallpaper", "Cyber & Dev", "Ready"};
    pages = new QStackedWidget;
    int i = 0;
    for (const QString &name : stepNames) {
      auto *btn = RatanaUI::makePillButton(name, i == 0);
      stepBtns.append(btn);
      sbl->addWidget(btn);
      ++i;
    }
    sbl->addStretch();
    root->addWidget(sidebar);

    // Content area
    auto *contentArea = new QWidget;
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(36, 28, 36, 20);

    // Page 0: Welcome
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Welcome to RatanaOS v11.1 🎉",
        "Thank you for choosing RatanaOS — a modern, privacy-first Linux distribution.\n"
        "This short guide will help you personalize your experience."));
      l->addStretch();
      pages->addWidget(p);
    }
    
    // Page 1: Profile Setup
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Profile Setup",
        "Upload an avatar and set up your personal Ratana Profile."));
      l->addWidget(RatanaUI::makePillButton("Open Profile Manager"));
      l->addStretch();
      pages->addWidget(p);
    }

    // Page 2: Privacy
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Privacy Settings",
        "RatanaOS is private by default."));
      auto *panel = RatanaUI::makePanel("Data Collection");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      pl->addWidget(new QCheckBox("Share anonymized crash reports"));
      pl->addWidget(new QCheckBox("Share anonymous usage statistics"));
      pl->addWidget(new QCheckBox("Enable location services"));
      l->addWidget(panel); l->addStretch();
      pages->addWidget(p);
    }

    // Page 3: RatanaAI
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Ratana Assistant — Your AI Copilot",
        "Ratana Assistant runs offline entirely on your device."));
      auto *panel = RatanaUI::makePanel("Setup");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      auto *grp = new QButtonGroup(p);
      auto *r1 = new QRadioButton("Enable Ratana Assistant (Recommended)");
      auto *r2 = new QRadioButton("Disable assistant");
      r1->setChecked(true);
      grp->addButton(r1); grp->addButton(r2);
      pl->addWidget(r1); pl->addWidget(r2);
      l->addWidget(panel); l->addStretch();
      pages->addWidget(p);
    }

    // Page 4: Theme
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Choose Your Theme",
        "Pick a color scheme. You can change this later in Appearance Center."));
      auto *panel = RatanaUI::makePanel("Color Scheme");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      auto *grp = new QButtonGroup(p);
      auto *dark  = new QRadioButton("🌙  Dark (Default)");
      auto *light = new QRadioButton("☀️   Light");
      auto *amoled = new QRadioButton("🌃  AMOLED");
      dark->setChecked(true);
      grp->addButton(dark); grp->addButton(light); grp->addButton(amoled);
      pl->addWidget(dark); pl->addWidget(light); pl->addWidget(amoled);
      l->addWidget(panel); l->addStretch();
      pages->addWidget(p);
    }
    
    // Page 5: Wallpaper
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Select Wallpaper",
        "Choose a wallpaper to personalize your desktop."));
      l->addWidget(RatanaUI::makePillButton("Open Appearance Center"));
      l->addStretch();
      pages->addWidget(p);
    }
    
    // Page 6: Cyber & Dev
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Install Optional Packages",
        "Choose your work environment."));
      auto *panel = RatanaUI::makePanel("Profiles");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      pl->addWidget(new QCheckBox("Enable Developer Mode (Install toolchains)"));
      pl->addWidget(new QCheckBox("Enable Cyber Profile (Install security toolkits)"));
      l->addWidget(panel); l->addStretch();
      pages->addWidget(p);
    }

    // Page 7: Ready
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("You're All Set! 🚀",
        "RatanaOS v11.1 is ready to use."));
      l->addWidget(RatanaUI::makePillButton("Start Using RatanaOS", true));
      l->addStretch();
      pages->addWidget(p);
    }

    contentLayout->addWidget(pages);

    // Navigation
    auto *navLayout = new QHBoxLayout;
    auto *btnBack = new QPushButton("← Back");
    auto *btnNext = new QPushButton("Next →");
    btnNext->setDefault(true);
    navLayout->addStretch();
    navLayout->addWidget(btnBack);
    navLayout->addWidget(btnNext);
    contentLayout->addLayout(navLayout);

    QObject::connect(btnNext, &QPushButton::clicked, [this]() {
      int n = pages->currentIndex() + 1;
      if (n < pages->count()) { pages->setCurrentIndex(n); updateSidebar(n); }
    });
    QObject::connect(btnBack, &QPushButton::clicked, [this]() {
      int n = pages->currentIndex() - 1;
      if (n >= 0) { pages->setCurrentIndex(n); updateSidebar(n); }
    });

    root->addWidget(contentArea, 1);
  }

 private:
  void updateSidebar(int idx) {
    for (int i = 0; i < stepBtns.size(); ++i)
      stepBtns[i]->setProperty("active", i == idx);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  WelcomeApp window;
  window.show();
  return app.exec();
}
