#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QDir>
#include <QStringList>
#include "customization_api.h"

QStringList scanAssets(const QString &folder) {
    QDir dir("/home/ratana/RatanaOS/assets/" + folder);
    if (!dir.exists()) {
        return QStringList{"(Default)"};
    }
    QStringList items = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    if (items.isEmpty()) return QStringList{"(Default)"};
    return items;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Ratana Appearance Center");
    window.resize(700, 500);

    QVBoxLayout *layout = new QVBoxLayout(&window);
    layout->addWidget(new QLabel("<h2>Desktop Personalization</h2>"));

    // Themes
    layout->addWidget(new QLabel("<b>Themes</b>"));
    QComboBox *comboTheme = new QComboBox();
    comboTheme->addItems({"Dark", "Light", "AMOLED", "Custom Theme"});
    layout->addWidget(comboTheme);

    // Icons
    layout->addWidget(new QLabel("<b>Icons (Loaded from /assets/icons/)</b>"));
    QComboBox *comboIcons = new QComboBox();
    comboIcons->addItems(scanAssets("icons"));
    layout->addWidget(comboIcons);

    // Wallpapers
    layout->addWidget(new QLabel("<b>Wallpapers (Loaded from /assets/wallpapers/)</b>"));
    QComboBox *comboWP = new QComboBox();
    comboWP->addItems(scanAssets("wallpapers"));
    layout->addWidget(comboWP);

    // Application
    QHBoxLayout *actionLayout = new QHBoxLayout();
    QPushButton *btnApply = new QPushButton("Apply Selected Settings");
    actionLayout->addStretch();
    actionLayout->addWidget(btnApply);
    layout->addLayout(actionLayout);

    QObject::connect(btnApply, &QPushButton::clicked, [&]() {
        RatanaOS::Customization::setTheme(comboTheme->currentText());
        RatanaOS::Customization::setWallpaper(comboWP->currentText());
        QMessageBox::information(&window, "Settings Applied", "Your personalization settings have been updated.\n(Configuration saved).");
    });

    layout->addStretch();
    window.show();
    return app.exec();
}
