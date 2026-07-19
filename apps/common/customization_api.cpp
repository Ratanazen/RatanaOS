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
    bool dbSuccess = query.exec();

    // Dynamically regenerate the CSS file
    QString cssContent;
    if (themeName == "AMOLED") {
        cssContent = R"(/* RatanaOS AMOLED StyleSheet */
QWidget { background: #000000; color: #ffffff; font-family: "Noto Sans"; font-size: 14px; }
QMainWindow, QFrame#ShellSurface { background: #000000; }
QLabel#HeroTitle { font-size: 28px; font-weight: 700; color: #ff9900; }
QLabel#SectionTitle { font-size: 18px; font-weight: 700; color: #ffffff; }
QLabel#Eyebrow { font-size: 11px; letter-spacing: 1px; text-transform: uppercase; color: #888888; }
QFrame#Panel, QFrame#Card, QListWidget, QTableWidget, QTextEdit, QLineEdit {
  background: #0a0a0a; border: 1px solid #333333; border-radius: 18px;
}
QFrame#Sidebar { background: #050505; color: #ffffff; border-radius: 24px; border: 1px solid #222222; }
QPushButton { border: 1px solid #333333; border-radius: 16px; padding: 10px 14px; background: #111111; color: white;}
QPushButton:hover { background: #222222; }
QPushButton[primary="true"] { background: #ff9900; color: black; font-weight: 700; }
QHeaderView::section { background: #111111; border: 0; padding: 8px; font-weight: 700; }
)";
    } else if (themeName == "Light") {
        cssContent = R"(/* RatanaOS Light StyleSheet */
QWidget { background: #ffffff; color: #111111; font-family: "Noto Sans"; font-size: 14px; }
QMainWindow, QFrame#ShellSurface { background: #f0f0f0; }
QLabel#HeroTitle { font-size: 28px; font-weight: 700; color: #111111; }
QLabel#SectionTitle { font-size: 18px; font-weight: 700; color: #222222; }
QLabel#Eyebrow { font-size: 11px; letter-spacing: 1px; text-transform: uppercase; color: #555555; }
QFrame#Panel, QFrame#Card, QListWidget, QTableWidget, QTextEdit, QLineEdit {
  background: #ffffff; border: 1px solid #cccccc; border-radius: 18px;
}
QFrame#Sidebar { background: #e0e0e0; color: #111111; border-radius: 24px; border: 1px solid #cccccc; }
QPushButton { border: 1px solid #cccccc; border-radius: 16px; padding: 10px 14px; background: #f5f5f5; color: black;}
QPushButton:hover { background: #e8e8e8; }
QPushButton[primary="true"] { background: #0066cc; color: white; font-weight: 700; }
QHeaderView::section { background: #e0e0e0; border: 0; padding: 8px; font-weight: 700; }
)";
    } else {
        // Default Dark / Ratana Style
        cssContent = R"(/* RatanaOS Default Dark StyleSheet */
QWidget { background: #f5efe4; color: #1d1d1b; font-family: "Noto Sans"; font-size: 14px; }
QMainWindow, QFrame#ShellSurface {
  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f8f1e6, stop:0.55 #efe2ca, stop:1 #d9c5a1);
}
QLabel#HeroTitle { font-size: 28px; font-weight: 700; }
QLabel#SectionTitle { font-size: 18px; font-weight: 700; }
QLabel#Eyebrow { font-size: 11px; letter-spacing: 1px; text-transform: uppercase; color: #6a5d49; }
QFrame#Panel, QFrame#Card, QListWidget, QTableWidget, QTextEdit, QLineEdit {
  background: rgba(255, 252, 247, 0.82); border: 1px solid rgba(96, 78, 43, 0.15); border-radius: 18px;
}
QFrame#Sidebar { background: rgba(54, 38, 22, 0.88); color: #f7f1e6; border-radius: 24px; }
QPushButton { border: 0; border-radius: 16px; padding: 10px 14px; background: rgba(87, 70, 40, 0.08); }
QPushButton:hover { background: rgba(87, 70, 40, 0.14); }
QPushButton[primary="true"] { background: #b96f31; color: white; font-weight: 700; }
QHeaderView::section { background: rgba(87, 70, 40, 0.10); border: 0; padding: 8px; font-weight: 700; }
)";
    }

    QFile file("/home/ratana/RatanaOS/assets/ratanaos.css");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << cssContent;
        file.close();
    }
    
    // Also try writing to system path if running as root
    QFile sysFile("/usr/share/ratanaos/assets/ratanaos.css");
    if (sysFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&sysFile);
        out << cssContent;
        sysFile.close();
    }

    return dbSuccess;
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
