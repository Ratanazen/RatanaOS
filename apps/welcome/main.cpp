#include "../common/ratana_ui.h"

#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>

class WelcomeWindow : public QMainWindow {
 public:
  WelcomeWindow() {
    setWindowTitle("Welcome to RatanaOS");
    resize(1024, 768);

    auto *surface = new QWidget;
    setCentralWidget(surface);

    auto *sidebar = new QFrame;
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(18, 18, 18, 18);
    auto *title = new QLabel("Setup");
    title->setObjectName("HeroTitle");
    sidebarLayout->addWidget(title);
    
    for (const QString &entry : {"Welcome", "Privacy", "RatanaAI", "Theme", "Ready"}) {
      sidebarLayout->addWidget(RatanaUI::makePillButton(entry, entry == "Welcome"));
    }
    sidebarLayout->addStretch();

    auto *content = new QWidget;
    auto *contentLayout = new QVBoxLayout(content);
    
    auto *panel = RatanaUI::makePanel("Welcome to your new workstation", "RatanaOS is built for focus, performance, and stability.");
    auto *panelLayout = qobject_cast<QVBoxLayout *>(panel->layout());
    panelLayout->addWidget(RatanaUI::makeMetricRow("Debian Stable Base", "Reliable", "Security updates out of the box"));
    panelLayout->addWidget(RatanaUI::makeMetricRow("Wayland Compositor", "Fluid", "Tear-free rendering"));
    panelLayout->addWidget(RatanaUI::makeMetricRow("RatanaAI Integration", "Smart", "Opt-in on-device intelligence"));
    
    panelLayout->addWidget(RatanaUI::makePillButton("Start Setup", true));
    
    contentLayout->addWidget(panel);
    contentLayout->addStretch();

    RatanaUI::installWindowScaffold(surface, sidebar, content);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  WelcomeWindow window;
  window.show();
  return app.exec();
}
