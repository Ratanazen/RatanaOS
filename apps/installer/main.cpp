#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QMainWindow window;
  window.setWindowTitle("RatanaOS Installer");
  window.resize(800, 600);

  auto *surface = new QWidget;
  window.setCentralWidget(surface);

  auto *layout = new QVBoxLayout(surface);
  auto *title = new QLabel("Welcome to RatanaOS");
  title->setAlignment(Qt::AlignCenter);
  layout->addWidget(title);

  auto *btnInstall = new QPushButton("Start Installation (Auto Partition)");
  layout->addWidget(btnInstall);

  window.show();
  return app.exec();
}
