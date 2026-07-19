#ifndef RATANA_PACKAGE_MANAGER_H
#define RATANA_PACKAGE_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>

class PackageManager : public QObject {
  Q_OBJECT

 public:
  enum class Backend { APT, FLATPAK };

  struct PackageInfo {
    QString name;
    QString version;
    QString description;
    bool isInstalled;
    Backend backend;
  };

  explicit PackageManager(QObject *parent = nullptr);

  QList<PackageInfo> searchPackages(const QString &query);
  bool installPackage(const QString &packageName, Backend backend = Backend::APT);
  bool removePackage(const QString &packageName, Backend backend = Backend::APT);
  bool updateSystem();

 signals:
  void operationOutput(const QString &output);
  void operationFinished(bool success);

 private:
  QList<PackageInfo> mockSearch(const QString &query);
  bool runProcess(const QString &program, const QStringList &args);
};

#endif // RATANA_PACKAGE_MANAGER_H
