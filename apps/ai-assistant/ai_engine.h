#ifndef RATANA_AI_ENGINE_H
#define RATANA_AI_ENGINE_H

#include <QObject>
#include <QString>

class AIEngine : public QObject {
  Q_OBJECT

 public:
  enum class Context {
    EXPLAIN_COMMAND,
    TROUBLESHOOT,
    DOCUMENTATION,
    PACKAGES
  };

  explicit AIEngine(QObject *parent = nullptr);

  void submitQuery(Context context, const QString &query);

 signals:
  void responseReceived(const QString &response);

 private:
  QString mockInference(Context context, const QString &query);
};

#endif // RATANA_AI_ENGINE_H
