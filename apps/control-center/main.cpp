#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QMainWindow window;
  window.setWindowTitle("RatanaOS Control Center");
  window.resize(1024, 768);

  auto *surface = new QWidget;
  window.setCentralWidget(surface);

  auto *layout = new QVBoxLayout(surface);
  auto *title = new QLabel("Control Center: Network, Display, Sound, Bluetooth");
  title->setAlignment(Qt::AlignCenter);
  layout->addWidget(title);

  window.show();
  return app.exec();
}
