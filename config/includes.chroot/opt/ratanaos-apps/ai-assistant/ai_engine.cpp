#include "ai_engine.h"
#include <QTimer>

AIEngine::AIEngine(QObject *parent) : QObject(parent) {}

void AIEngine::submitQuery(Context context, const QString &query) {
  // Simulate network latency or on-device model inference time
  QString response = mockInference(context, query);
  QTimer::singleShot(1500, this, [this, response]() {
    emit responseReceived(response);
  });
}

QString AIEngine::mockInference(Context context, const QString &query) {
  if (query.trimmed().isEmpty()) {
    return "Please provide a query.";
  }

  if (query.contains("theme", Qt::CaseInsensitive) || 
      query.contains("wallpaper", Qt::CaseInsensitive)) {
      return "Opening Appearance Center";
  }
  
  if (query.contains("install apps", Qt::CaseInsensitive)) {
      return "Opening Software Center";
  }
  
  if (query.contains("fix settings", Qt::CaseInsensitive)) {
      return "Opening Settings";
  }

  if (query.contains("avatar", Qt::CaseInsensitive) ||
      query.contains("profile", Qt::CaseInsensitive)) {
      return "Opening Profile Manager";
  }

  switch (context) {
    case Context::EXPLAIN_COMMAND:
      if (query.contains("ls")) return "`ls` is a Linux command used to list directory contents. Using `-l` gives a long format, and `-a` shows hidden files.";
      if (query.contains("chmod")) return "`chmod` changes the file mode bits (permissions). For example, `chmod +x` makes a file executable.";
      return "That command is a standard utility. Read its manual page via `man " + query + "` for full details.";
      
    case Context::TROUBLESHOOT:
      if (query.contains("boot")) return "Boot issues often stem from GRUB configuration or missing initramfs modules. Check `/var/log/boot.log`.";
      if (query.contains("network")) return "If the network is down, check `systemctl status NetworkManager` and ensure your interfaces are UP via `ip a`.";
      return "I recommend checking `journalctl -xe` for the most recent system errors related to that issue.";
      
    case Context::DOCUMENTATION:
      return "Based on the Arch Wiki and Debian Docs, the configuration for " + query + " is typically found in `/etc/`.";
      
    case Context::PACKAGES:
      if (query.contains("browser")) return "I recommend installing `firefox` or `chromium` from the Ratana Software Center.";
      if (query.contains("editor")) return "For coding, `vim`, `nano`, and `vscode` are highly recommended packages.";
      return "You can use `apt-cache search " + query + "` or the Software Center to find related packages.";
  }
  
  return "I'm not sure how to respond to that.";
}
