#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>
#include "customization_api.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Ratana Login Manager");
    window.resize(600, 500);

    QVBoxLayout *layout = new QVBoxLayout(&window);
    layout->addWidget(new QLabel("<h2>Login Screen Customization</h2>"));

    // Features: User avatar display, Custom background image, Blur effect, Dark/light mode, Clock, System information.
    QCheckBox *chkAvatar = new QCheckBox("Enable User Avatar Display");
    chkAvatar->setChecked(true);
    
    QCheckBox *chkBlur = new QCheckBox("Enable Blur Effect on Background");
    chkBlur->setChecked(true);

    QCheckBox *chkDarkLight = new QCheckBox("Follow System Dark/Light Mode");
    chkDarkLight->setChecked(true);

    QCheckBox *chkClock = new QCheckBox("Display Clock");
    chkClock->setChecked(true);

    QCheckBox *chkSysInfo = new QCheckBox("Display System Information");
    chkSysInfo->setChecked(true);

    layout->addWidget(chkAvatar);
    layout->addWidget(chkBlur);
    layout->addWidget(chkDarkLight);
    layout->addWidget(chkClock);
    layout->addWidget(chkSysInfo);

    QPushButton *btnBackground = new QPushButton("Set Custom Background Image");
    layout->addWidget(btnBackground);

    QPushButton *btnApply = new QPushButton("Apply to /etc/ratana/login/");
    layout->addWidget(btnApply);

    QObject::connect(btnBackground, &QPushButton::clicked, [&]() {
        QMessageBox::information(&window, "Background", "Select background image dialog...");
    });

    QObject::connect(btnApply, &QPushButton::clicked, [&]() {
        QMessageBox::information(&window, "Success", "Login configuration saved to /etc/ratana/login/");
    });

    layout->addStretch();
    window.show();
    return app.exec();
}
