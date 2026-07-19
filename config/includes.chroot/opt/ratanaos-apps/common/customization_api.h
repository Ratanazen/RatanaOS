#ifndef RATANA_CUSTOMIZATION_API_H
#define RATANA_CUSTOMIZATION_API_H

#include <QString>
#include <QVariantMap>

namespace RatanaOS {
namespace Customization {

// Profile Settings
bool setAvatar(const QString& imagePath);
bool removeAvatar();
QString getAvatarPath();
QVariantMap getUserSettings();

// Appearance Settings
bool setTheme(const QString& themeName);
QString getTheme();

bool setIconPack(const QString& iconPackName);
QString getIconPack();

bool setWallpaper(const QString& imagePath);
QString getWallpaper();

// Login Settings
bool setLoginBackground(const QString& imagePath);

} // namespace Customization
} // namespace RatanaOS

#endif // RATANA_CUSTOMIZATION_API_H
