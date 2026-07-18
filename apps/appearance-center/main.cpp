#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include "customization_api.h"

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
    layout->addWidget(new QLabel("<b>Icons</b>"));
    QHBoxLayout *iconLayout = new QHBoxLayout();
    QPushButton *btnPreviewIcons = new QPushButton("Preview Icons");
    QPushButton *btnApplyIcons = new QPushButton("Apply Icons");
    QPushButton *btnInstallIcons = new QPushButton("Install Icon Packs");
    iconLayout->addWidget(btnPreviewIcons);
    iconLayout->addWidget(btnApplyIcons);
    iconLayout->addWidget(btnInstallIcons);
    layout->addLayout(iconLayout);

    // Wallpapers
    layout->addWidget(new QLabel("<b>Wallpapers (/usr/share/ratana/themes/)</b>"));
    QHBoxLayout *wpLayout = new QHBoxLayout();
    QPushButton *btnLocalWP = new QPushButton("Local Images");
    QPushButton *btnOnlineWP = new QPushButton("Online Source");
    QPushButton *btnSlideshow = new QPushButton("Slideshow");
    QPushButton *btnAutoWP = new QPushButton("Auto Change");
    wpLayout->addWidget(btnLocalWP);
    wpLayout->addWidget(btnOnlineWP);
    wpLayout->addWidget(btnSlideshow);
    wpLayout->addWidget(btnAutoWP);
    layout->addLayout(wpLayout);

    QObject::connect(comboTheme, &QComboBox::currentTextChanged, [&](const QString &theme) {
        RatanaOS::Customization::setTheme(theme);
    });

    QObject::connect(btnLocalWP, &QPushButton::clicked, [&]() {
        QMessageBox::information(&window, "Wallpapers", "Opening /usr/share/ratana/themes/");
    });

    layout->addStretch();
    window.show();
    return app.exec();
}
