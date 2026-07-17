#pragma once

#include <QApplication>
#include <QColor>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

namespace RatanaUI {

void applyAppTheme(QApplication &app);

QWidget *makePanel(const QString &title, const QString &subtitle = QString());
QFrame *makeCard(
    const QString &eyebrow,
    const QString &title,
    const QString &body,
    const QColor &accent);
QPushButton *makePillButton(const QString &label, bool primary = false);
QWidget *makeMetricRow(const QString &label, const QString &value, const QString &detail);
QWidget *makeSectionList(const QString &title, const QStringList &items);
QHBoxLayout *installWindowScaffold(
    QWidget *window,
    QWidget *sidebar,
    QWidget *content,
    QWidget *aside = nullptr);

}  // namespace RatanaUI
