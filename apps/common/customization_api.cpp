#include "customization_api.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>

namespace RatanaOS {
namespace Customization {

static const QString PROFILE_DIR = QDir::homePath() + "/.config/ratana/profile";
static const QString DB_DIR = QDir::homePath() + "/.local/share/ratana";

void initDatabases() {
    QDir().mkpath(DB_DIR);
    QDir().mkpath(PROFILE_DIR);
    
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "SettingsDB");
    db.setDatabaseName(DB_DIR + "/settings.db");
    if (db.open()) {
        QSqlQuery query(db);
        query.exec("CREATE TABLE IF NOT EXISTS preferences (key TEXT PRIMARY KEY, value TEXT)");
    }
    
    QSqlDatabase themeDb = QSqlDatabase::addDatabase("QSQLITE", "ThemesDB");
    themeDb.setDatabaseName(DB_DIR + "/themes.db");
    if (themeDb.open()) {
        QSqlQuery query(themeDb);
        query.exec("CREATE TABLE IF NOT EXISTS themes (id TEXT PRIMARY KEY, type TEXT, path TEXT)");
        query.exec("CREATE TABLE IF NOT EXISTS wallpapers (id INTEGER PRIMARY KEY AUTOINCREMENT, path TEXT, date_added TEXT)");
    }
    
    QSqlDatabase profileDb = QSqlDatabase::addDatabase("QSQLITE", "ProfilesDB");
    profileDb.setDatabaseName(DB_DIR + "/profiles.db");
    if (profileDb.open()) {
        QSqlQuery query(profileDb);
        query.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, avatar_path TEXT, created_at TEXT)");
    }
}

bool setAvatar(const QString& imagePath) {
    QDir().mkpath(PROFILE_DIR);
    QString dest = PROFILE_DIR + "/avatar.png";
    if (QFile::exists(dest)) QFile::remove(dest);
    return QFile::copy(imagePath, dest);
}

bool removeAvatar() {
    return QFile::remove(PROFILE_DIR + "/avatar.png");
}

QString getAvatarPath() {
    return PROFILE_DIR + "/avatar.png";
}

QVariantMap getUserSettings() {
    QVariantMap settings;
    QFile file(PROFILE_DIR + "/settings.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        settings = doc.object().toVariantMap();
    }
    return settings;
}

bool setTheme(const QString& themeName) {
    initDatabases();
    QSqlDatabase db = QSqlDatabase::database("SettingsDB");
    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO preferences (key, value) VALUES (?, ?)");
    query.addBindValue("theme");
    query.addBindValue(themeName);
    return query.exec();
}

QString getTheme() {
    initDatabases();
    QSqlDatabase db = QSqlDatabase::database("SettingsDB");
    QSqlQuery query(db);
    query.exec("SELECT value FROM preferences WHERE key='theme'");
    if (query.next()) return query.value(0).toString();
    return "Dark";
}

bool setIconPack(const QString& iconPackName) {
    return true;
}

QString getIconPack() {
    return "Ratana-Icons";
}

bool setWallpaper(const QString& imagePath) {
    return true;
}

QString getWallpaper() {
    return "/usr/share/ratana/themes/default.jpg";
}

bool setLoginBackground(const QString& imagePath) {
    QString destDir = "/tmp/ratana_login";
    QDir().mkpath(destDir);
    return QFile::copy(imagePath, destDir + "/background.jpg");
}

} // namespace Customization
} // namespace RatanaOS
