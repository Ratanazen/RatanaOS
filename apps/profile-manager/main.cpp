#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include "customization_api.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Ratana Profile Manager");
    window.resize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(&window);
    
    QLabel *title = new QLabel("<h2>User Profile System</h2>");
    layout->addWidget(title);

    QLabel *avatarPreview = new QLabel("No Avatar");
    avatarPreview->setFixedSize(128, 128);
    avatarPreview->setStyleSheet("background-color: #333; color: white; border-radius: 64px;");
    avatarPreview->setAlignment(Qt::AlignCenter);
    
    QString currentAvatar = RatanaOS::Customization::getAvatarPath();
    if (QFile::exists(currentAvatar)) {
        avatarPreview->setText("Avatar Set");
    }

    layout->addWidget(avatarPreview, 0, Qt::AlignCenter);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    QPushButton *btnUpload = new QPushButton("Upload Picture");
    QPushButton *btnCrop = new QPushButton("Crop Image");
    QPushButton *btnGen = new QPushButton("Default Generator");
    QPushButton *btnRemove = new QPushButton("Remove Avatar");
    
    btnLayout->addWidget(btnUpload);
    btnLayout->addWidget(btnCrop);
    btnLayout->addWidget(btnGen);
    btnLayout->addWidget(btnRemove);
    
    layout->addLayout(btnLayout);
    
    QObject::connect(btnUpload, &QPushButton::clicked, [&]() {
        QString path = QFileDialog::getOpenFileName(&window, "Select Avatar", "", "Images (*.png *.jpg *.webp *.svg)");
        if (!path.isEmpty()) {
            if (RatanaOS::Customization::setAvatar(path)) {
                avatarPreview->setText("Avatar Updated");
                QMessageBox::information(&window, "Success", "Avatar saved to ~/.config/ratana/profile/");
            }
        }
    });

    QObject::connect(btnRemove, &QPushButton::clicked, [&]() {
        if (RatanaOS::Customization::removeAvatar()) {
            avatarPreview->setText("No Avatar");
        }
    });

    QObject::connect(btnCrop, &QPushButton::clicked, [&]() {
        QMessageBox::information(&window, "Crop", "Opening image cropper tool...");
    });

    QObject::connect(btnGen, &QPushButton::clicked, [&]() {
        QMessageBox::information(&window, "Generate", "Generating default avatar...");
    });

    layout->addStretch();
    window.show();
    return app.exec();
}
