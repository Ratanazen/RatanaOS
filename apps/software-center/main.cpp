#include "../common/ratana_ui.h"
#include "package_manager.h"

#include <QApplication>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class SoftwareCenterWindow : public QMainWindow {
  PackageManager *pkgManager;
  QGridLayout *resultsGrid;
  QTextEdit *consoleOutput;

 public:
  SoftwareCenterWindow() {
    setWindowTitle("RatanaOS Software Center");
    resize(1440, 900);

    pkgManager = new PackageManager(this);

    auto *surface = new QWidget;
    setCentralWidget(surface);

    // Sidebar
    auto *sidebar = new QFrame;
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(18, 18, 18, 18);
    auto *title = new QLabel("Software Center");
    title->setObjectName("HeroTitle");
    sidebarLayout->addWidget(title);
    for (const QString &entry : {"Discover", "Search", "Repositories", "Updates", "Installed"}) {
      sidebarLayout->addWidget(RatanaUI::makePillButton(entry, entry == "Discover"));
    }
    sidebarLayout->addStretch();

    // Content Area
    auto *content = new QWidget;
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(18);

    auto *searchBox = new QLineEdit;
    searchBox->setPlaceholderText("Search for applications... (e.g. browser)");
    contentLayout->addWidget(searchBox);

    resultsGrid = new QGridLayout;
    resultsGrid->setSpacing(18);
    contentLayout->addLayout(resultsGrid);
    
    contentLayout->addStretch();

    consoleOutput = new QTextEdit;
    consoleOutput->setReadOnly(true);
    consoleOutput->setFixedHeight(150);
    consoleOutput->setStyleSheet("background: #111; color: #fff; font-family: monospace;");
    contentLayout->addWidget(consoleOutput);

    // Signals
    connect(pkgManager, &PackageManager::operationOutput, [this](const QString &output) {
      consoleOutput->append(output);
    });

    connect(searchBox, &QLineEdit::returnPressed, [this, searchBox]() {
      clearGrid();
      QList<PackageManager::PackageInfo> results = pkgManager->searchPackages(searchBox->text());
      int row = 0, col = 0;
      for (const auto &pkg : results) {
        QString backendLabel = (pkg.backend == PackageManager::Backend::FLATPAK) ? " (Flatpak)" : " (APT)";
        auto *card = RatanaUI::makeCard("Package" + backendLabel, pkg.name, pkg.description, QColor("#4f6fa3"));
        auto *cardLayout = qobject_cast<QVBoxLayout *>(card->layout());
        
        auto *btn = RatanaUI::makePillButton(pkg.isInstalled ? "Remove" : "Install", !pkg.isInstalled);
        connect(btn, &QPushButton::clicked, [this, pkg, btn]() {
            if (pkg.isInstalled) {
                pkgManager->removePackage(pkg.name, pkg.backend);
                btn->setText("Install");
            } else {
                pkgManager->installPackage(pkg.name, pkg.backend);
                btn->setText("Remove");
            }
        });
        cardLayout->addWidget(btn);
        
        resultsGrid->addWidget(card, row, col);
        col++;
        if (col == 2) { col = 0; row++; }
      }
    });

    RatanaUI::installWindowScaffold(surface, sidebar, content, nullptr);
    
    // Initial populate
    searchBox->setText("browser");
    emit searchBox->returnPressed();
  }

 private:
  void clearGrid() {
    QLayoutItem *child;
    while ((child = resultsGrid->takeAt(0)) != nullptr) {
      if (child->widget()) {
        child->widget()->deleteLater();
      }
      delete child;
    }
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  SoftwareCenterWindow window;
  window.show();
  return app.exec();
}
