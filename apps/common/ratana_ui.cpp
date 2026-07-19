#include "ratana_ui.h"

#include <QFont>
#include <QFile>
#include <QTextStream>

namespace RatanaUI {

void applyAppTheme(QApplication &app) {
  QString cssPath = "/usr/share/ratanaos/assets/ratanaos.css";
  QFile file(cssPath);
  if (!file.exists()) {
    cssPath = "/home/ratana/RatanaOS/assets/ratanaos.css";
    file.setFileName(cssPath);
  }

  if (file.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream in(&file);
    app.setStyleSheet(in.readAll());
    file.close();
  } else {
    // Fallback if no CSS file exists
    app.setStyleSheet(R"(
      QWidget { background: #f5efe4; color: #1d1d1b; font-family: "Noto Sans"; font-size: 14px; }
      QPushButton { border-radius: 16px; padding: 10px 14px; background: rgba(87, 70, 40, 0.08); }
    )");
  }

  QFont font("Noto Sans", 11);
  app.setFont(font);
}

QWidget *makePanel(const QString &title, const QString &subtitle) {
  auto *panel = new QFrame;
  panel->setObjectName("Panel");
  auto *layout = new QVBoxLayout(panel);
  layout->setContentsMargins(20, 20, 20, 20);
  layout->setSpacing(8);

  auto *eyebrow = new QLabel("RatanaOS");
  eyebrow->setObjectName("Eyebrow");
  auto *heading = new QLabel(title);
  heading->setObjectName("SectionTitle");

  layout->addWidget(eyebrow);
  layout->addWidget(heading);

  if (!subtitle.isEmpty()) {
    auto *sub = new QLabel(subtitle);
    sub->setWordWrap(true);
    sub->setStyleSheet("color: #5d503c;");
    layout->addWidget(sub);
  }

  return panel;
}

QFrame *makeCard(const QString &eyebrow, const QString &title, const QString &body, const QColor &accent) {
  auto *card = new QFrame;
  card->setObjectName("Card");
  card->setStyleSheet(QString(
      "QFrame#Card { border-left: 5px solid %1; }").arg(accent.name()));

  auto *layout = new QVBoxLayout(card);
  layout->setContentsMargins(18, 18, 18, 18);
  layout->setSpacing(6);

  auto *eyebrowLabel = new QLabel(eyebrow.toUpper());
  eyebrowLabel->setObjectName("Eyebrow");
  auto *titleLabel = new QLabel(title);
  titleLabel->setStyleSheet("font-size: 20px; font-weight: 700;");
  auto *bodyLabel = new QLabel(body);
  bodyLabel->setWordWrap(true);
  bodyLabel->setStyleSheet("color: #584c39;");

  layout->addWidget(eyebrowLabel);
  layout->addWidget(titleLabel);
  layout->addWidget(bodyLabel);
  layout->addStretch();

  return card;
}

QPushButton *makePillButton(const QString &label, bool primary) {
  auto *button = new QPushButton(label);
  button->setProperty("primary", primary);
  return button;
}

QWidget *makeMetricRow(const QString &label, const QString &value, const QString &detail) {
  auto *row = new QWidget;
  auto *layout = new QHBoxLayout(row);
  layout->setContentsMargins(0, 0, 0, 0);

  auto *left = new QLabel(label);
  left->setStyleSheet("font-weight: 600;");
  auto *middle = new QLabel(value);
  middle->setStyleSheet("font-size: 18px; font-weight: 700;");
  auto *right = new QLabel(detail);
  right->setStyleSheet("color: #7a6c58;");

  layout->addWidget(left);
  layout->addStretch();
  layout->addWidget(middle);
  layout->addSpacing(16);
  layout->addWidget(right);

  return row;
}

QWidget *makeSectionList(const QString &title, const QStringList &items) {
  auto *panel = qobject_cast<QFrame *>(makePanel(title));
  auto *layout = qobject_cast<QVBoxLayout *>(panel->layout());
  for (const QString &item : items) {
    auto *label = new QLabel(QString("• %1").arg(item));
    label->setStyleSheet("padding: 6px 0;");
    layout->addWidget(label);
  }
  layout->addStretch();
  return panel;
}

QHBoxLayout *installWindowScaffold(
    QWidget *window,
    QWidget *sidebar,
    QWidget *content,
    QWidget *aside) {
  auto *root = new QHBoxLayout(window);
  root->setContentsMargins(24, 24, 24, 24);
  root->setSpacing(20);

  sidebar->setObjectName("Sidebar");
  sidebar->setMinimumWidth(220);
  root->addWidget(sidebar);
  root->addWidget(content, 1);

  if (aside) {
    aside->setMinimumWidth(260);
    root->addWidget(aside);
  }

  return root;
}

}  // namespace RatanaUI
