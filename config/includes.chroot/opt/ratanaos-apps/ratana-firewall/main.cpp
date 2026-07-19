#include "../common/ratana_ui.h"

#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTabWidget>

class FirewallApp : public QMainWindow {
 public:
  FirewallApp() {
    setWindowTitle("Ratana Firewall");
    resize(1100, 750);

    auto *surface = new QWidget;
    setCentralWidget(surface);
    auto *root = new QHBoxLayout(surface);
    root->setContentsMargins(0, 0, 0, 0);

    // Sidebar
    auto *sidebar = new QFrame;
    sidebar->setObjectName("DesktopPanel");
    sidebar->setFixedWidth(210);
    auto *sbl = new QVBoxLayout(sidebar);
    sbl->setContentsMargins(12, 20, 12, 12);
    auto *t = new QLabel("Ratana Firewall");
    t->setObjectName("HeroTitle");
    sbl->addWidget(t);
    sbl->addSpacing(8);
    sbl->addWidget(RatanaUI::makePillButton("Overview", true));
    sbl->addWidget(RatanaUI::makePillButton("Rules"));
    sbl->addWidget(RatanaUI::makePillButton("Ports"));
    sbl->addWidget(RatanaUI::makePillButton("IDS / fail2ban"));
    sbl->addWidget(RatanaUI::makePillButton("Logs"));
    sbl->addStretch();
    root->addWidget(sidebar);

    // Content
    auto *content = new QWidget;
    auto *cl = new QVBoxLayout(content);
    cl->setContentsMargins(32, 24, 32, 24);

    auto *status = RatanaUI::makePanel("Firewall Status",
      "UFW: ✅ Active (default: deny incoming, allow outgoing)\n"
      "AppArmor: ✅ Enforcing\n"
      "fail2ban: ✅ Running — 3 IPs currently banned");

    auto *rules = RatanaUI::makePanel("Active Rules");
    auto *rl = qobject_cast<QVBoxLayout*>(rules->layout());
    auto *ruleList = new QListWidget;
    ruleList->addItems({
      "22/tcp (SSH)    ALLOW IN    Anywhere",
      "80/tcp (HTTP)   ALLOW IN    Anywhere",
      "443/tcp (HTTPS) ALLOW IN    Anywhere",
      "8080/tcp        DENY IN     Anywhere",
      "Anywhere        ALLOW OUT   Anywhere"
    });
    rl->addWidget(ruleList);

    auto *addRuleLayout = new QHBoxLayout;
    auto *portEdit = new QLineEdit; portEdit->setPlaceholderText("Port (e.g. 3000)");
    auto *protoCombo = new QComboBox; protoCombo->addItems({"TCP", "UDP", "Any"});
    auto *actionCombo = new QComboBox; actionCombo->addItems({"ALLOW", "DENY", "LIMIT"});
    auto *dirCombo = new QComboBox; dirCombo->addItems({"IN", "OUT"});
    addRuleLayout->addWidget(portEdit); addRuleLayout->addWidget(protoCombo);
    addRuleLayout->addWidget(actionCombo); addRuleLayout->addWidget(dirCombo);
    addRuleLayout->addWidget(RatanaUI::makePillButton("Add Rule", true));
    rl->addLayout(addRuleLayout);

    cl->addWidget(status);
    cl->addWidget(rules);
    cl->addStretch();
    root->addWidget(content, 1);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  FirewallApp window;
  window.show();
  return app.exec();
}
