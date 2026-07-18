#include "../../apps/common/ratana_ui.h"

#include <QApplication>
#include <QDate>
#include <QTime>
#include <QTimer>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>

class SessionManager : public QObject {
 public:
  SessionManager(QObject *parent = nullptr) : QObject(parent) {}
  void lockScreen() { qDebug("Locking screen..."); }
  void logout() { qDebug("Logging out..."); }
  void shutdown() { qDebug("Shutting down..."); }
};

class PanelWidget : public QFrame {
 public:
  PanelWidget(QWidget *parent = nullptr) : QFrame(parent) {
    setObjectName("DesktopPanel");
    setFixedHeight(40);
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 0, 10, 0);

    auto *launcherBtn = RatanaUI::makePillButton("Launcher", true);
    layout->addWidget(launcherBtn);
    
    layout->addStretch();
    
    auto *clockLabel = new QLabel(QTime::currentTime().toString("hh:mm"));
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [clockLabel]() {
      clockLabel->setText(QTime::currentTime().toString("hh:mm"));
    });
    timer->start(1000);
    layout->addWidget(clockLabel);
    
    layout->addStretch();

    auto *notifBtn = RatanaUI::makePillButton("Notifications");
    auto *sessionBtn = RatanaUI::makePillButton("Session");
    layout->addWidget(notifBtn);
    layout->addWidget(sessionBtn);
  }
};

class LauncherWidget : public QFrame {
 public:
  LauncherWidget(QWidget *parent = nullptr) : QFrame(parent) {
    auto *layout = new QGridLayout(this);
    layout->setSpacing(20);
    layout->addWidget(RatanaUI::makeCard("App", "Terminal", "CLI Environment", QColor("#333333")), 0, 0);
    layout->addWidget(RatanaUI::makeCard("App", "Settings", "System Config", QColor("#2b7a6e")), 0, 1);
    layout->addWidget(RatanaUI::makeCard("App", "Software", "App Center", QColor("#4f6fa3")), 1, 0);
    layout->addWidget(RatanaUI::makeCard("App", "Monitor", "Task Manager", QColor("#87516e")), 1, 1);
  }
};

class NotificationCenterWidget : public QFrame {
 public:
  NotificationCenterWidget(QWidget *parent = nullptr) : QFrame(parent) {
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("<b>Recent Notifications</b>"));
    layout->addWidget(RatanaUI::makeMetricRow("System Update", "Available", "Core components ready"));
    layout->addWidget(RatanaUI::makeMetricRow("Security", "Scan complete", "No issues found"));
    layout->addStretch();
  }
};

class DesktopEnvironment : public QMainWindow {
 public:
  DesktopEnvironment() {
    setWindowTitle("RatanaOS Desktop Environment");
    resize(1920, 1080);
    
    auto *central = new QWidget;
    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    auto *panel = new PanelWidget(this);
    mainLayout->addWidget(panel);
    
    auto *workspace = new QWidget;
    auto *wsLayout = new QHBoxLayout(workspace);
    
    auto *launcher = new LauncherWidget(this);
    auto *notifs = new NotificationCenterWidget(this);
    
    wsLayout->addWidget(launcher, 2);
    wsLayout->addWidget(notifs, 1);
    
    mainLayout->addWidget(workspace);
    setCentralWidget(central);
    
    // Session Manager integration
    SessionManager session;
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  DesktopEnvironment shell;
  shell.show();
  return app.exec();
}
