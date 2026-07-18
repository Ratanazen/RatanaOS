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

    // ── Sidebar ───────────────────────────────────────────────────
    auto *sidebar = new QFrame;
    sidebar->setObjectName("DesktopPanel");
    sidebar->setFixedWidth(210);
    auto *sbl = new QVBoxLayout(sidebar);
    sbl->setContentsMargins(14, 28, 14, 14);
    auto *logo = new QLabel("🔥 RatanaOS");
    logo->setObjectName("HeroTitle");
    sbl->addWidget(logo);
    sbl->addSpacing(16);

    QStringList stepNames = {"Welcome", "Privacy", "RatanaAI", "Theme", "Ready"};
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

    // ── Content area ──────────────────────────────────────────────
    auto *contentArea = new QWidget;
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(36, 28, 36, 20);

    // ── Page 0: Welcome ───────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Welcome to RatanaOS v5.0 Phoenix 🎉",
        "Thank you for choosing RatanaOS — a modern, privacy-first Linux distribution\n"
        "built for cybersecurity professionals, developers, and everyday users alike.\n\n"
        "This short guide will help you personalize your experience.\n"
        "It only takes about 2 minutes to complete."));
      l->addStretch();
      pages->addWidget(p);
    }

    // ── Page 1: Privacy ───────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Privacy Settings",
        "RatanaOS is private by default. All options below are off unless you enable them."));
      auto *panel = RatanaUI::makePanel("Data Collection");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      auto *telBox = new QCheckBox("Share anonymized crash reports (helps improve stability)");
      auto *usageBox = new QCheckBox("Share anonymous usage statistics");
      auto *locBox = new QCheckBox("Enable location services");
      pl->addWidget(telBox); pl->addWidget(usageBox); pl->addWidget(locBox);
      pl->addWidget(new QLabel("\n🔒 All telemetry is fully anonymized, stored locally first,\n"
                               "and never sold. You can change this any time in Settings → Privacy."));
      l->addWidget(panel); l->addStretch();
      pages->addWidget(p);
    }

    // ── Page 2: RatanaAI ──────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Ratana Assistant — Your AI Copilot",
        "Ratana Assistant is a built-in AI that runs entirely on your device.\n"
        "No data is sent to the cloud. It can help you:\n\n"
        "  • Explain shell commands and error messages\n"
        "  • Troubleshoot hardware and software issues\n"
        "  • Suggest packages and system configurations\n"
        "  • Automate routine tasks via natural language"));
      auto *panel = RatanaUI::makePanel("Setup");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      auto *grp = new QButtonGroup(p);
      auto *r1 = new QRadioButton("Enable Ratana Assistant (Recommended)");
      auto *r2 = new QRadioButton("Enable assistant, but only when I open it manually");
      auto *r3 = new QRadioButton("Disable assistant");
      r1->setChecked(true);
      grp->addButton(r1); grp->addButton(r2); grp->addButton(r3);
      pl->addWidget(r1); pl->addWidget(r2); pl->addWidget(r3);
      pl->addWidget(new QLabel("\n💡 Voice commands: available as an optional offline package in Software Center."));
      l->addWidget(panel); l->addStretch();
      pages->addWidget(p);
    }

    // ── Page 3: Theme ─────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Choose Your Theme",
        "Pick a color scheme. You can change this at any time in Settings → Appearance."));
      auto *panel = RatanaUI::makePanel("Color Scheme");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      auto *grp = new QButtonGroup(p);
      auto *dark  = new QRadioButton("🌙  Ratana Dark (Default)");
      auto *light = new QRadioButton("☀️   Ratana Light");
      auto *mid   = new QRadioButton("🌃  Ratana Midnight — Ultra-dark for low-light use");
      auto *day   = new QRadioButton("🌅  Ratana Daybreak — Warm light mode");
      dark->setChecked(true);
      grp->addButton(dark); grp->addButton(light);
      grp->addButton(mid);  grp->addButton(day);
      pl->addWidget(dark); pl->addWidget(light);
      pl->addWidget(mid);  pl->addWidget(day);
      l->addWidget(panel); l->addStretch();
      pages->addWidget(p);
    }

    // ── Page 4: Ready ─────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("You're All Set! 🚀",
        "RatanaOS v5.0 Phoenix is ready to use.\n\n"
        "Quick tips:\n"
        "  • Press the 🔥 launcher button or Super key to open apps\n"
        "  • Use Ratana Software Center to install new applications\n"
        "  • Run Ratana Update to keep your system secure\n"
        "  • Ask Ratana Assistant anything — just type or click the AI button\n\n"
        "Documentation: open the Ratana Welcome app from the launcher at any time."));
      l->addWidget(RatanaUI::makePillButton("Start Using RatanaOS", true));
      l->addWidget(RatanaUI::makePillButton("Open Software Center"));
      l->addStretch();
      pages->addWidget(p);
    }

    contentLayout->addWidget(pages);

    // ── Navigation ────────────────────────────────────────────────
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
