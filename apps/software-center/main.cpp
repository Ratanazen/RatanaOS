#include "../common/ratana_ui.h"

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QVBoxLayout>

class SoftwareCenterWindow : public QMainWindow {
 public:
  SoftwareCenterWindow() {
    setWindowTitle("RatanaOS Software Center");
    resize(1440, 900);

    auto *surface = new QWidget;
    setCentralWidget(surface);

    auto *sidebar = new QFrame;
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(18, 18, 18, 18);
    auto *title = new QLabel("Software Center");
    title->setObjectName("HeroTitle");
    sidebarLayout->addWidget(title);
    for (const QString &entry : {"Discover", "Work", "Create", "System", "Installed"}) {
      sidebarLayout->addWidget(RatanaUI::makePillButton(entry, entry == "Discover"));
    }
    sidebarLayout->addStretch();

    auto *content = new QWidget;
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(18);
    contentLayout->addWidget(RatanaUI::makeCard(
        "Featured collection",
        "Craft a fast, focused workstation",
        "Hand-picked apps for developers, creators, and power users. Each card exposes install state and update readiness.",
        QColor("#b96f31")));

    auto *grid = new QGridLayout;
    grid->setSpacing(18);

    struct AppCardData {
      QString eyebrow;
      QString title;
      QString body;
      QColor accent;
    };

    const QList<AppCardData> cards = {
        {"Developer tools", "Ratana Terminal", "GPU-aware shell with split views and profile presets.", QColor("#7c5c37")},
        {"Creativity", "Canvas Studio", "Layered graphics workspace with tablet-friendly controls.", QColor("#2e6f68")},
        {"Communication", "Orbit Mail", "Unified inbox with offline sync and focus mode.", QColor("#5f6da8")},
        {"System", "Driver Hub", "Firmware and kernel extension manager for supported devices.", QColor("#8d5f73")},
    };

    int row = 0;
    int column = 0;
    for (const auto &cardData : cards) {
      auto *card = RatanaUI::makeCard(cardData.eyebrow, cardData.title, cardData.body, cardData.accent);
      auto *layout = qobject_cast<QVBoxLayout *>(card->layout());
      layout->addWidget(RatanaUI::makePillButton("Install", true));
      layout->addWidget(RatanaUI::makePillButton("Preview"));
      grid->addWidget(card, row, column);
      ++column;
      if (column == 2) {
        column = 0;
        ++row;
      }
    }

    contentLayout->addLayout(grid);

    auto *aside = RatanaUI::makeSectionList("Deployment Channels", {
      "Stable: audited default feed",
      "Preview: early desktop integrations",
      "Nightly: internal QA and contributors"
    });

    RatanaUI::installWindowScaffold(surface, sidebar, content, aside);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  SoftwareCenterWindow window;
  window.show();
  return app.exec();
}
