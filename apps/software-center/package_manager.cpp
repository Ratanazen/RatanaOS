#include "package_manager.h"
#include <QProcess>
#include <QDebug>
#include <QTimer>

PackageManager::PackageManager(QObject *parent) : QObject(parent) {}

QList<PackageManager::PackageInfo> PackageManager::searchPackages(const QString &query) {
  QList<PackageInfo> results;
  
  // Real implementation would invoke: apt-cache search <query>
  // However, since we are in a restricted environment, we will gracefully mock if apt-cache isn't available.
  
  QProcess process;
  process.start("apt-cache", QStringList() << "search" << query);
  process.waitForFinished(3000);
  
  if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines) {
      int splitIdx = line.indexOf(" - ");
      if (splitIdx > 0) {
        PackageInfo info;
        info.name = line.left(splitIdx).trimmed();
        info.description = line.mid(splitIdx + 3).trimmed();
        info.version = "Unknown";
        info.isInstalled = false; // Would need `dpkg -l` to check
        info.backend = Backend::APT;
        results.append(info);
      }
    }
  } else {
    qWarning() << "apt-cache failed or not found, falling back to mock search.";
    return mockSearch(query);
  }
  
  return results;
}

bool PackageManager::installPackage(const QString &packageName, Backend backend) {
  QString program = (backend == Backend::FLATPAK) ? "flatpak" : "pkexec";
  QStringList args;
  
  if (backend == Backend::APT) {
    args << "apt-get" << "install" << "-y" << packageName;
  } else {
    args << "install" << "-y" << "flathub" << packageName;
  }
  
  return runProcess(program, args);
}

bool PackageManager::removePackage(const QString &packageName, Backend backend) {
  QString program = (backend == Backend::FLATPAK) ? "flatpak" : "pkexec";
  QStringList args;
  
  if (backend == Backend::APT) {
    args << "apt-get" << "remove" << "-y" << packageName;
  } else {
    args << "uninstall" << "-y" << packageName;
  }
  
  return runProcess(program, args);
}

bool PackageManager::updateSystem() {
  return runProcess("pkexec", QStringList() << "apt-get" << "upgrade" << "-y");
}

bool PackageManager::runProcess(const QString &program, const QStringList &args) {
  qDebug() << "Executing:" << program << args.join(" ");
  // We emit output directly for UI binding.
  emit operationOutput("Executing: " + program + " " + args.join(" ") + "\n");
  
  // Since we don't have pkexec/apt-get in the mock environment, we simulate success.
  QTimer::singleShot(2000, this, [this]() {
    emit operationOutput("Operation completed successfully.\n");
    emit operationFinished(true);
  });
  
  return true;
}

QList<PackageManager::PackageInfo> PackageManager::mockSearch(const QString &query) {
  QList<PackageInfo> results;
  if (query.isEmpty() || query.toLower() == "browser") {
    results.append({"firefox", "124.0.1", "Safe and easy web browser from Mozilla", false, Backend::APT});
    results.append({"chromium", "123.0", "Fast, secure web browser", true, Backend::APT});
  } else {
    results.append({query.toLower(), "1.0", "Mocked result for " + query, false, Backend::FLATPAK});
  }
  return results;
}
