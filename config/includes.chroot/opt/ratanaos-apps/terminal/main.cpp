#include "../common/ratana_ui.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QTextEdit>
#include <QVBoxLayout>

class TerminalWindow : public QMainWindow {
 public:
  TerminalWindow() {
    setWindowTitle("Ratana Terminal");
    resize(1200, 800);

    auto *surface = new QWidget;
    setCentralWidget(surface);

    auto *mainLayout = new QHBoxLayout(surface);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar for profiles and AI Copilot
    auto *sidebar = new QFrame;
    sidebar->setObjectName("DesktopPanel");
    sidebar->setFixedWidth(250);
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->addWidget(new QLabel("<b>Profiles</b>"));
    sidebarLayout->addWidget(RatanaUI::makePillButton("Default (bash)", true));
    sidebarLayout->addWidget(RatanaUI::makePillButton("Remote SSH"));
    sidebarLayout->addStretch();
    sidebarLayout->addWidget(new QLabel("<b>RatanaAI Copilot</b>"));
    sidebarLayout->addWidget(new QLabel("Type natural language to\ngenerate shell commands."));
    mainLayout->addWidget(sidebar);

    // Terminal Emulator mockup
    auto *terminalArea = new QTextEdit;
    terminalArea->setStyleSheet("background-color: #1a1614; color: #e6e1da; font-family: 'JetBrains Mono', monospace; font-size: 14px;");
    terminalArea->setPlainText(
        "ratana@ratanaos:~$ uname -a\n"
        "Linux ratanaos 6.1.0-18-amd64 #1 SMP PREEMPT_DYNAMIC Debian 6.1.76-1 (2024-02-01) x86_64 GNU/Linux\n"
        "ratana@ratanaos:~$ \n"
    );
    mainLayout->addWidget(terminalArea, 1);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  TerminalWindow window;
  window.show();
  return app.exec();
}
