#include "../common/ratana_ui.h"

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QTextEdit>
#include <QVBoxLayout>

class AIAssistantWindow : public QMainWindow {
 public:
  AIAssistantWindow() {
    setWindowTitle("RatanaOS AI Assistant");
    resize(1460, 920);

    auto *surface = new QWidget;
    setCentralWidget(surface);

    auto *sidebar = new QFrame;
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(18, 18, 18, 18);
    auto *title = new QLabel("AI Assistant");
    title->setObjectName("HeroTitle");
    sidebarLayout->addWidget(title);
    for (const QString &entry : {"Conversations", "System Tasks", "Automation", "Models"}) {
      sidebarLayout->addWidget(RatanaUI::makePillButton(entry, entry == "Conversations"));
    }
    sidebarLayout->addStretch();

    auto *content = new QWidget;
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(16);

    auto *conversation = RatanaUI::makePanel("Workspace Copilot", "The assistant UI is designed for system actions, summarization, and guided workflows.");
    auto *conversationLayout = qobject_cast<QVBoxLayout *>(conversation->layout());
    auto *chat = new QTextEdit;
    chat->setReadOnly(true);
    chat->setPlainText(
        "User: Prepare my workspace for presentation mode.\n\n"
        "Assistant: I can dim notifications, open the display profile, and preload presenter notes. Which one should run first?\n\n"
        "User: Dim notifications and show battery health.\n\n"
        "Assistant: Focus Shield is ready. Battery health is 94% with 6h 12m remaining.");
    conversationLayout->addWidget(chat);

    auto *composer = new QLineEdit;
    composer->setPlaceholderText("Ask RatanaOS to automate a task, summarize activity, or explain a system event.");
    conversationLayout->addWidget(composer);
    conversationLayout->addWidget(RatanaUI::makePillButton("Send Prompt", true));

    auto *tools = RatanaUI::makePanel("Action Palette");
    auto *toolsLayout = qobject_cast<QVBoxLayout *>(tools->layout());
    toolsLayout->addWidget(new QLabel("• Restart audio service"));
    toolsLayout->addWidget(new QLabel("• Summarize update history"));
    toolsLayout->addWidget(new QLabel("• Generate release notes"));
    toolsLayout->addWidget(new QLabel("• Explain high CPU usage"));
    toolsLayout->addStretch();

    contentLayout->addWidget(conversation, 2);
    contentLayout->addWidget(tools, 1);

    auto *aside = RatanaUI::makeSectionList("Guardrails", {
      "Require confirmation for privileged actions",
      "Store local session summaries only",
      "Expose model routing for transparency"
    });

    RatanaUI::installWindowScaffold(surface, sidebar, content, aside);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  AIAssistantWindow window;
  window.show();
  return app.exec();
}
