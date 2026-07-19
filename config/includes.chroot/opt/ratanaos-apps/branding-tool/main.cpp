#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Ratana Branding Tool");
    window.resize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(&window);
    layout->addWidget(new QLabel("<h2>Custom Branding Manager</h2>"));
    layout->addWidget(new QLabel("Configure OS Branding for administrators."));

    QStringList features = {
        "Change OS Logo (branding/logo/)",
        "Change Boot Image",
        "Customize Plymouth Animation (branding/plymouth/)",
        "Set GRUB Background (branding/grub/)",
        "Set Default Wallpaper (branding/wallpaper/)",
        "Change Installer Logo"
    };

    for (const QString &f : features) {
        QPushButton *btn = new QPushButton(f);
        layout->addWidget(btn);
        QObject::connect(btn, &QPushButton::clicked, [f, &window]() {
            QMessageBox::information(&window, "Branding Tool", "Opening configuration for: " + f);
        });
    }

    layout->addStretch();
    window.show();
    return app.exec();
}
