#include "../common/ratana_ui.h"
#include "ai_engine.h"

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QListWidget>

class AIAssistantWindow : public QMainWindow {
  AIEngine *engine;
  QTextEdit *chatArea;
  QLineEdit *composer;
  AIEngine::Context currentContext = AIEngine::Context::EXPLAIN_COMMAND;

 public:
  AIAssistantWindow() {
    setWindowTitle("RatanaOS AI Assistant");
    resize(1460, 920);

    engine = new AIEngine(this);
    
    auto *surface = new QWidget;
    setCentralWidget(surface);

    auto *sidebar = new QFrame;
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(18, 18, 18, 18);
    auto *title = new QLabel("RatanaAI");
    title->setObjectName("HeroTitle");
    sidebarLayout->addWidget(title);
    
    sidebarLayout->addWidget(new QLabel("<b>Contexts</b>"));
    
    auto *btnExplain = RatanaUI::makePillButton("Explain Command", true);
    auto *btnTrouble = RatanaUI::makePillButton("Troubleshoot");
    auto *btnDocs = RatanaUI::makePillButton("Documentation");
    auto *btnPkgs = RatanaUI::makePillButton("Packages");
    
    // Quick UI mock for toggling states (in a real app, use QButtonGroup)
    connect(btnExplain, &QPushButton::clicked, [this]() { currentContext = AIEngine::Context::EXPLAIN_COMMAND; chatArea->append("<i>Context switched to: Explain Command</i>"); });
    connect(btnTrouble, &QPushButton::clicked, [this]() { currentContext = AIEngine::Context::TROUBLESHOOT; chatArea->append("<i>Context switched to: Troubleshoot</i>"); });
    connect(btnDocs, &QPushButton::clicked, [this]() { currentContext = AIEngine::Context::DOCUMENTATION; chatArea->append("<i>Context switched to: Documentation</i>"); });
    connect(btnPkgs, &QPushButton::clicked, [this]() { currentContext = AIEngine::Context::PACKAGES; chatArea->append("<i>Context switched to: Packages</i>"); });
    
    sidebarLayout->addWidget(btnExplain);
    sidebarLayout->addWidget(btnTrouble);
    sidebarLayout->addWidget(btnDocs);
    sidebarLayout->addWidget(btnPkgs);
    sidebarLayout->addStretch();

    auto *content = new QWidget;
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(16);

    auto *conversation = RatanaUI::makePanel("Workspace Copilot", "Ask RatanaAI for system assistance.");
    auto *conversationLayout = qobject_cast<QVBoxLayout *>(conversation->layout());
    
    chatArea = new QTextEdit;
    chatArea->setReadOnly(true);
    chatArea->setStyleSheet("font-size: 15px;");
    chatArea->setPlainText("Assistant: Hello! I am RatanaAI. How can I help you manage your system today?\n");
    conversationLayout->addWidget(chatArea);

    auto *inputLayout = new QHBoxLayout;
    composer = new QLineEdit;
    composer->setPlaceholderText("Ask a question...");
    auto *btnSend = RatanaUI::makePillButton("Send", true);
    inputLayout->addWidget(composer);
    inputLayout->addWidget(btnSend);
    conversationLayout->addLayout(inputLayout);

    contentLayout->addWidget(conversation);
    
    RatanaUI::installWindowScaffold(surface, sidebar, content, nullptr);
    
    // Connect Engine
    connect(btnSend, &QPushButton::clicked, this, &AIAssistantWindow::handleSend);
    connect(composer, &QLineEdit::returnPressed, this, &AIAssistantWindow::handleSend);
    
    connect(engine, &AIEngine::responseReceived, [this](const QString &response) {
       chatArea->append("<b>RatanaAI:</b> " + response + "\n");
    });
  }
  
 private:
  void handleSend() {
    QString query = composer->text();
    if (query.isEmpty()) return;
    
    chatArea->append("<b>User:</b> " + query);
    composer->clear();
    
    engine->submitQuery(currentContext, query);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  AIAssistantWindow window;
  window.show();
  return app.exec();
}
