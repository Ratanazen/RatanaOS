#include "../common/ratana_ui.h"

#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QRadioButton>
#include <QCheckBox>
#include <QButtonGroup>
#include <QSlider>
#include <QComboBox>

class WelcomeApp : public QMainWindow {
  QStackedWidget *pages;
  QList<QPushButton*> stepBtns;

 public:
  WelcomeApp() {
    setWindowTitle("RatanaOS Welcome (v19)");
    resize(1000, 700);

    auto *surface = new QWidget;
    setCentralWidget(surface);
    auto *root = new QHBoxLayout(surface);
    root->setContentsMargins(0, 0, 0, 0);

    // Sidebar
    auto *sidebar = new QFrame;
    sidebar->setObjectName("DesktopPanel");
    sidebar->setFixedWidth(230);
    auto *sbl = new QVBoxLayout(sidebar);
    sbl->setContentsMargins(14, 28, 14, 14);
    auto *logo = new QLabel("🚀 RatanaOS");
    logo->setObjectName("HeroTitle");
    sbl->addWidget(logo);
    sbl->addSpacing(16);

    QStringList stepNames = {
      "Welcome",
      "Hardware Scan",
      "Drivers",
      "Peripherals",
      "Display & Input",
      "Accessibility",
      "Performance",
      "Ready"
    };

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

    // 0: Welcome
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Welcome to RatanaOS v19",
        "Let's get your hardware configured and your system personalized. "
        "This wizard will automatically detect your devices and offer the best setup options."));
      l->addStretch();
      pages->addWidget(p);
    }
    
    // 1: Hardware Scan
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      auto *panel = RatanaUI::makePanel("Hardware Detection", "Automatically profiling your system capabilities.");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      
      auto *grid = new QGridLayout;
      grid->addWidget(new QLabel("CPU:"), 0, 0); grid->addWidget(new QLabel("AMD Ryzen 7 5800X (8 Cores)"), 0, 1);
      grid->addWidget(new QLabel("GPU:"), 1, 0); grid->addWidget(new QLabel("NVIDIA GeForce RTX 3070"), 1, 1);
      grid->addWidget(new QLabel("RAM:"), 2, 0); grid->addWidget(new QLabel("32 GB DDR4"), 2, 1);
      grid->addWidget(new QLabel("Net:"), 3, 0); grid->addWidget(new QLabel("Intel Wi-Fi 6 AX200"), 3, 1);
      pl->addLayout(grid);
      l->addWidget(panel);
      l->addStretch();
      pages->addWidget(p);
    }

    // 2: Drivers
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      auto *panel = RatanaUI::makePanel("Driver Manager", "Select drivers for your detected hardware.");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      
      pl->addWidget(new QLabel("<b>Graphics (NVIDIA GeForce RTX 3070)</b>"));
      auto *gpuGrp = new QButtonGroup(p);
      auto *gpu1 = new QRadioButton("NVIDIA Proprietary Driver 550 (Recommended for Gaming)");
      auto *gpu2 = new QRadioButton("Nouveau Open-Source Driver");
      gpu1->setChecked(true);
      gpuGrp->addButton(gpu1); gpuGrp->addButton(gpu2);
      pl->addWidget(gpu1); pl->addWidget(gpu2);
      
      pl->addSpacing(15);
      pl->addWidget(new QLabel("<b>Networking (Intel Wi-Fi 6)</b>"));
      pl->addWidget(new QCheckBox("Install proprietary Wi-Fi firmware (Recommended)", p));
      
      l->addWidget(panel); l->addStretch();
      pages->addWidget(p);
    }

    // 3: Peripherals
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      auto *panel = RatanaUI::makePanel("Printers & Bluetooth", "Connect to your external devices.");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      
      pl->addWidget(RatanaUI::makePillButton("Scan for Bluetooth Devices"));
      pl->addWidget(new QLabel("<i>Found: Apple Magic Mouse, Keychron K8</i>"));
      pl->addSpacing(10);
      pl->addWidget(RatanaUI::makePillButton("Add Network Printer"));
      pl->addWidget(new QLabel("<i>Found: HP Color LaserJet Pro on 192.168.1.50</i>"));

      l->addWidget(panel); l->addStretch();
      pages->addWidget(p);
    }

    // 4: Display & Input
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      auto *panel = RatanaUI::makePanel("Display & Keyboard Configuration", "Optimize your viewing and typing experience.");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      
      pl->addWidget(new QLabel("<b>UI Scaling</b>"));
      auto *scaleSlider = new QSlider(Qt::Horizontal);
      scaleSlider->setRange(100, 200);
      scaleSlider->setTickInterval(25);
      scaleSlider->setTickPosition(QSlider::TicksBelow);
      pl->addWidget(scaleSlider);
      
      pl->addSpacing(15);
      pl->addWidget(new QLabel("<b>Keyboard Layout</b>"));
      auto *kbCombo = new QComboBox;
      kbCombo->addItems({"English (US)", "English (UK)", "Khmer", "Spanish", "French"});
      pl->addWidget(kbCombo);

      l->addWidget(panel); l->addStretch();
      pages->addWidget(p);
    }
    
    // 5: Accessibility
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      auto *panel = RatanaUI::makePanel("Accessibility", "Tailor the OS to your needs.");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      
      pl->addWidget(new QCheckBox("High Contrast Theme"));
      pl->addWidget(new QCheckBox("Enable Screen Reader (Orca)"));
      pl->addWidget(new QCheckBox("Large Text Mode"));
      pl->addWidget(new QCheckBox("Visual Alerts for Sounds"));

      l->addWidget(panel); l->addStretch();
      pages->addWidget(p);
    }

    // 6: Performance
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      auto *panel = RatanaUI::makePanel("Performance Profile", "Balance power draw with performance.");
      auto *pl = qobject_cast<QVBoxLayout*>(panel->layout());
      
      auto *perfGrp = new QButtonGroup(p);
      auto *perf1 = new QRadioButton("Power Saver (Maximum battery life)");
      auto *perf2 = new QRadioButton("Balanced (Recommended)");
      auto *perf3 = new QRadioButton("Performance (Maximum speed, higher thermal output)");
      perf2->setChecked(true);
      perfGrp->addButton(perf1); perfGrp->addButton(perf2); perfGrp->addButton(perf3);
      
      pl->addWidget(perf1); pl->addWidget(perf2); pl->addWidget(perf3);

      l->addWidget(panel); l->addStretch();
      pages->addWidget(p);
    }

    // 7: Ready
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p);
      l->addWidget(RatanaUI::makePanel("Setup Complete! ✅",
        "Your hardware is configured and your profile is saved.\n"
        "Welcome to your new RatanaOS desktop."));
      l->addWidget(RatanaUI::makePillButton("Finish and Start Exploring", true));
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
    for (int i = 0; i < stepBtns.size(); ++i) {
      stepBtns[i]->setProperty("active", i == idx);
      stepBtns[i]->style()->unpolish(stepBtns[i]);
      stepBtns[i]->style()->polish(stepBtns[i]);
    }
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  WelcomeApp window;
  window.show();
  return app.exec();
}
