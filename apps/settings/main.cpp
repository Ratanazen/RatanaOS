#include "../common/ratana_ui.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QSlider>
#include <QVBoxLayout>

class SettingsWindow : public QMainWindow {
 public:
  SettingsWindow() {
    setWindowTitle("RatanaOS Settings");
    resize(1360, 880);

    auto *surface = new QWidget;
    setCentralWidget(surface);

    auto *sidebar = new QFrame;
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(18, 18, 18, 18);
    sidebarLayout->setSpacing(10);
    auto *name = new QLabel("Settings");
    name->setObjectName("HeroTitle");
    sidebarLayout->addWidget(name);
    for (const QString &entry : {"Profile", "Appearance", "Themes", "Icons", "Wallpaper", "Login Screen", "Privacy", "Network", "Hardware", "Updates"}) {
      sidebarLayout->addWidget(RatanaUI::makePillButton(entry, entry == "Profile"));
    }
    sidebarLayout->addStretch();

    auto *content = new QWidget;
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(18);

    auto *hero = RatanaUI::makePanel("Personalization", "Tune the desktop experience for focus, comfort, and motion.");
    contentLayout->addWidget(hero);

    auto *grid = new QGridLayout;
    grid->setSpacing(18);

    auto *appearance = RatanaUI::makePanel("Appearance");
    auto *appearanceLayout = qobject_cast<QVBoxLayout *>(appearance->layout());
    appearanceLayout->addWidget(new QLabel("Accent color"));
    auto *accent = new QComboBox;
    accent->addItems({"Terracotta", "Forest", "Ocean", "Graphite"});
    appearanceLayout->addWidget(accent);
    appearanceLayout->addWidget(new QLabel("Window density"));
    auto *density = new QSlider(Qt::Horizontal);
    density->setValue(65);
    appearanceLayout->addWidget(density);
    appearanceLayout->addStretch();

    auto *display = RatanaUI::makePanel("Display");
    auto *displayLayout = qobject_cast<QVBoxLayout *>(display->layout());
    displayLayout->addWidget(new QCheckBox("Night light on schedule"));
    displayLayout->addWidget(new QCheckBox("Adaptive refresh rate"));
    displayLayout->addWidget(new QCheckBox("Scale dock on hover"));
    displayLayout->addStretch();

    auto *privacy = RatanaUI::makePanel("Privacy");
    auto *privacyLayout = qobject_cast<QVBoxLayout *>(privacy->layout());
    privacyLayout->addWidget(new QCheckBox("Require app permission prompts"));
    privacyLayout->addWidget(new QCheckBox("Clear clipboard after 5 minutes"));
    privacyLayout->addWidget(new QCheckBox("Reduce telemetry to diagnostics only"));
    privacyLayout->addStretch();

    auto *wallpaper = RatanaUI::makePanel("Workspace Background");
    auto *wallpaperLayout = qobject_cast<QVBoxLayout *>(wallpaper->layout());
    wallpaperLayout->addWidget(new QLabel("Current scene: Sandstone Gradient"));
    wallpaperLayout->addWidget(RatanaUI::makePillButton("Change Wallpaper"));
    wallpaperLayout->addWidget(RatanaUI::makePillButton("Open Dynamic Themes", true));
    wallpaperLayout->addStretch();

    grid->addWidget(appearance, 0, 0);
    grid->addWidget(display, 0, 1);
    grid->addWidget(privacy, 1, 0);
    grid->addWidget(wallpaper, 1, 1);

    contentLayout->addLayout(grid);

    auto *aside = RatanaUI::makeSectionList("Suggested Tasks", {
      "Enable Wayland fractional scaling",
      "Review update cadence",
      "Connect Ratana Account"
    });

    RatanaUI::installWindowScaffold(surface, sidebar, content, aside);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  SettingsWindow window;
  window.show();
  return app.exec();
}
