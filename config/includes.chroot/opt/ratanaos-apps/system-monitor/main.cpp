#include "../common/ratana_ui.h"

#include <QApplication>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

class SystemMonitorWindow : public QMainWindow {
 public:
  SystemMonitorWindow() {
    setWindowTitle("RatanaOS System Monitor");
    resize(1380, 860);

    auto *surface = new QWidget;
    setCentralWidget(surface);

    auto *sidebar = new QFrame;
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(18, 18, 18, 18);
    auto *title = new QLabel("System Monitor");
    title->setObjectName("HeroTitle");
    sidebarLayout->addWidget(title);
    for (const QString &entry : {"Overview", "Processes", "Performance", "Storage", "Services"}) {
      sidebarLayout->addWidget(RatanaUI::makePillButton(entry, entry == "Overview"));
    }
    sidebarLayout->addStretch();

    auto *content = new QWidget;
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(18);

    auto *summary = RatanaUI::makePanel("Resource Summary", "Live metrics can later be backed by `/proc`, `systemd`, and container runtimes.");
    auto *summaryLayout = qobject_cast<QVBoxLayout *>(summary->layout());
    summaryLayout->addWidget(RatanaUI::makeMetricRow("CPU", "28%", "8 cores active"));
    summaryLayout->addWidget(RatanaUI::makeMetricRow("Memory", "11.4 GB", "16 GB installed"));
    summaryLayout->addWidget(RatanaUI::makeMetricRow("GPU", "41%", "Wayland compositor healthy"));
    summaryLayout->addWidget(RatanaUI::makeMetricRow("Network", "312 Mbps", "Wi-Fi 6 uplink"));
    contentLayout->addWidget(summary);

    auto *table = new QTableWidget(5, 4);
    table->setHorizontalHeaderLabels({"Process", "CPU", "Memory", "State"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    const QList<QStringList> rows = {
      {"ratana-desktop-shell", "8.3%", "312 MB", "Responsive"},
      {"ratana-ai-assistant", "4.7%", "428 MB", "Streaming"},
      {"containerd", "3.1%", "215 MB", "Active"},
      {"kwin_wayland", "2.8%", "190 MB", "Healthy"},
      {"postgres", "1.6%", "482 MB", "Steady"},
    };

    for (int i = 0; i < rows.size(); ++i) {
      for (int j = 0; j < rows[i].size(); ++j) {
        table->setItem(i, j, new QTableWidgetItem(rows[i][j]));
      }
    }

    contentLayout->addWidget(table);

    auto *aside = RatanaUI::makeSectionList("Supervisor Notes", {
      "Wayland frame pacing within target",
      "No thermal throttling detected",
      "Two updates available for monitoring plugins"
    });

    RatanaUI::installWindowScaffold(surface, sidebar, content, aside);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  SystemMonitorWindow window;
  window.show();
  return app.exec();
}
