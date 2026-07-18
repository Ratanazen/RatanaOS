#include "../common/ratana_ui.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>

class FileManagerWindow : public QMainWindow {
 public:
  FileManagerWindow() {
    setWindowTitle("Ratana File Manager");
    resize(1280, 800);

    auto *surface = new QWidget;
    setCentralWidget(surface);
    
    // Modern "Places" sidebar
    auto *sidebar = new QFrame;
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(18, 18, 18, 18);
    auto *title = new QLabel("Files");
    title->setObjectName("HeroTitle");
    sidebarLayout->addWidget(title);
    
    for (const QString &entry : {"Home", "Desktop", "Documents", "Downloads", "Pictures", "Trash"}) {
      sidebarLayout->addWidget(RatanaUI::makePillButton(entry, entry == "Home"));
    }
    sidebarLayout->addStretch();
    
    auto *content = new QWidget;
    auto *contentLayout = new QVBoxLayout(content);
    
    // Breadcrumbs
    auto *header = RatanaUI::makePanel("Home / Documents / Projects", "12 items, 4.2 GB available");
    contentLayout->addWidget(header);
    
    auto *grid = new QGridLayout;
    grid->setSpacing(18);
    
    grid->addWidget(RatanaUI::makeCard("Folder", "RatanaOS_Source", "Modified Today", QColor("#4f6fa3")), 0, 0);
    grid->addWidget(RatanaUI::makeCard("Archive", "backup_2026.tar.gz", "1.2 GB", QColor("#87516e")), 0, 1);
    grid->addWidget(RatanaUI::makeCard("Document", "release_notes.pdf", "42 KB", QColor("#2b7a6e")), 0, 2);
    
    contentLayout->addLayout(grid);
    contentLayout->addStretch();

    RatanaUI::installWindowScaffold(surface, sidebar, content);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  FileManagerWindow window;
  window.show();
  return app.exec();
}
