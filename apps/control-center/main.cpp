#include "../common/ratana_ui.h"

#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QStackedWidget>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>

class ControlCenterWindow : public QMainWindow {
  QStackedWidget *content;

 public:
  ControlCenterWindow() {
    setWindowTitle("Ratana Settings");
    resize(1200, 800);

    auto *surface = new QWidget;
    setCentralWidget(surface);
    auto *root = new QHBoxLayout(surface);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Sidebar ───────────────────────────────────────────────────
    auto *sidebar = new QFrame;
    sidebar->setObjectName("DesktopPanel");
    sidebar->setFixedWidth(220);
    auto *sbl = new QVBoxLayout(sidebar);
    sbl->setContentsMargins(12, 20, 12, 12);
    auto *title = new QLabel("Settings");
    title->setObjectName("HeroTitle");
    sbl->addWidget(title);
    sbl->addSpacing(8);

    const QStringList sections = {
      "Appearance", "Display", "Sound", "Network",
      "Bluetooth", "Power", "Privacy", "Users",
      "Updates", "Security", "Accessibility", "About"
    };
    content = new QStackedWidget;
    int idx = 0;
    for (const QString &sec : sections) {
      auto *btn = RatanaUI::makePillButton(sec, idx == 0);
      QObject::connect(btn, &QPushButton::clicked, [this, idx](){ content->setCurrentIndex(idx); });
      sbl->addWidget(btn);
      ++idx;
    }
    sbl->addStretch();
    root->addWidget(sidebar);

    // ── Appearance ────────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p); l->setContentsMargins(32,24,32,24);
      l->addWidget(RatanaUI::makePanel("Appearance", "Customize the look and feel of your desktop."));
      auto *themePanel = RatanaUI::makePanel("Color Theme");
      auto *tl = qobject_cast<QVBoxLayout*>(themePanel->layout());
      auto *themeCombo = new QComboBox;
      themeCombo->addItems({"Ratana Dark", "Ratana Light", "Ratana Midnight", "Ratana Daybreak"});
      tl->addWidget(themeCombo);
      auto *accentCombo = new QComboBox;
      accentCombo->addItems({"Electric Blue (Default)", "Cyber Green", "Ember Orange", "Violet"});
      tl->addWidget(new QLabel("Accent Color:")); tl->addWidget(accentCombo);
      l->addWidget(themePanel);
      auto *iconPanel = RatanaUI::makePanel("Icons & Fonts");
      auto *il = qobject_cast<QVBoxLayout*>(iconPanel->layout());
      il->addWidget(new QLabel("Icon Pack:"));
      auto *iconCombo = new QComboBox; iconCombo->addItems({"Ratana Icons", "Papirus", "Breeze"});
      il->addWidget(iconCombo);
      il->addWidget(new QLabel("Font Size:")); auto *fs = new QSlider(Qt::Horizontal);
      fs->setRange(8, 18); fs->setValue(11); il->addWidget(fs);
      l->addWidget(iconPanel); l->addStretch();
      content->addWidget(p);
    }

    // ── Display ───────────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p); l->setContentsMargins(32,24,32,24);
      l->addWidget(RatanaUI::makePanel("Display", "Configure your monitor resolution and refresh rate."));
      auto *dp = RatanaUI::makePanel("Monitor");
      auto *dl = qobject_cast<QVBoxLayout*>(dp->layout());
      dl->addWidget(new QLabel("Resolution:")); auto *res = new QComboBox;
      res->addItems({"3840×2160 (4K)", "2560×1440 (QHD)", "1920×1080 (Full HD)", "1366×768", "1280×720"});
      dl->addWidget(res);
      dl->addWidget(new QLabel("Refresh Rate:")); auto *rr = new QComboBox;
      rr->addItems({"144 Hz", "120 Hz", "60 Hz", "30 Hz"});
      dl->addWidget(rr);
      dl->addWidget(new QLabel("Brightness:")); auto *br = new QSlider(Qt::Horizontal);
      br->setRange(10, 100); br->setValue(80); dl->addWidget(br);
      dl->addWidget(new QCheckBox("Night Mode (reduce blue light)"));
      l->addWidget(dp); l->addStretch();
      content->addWidget(p);
    }

    // ── Sound ─────────────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p); l->setContentsMargins(32,24,32,24);
      l->addWidget(RatanaUI::makePanel("Sound", "Adjust audio output and input settings."));
      auto *sp = RatanaUI::makePanel("Output");
      auto *sl = qobject_cast<QVBoxLayout*>(sp->layout());
      sl->addWidget(new QLabel("Output Device:")); auto *od = new QComboBox;
      od->addItems({"Built-in Speakers", "Headphones", "HDMI Audio", "Bluetooth Headset"});
      sl->addWidget(od);
      sl->addWidget(new QLabel("Volume:")); auto *vol = new QSlider(Qt::Horizontal);
      vol->setRange(0, 100); vol->setValue(70); sl->addWidget(vol);
      sl->addWidget(new QCheckBox("Mute all sounds"));
      l->addWidget(sp);
      auto *ip = RatanaUI::makePanel("Input");
      auto *il = qobject_cast<QVBoxLayout*>(ip->layout());
      il->addWidget(new QLabel("Input Device:")); auto *mic = new QComboBox;
      mic->addItems({"Built-in Microphone", "USB Microphone", "Headset Microphone"});
      il->addWidget(mic);
      il->addWidget(new QLabel("Input Level:")); auto *inVol = new QSlider(Qt::Horizontal);
      inVol->setRange(0, 100); inVol->setValue(80); il->addWidget(inVol);
      l->addWidget(ip); l->addStretch();
      content->addWidget(p);
    }

    // ── Network ───────────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p); l->setContentsMargins(32,24,32,24);
      l->addWidget(RatanaUI::makePanel("Network", "Manage WiFi, Ethernet, and VPN connections."));
      auto *np = RatanaUI::makePanel("Wi-Fi Networks");
      auto *nl = qobject_cast<QVBoxLayout*>(np->layout());
      nl->addWidget(new QCheckBox("Enable Wi-Fi"));
      auto *wifiList = new QListWidget;
      wifiList->addItems({"🔒 HomeNetwork_5G  ▓▓▓▓░  Connected",
                          "🔒 Office_WiFi       ▓▓▓░░  Available",
                          "   Guest_Network    ▓▓░░░  Available"});
      nl->addWidget(wifiList);
      nl->addWidget(RatanaUI::makePillButton("Connect", true));
      l->addWidget(np);
      auto *vp = RatanaUI::makePanel("VPN");
      auto *vl = qobject_cast<QVBoxLayout*>(vp->layout());
      vl->addWidget(new QCheckBox("Enable VPN"));
      vl->addWidget(new QLabel("Configuration File:"));
      vl->addWidget(new QLineEdit("/etc/vpn/ratana.conf"));
      l->addWidget(vp); l->addStretch();
      content->addWidget(p);
    }

    // ── Bluetooth ─────────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p); l->setContentsMargins(32,24,32,24);
      l->addWidget(RatanaUI::makePanel("Bluetooth", "Pair and manage Bluetooth devices."));
      auto *bp = RatanaUI::makePanel("Devices");
      auto *bl = qobject_cast<QVBoxLayout*>(bp->layout());
      bl->addWidget(new QCheckBox("Enable Bluetooth"));
      auto *btList = new QListWidget;
      btList->addItems({"🎧 Sony WH-1000XM5  (Paired, Connected)", "🖱 Logitech MX Master  (Paired)", "⌨ Keyboard K380  (Available)"});
      bl->addWidget(btList);
      bl->addWidget(RatanaUI::makePillButton("Scan for Devices"));
      l->addWidget(bp); l->addStretch();
      content->addWidget(p);
    }

    // ── Power ─────────────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p); l->setContentsMargins(32,24,32,24);
      l->addWidget(RatanaUI::makePanel("Power", "Manage power profiles and sleep settings."));
      auto *pp = RatanaUI::makePanel("Power Profile");
      auto *pl = qobject_cast<QVBoxLayout*>(pp->layout());
      auto *modeCombo = new QComboBox;
      modeCombo->addItems({"Performance", "Balanced (Recommended)", "Power Saver"});
      modeCombo->setCurrentIndex(1);
      pl->addWidget(modeCombo);
      pl->addWidget(new QLabel("Sleep after (minutes):"));
      auto *sleep = new QSpinBox; sleep->setRange(1, 60); sleep->setValue(10);
      pl->addWidget(sleep);
      pl->addWidget(new QCheckBox("Suspend on lid close"));
      l->addWidget(pp); l->addStretch();
      content->addWidget(p);
    }

    // ── Privacy ───────────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p); l->setContentsMargins(32,24,32,24);
      l->addWidget(RatanaUI::makePanel("Privacy", "Control what data the system collects and shares."));
      auto *priv = RatanaUI::makePanel("Data & Usage");
      auto *pvl = qobject_cast<QVBoxLayout*>(priv->layout());
      pvl->addWidget(new QCheckBox("Share anonymized diagnostics (opt-in)"));
      pvl->addWidget(new QCheckBox("Enable location services"));
      pvl->addWidget(new QCheckBox("Allow app telemetry"));
      pvl->addWidget(new QCheckBox("Remember recent files"));
      l->addWidget(priv); l->addStretch();
      content->addWidget(p);
    }

    // ── Users ─────────────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p); l->setContentsMargins(32,24,32,24);
      l->addWidget(RatanaUI::makePanel("Users & Accounts", "Manage user accounts and login options."));
      auto *up = RatanaUI::makePanel("Current User");
      auto *ul = qobject_cast<QVBoxLayout*>(up->layout());
      ul->addWidget(new QLabel("Full Name:")); ul->addWidget(new QLineEdit("RatanaOS User"));
      ul->addWidget(new QLabel("Username:")); ul->addWidget(new QLineEdit("ratana"));
      ul->addWidget(new QLabel("Password:")); auto *pw = new QLineEdit; pw->setEchoMode(QLineEdit::Password); ul->addWidget(pw);
      ul->addWidget(new QCheckBox("Auto-login"));
      l->addWidget(up); l->addStretch();
      content->addWidget(p);
    }

    // ── Updates ───────────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p); l->setContentsMargins(32,24,32,24);
      l->addWidget(RatanaUI::makePanel("System Updates", "Configure how your system receives updates."));
      auto *upanel = RatanaUI::makePanel("Update Settings");
      auto *upl = qobject_cast<QVBoxLayout*>(upanel->layout());
      auto *channelCombo = new QComboBox;
      channelCombo->addItems({"Stable (Recommended)", "Beta", "Nightly"});
      upl->addWidget(new QLabel("Update Channel:")); upl->addWidget(channelCombo);
      upl->addWidget(new QCheckBox("Automatic updates"));
      upl->addWidget(new QCheckBox("Automatic security patches"));
      upl->addWidget(new QCheckBox("Use atomic updates (Btrfs snapshot before each update)"));
      upl->addWidget(RatanaUI::makePillButton("Check for Updates Now", true));
      l->addWidget(upanel); l->addStretch();
      content->addWidget(p);
    }

    // ── Security ──────────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p); l->setContentsMargins(32,24,32,24);
      l->addWidget(RatanaUI::makePanel("Security", "System hardening and firewall configuration."));
      auto *sp = RatanaUI::makePanel("Firewall & Access");
      auto *sl = qobject_cast<QVBoxLayout*>(sp->layout());
      sl->addWidget(new QCheckBox("Enable UFW Firewall (Recommended)"));
      sl->addWidget(new QCheckBox("Enable AppArmor MAC profiles"));
      sl->addWidget(new QCheckBox("Enable fail2ban intrusion detection"));
      sl->addWidget(new QCheckBox("Enable automatic security patches"));
      sl->addWidget(RatanaUI::makePillButton("Open Ratana Firewall →"));
      l->addWidget(sp); l->addStretch();
      content->addWidget(p);
    }

    // ── Accessibility ─────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p); l->setContentsMargins(32,24,32,24);
      l->addWidget(RatanaUI::makePanel("Accessibility", "Assistive technology and visual accessibility options."));
      auto *ap = RatanaUI::makePanel("Visual Assistance");
      auto *al = qobject_cast<QVBoxLayout*>(ap->layout());
      al->addWidget(new QCheckBox("Enable screen reader (Orca)"));
      al->addWidget(new QCheckBox("High contrast theme"));
      al->addWidget(new QCheckBox("Large text mode"));
      al->addWidget(new QLabel("Magnification:")); auto *mag = new QSlider(Qt::Horizontal);
      mag->setRange(100, 400); mag->setValue(100); al->addWidget(mag);
      al->addWidget(new QCheckBox("Reduce motion effects"));
      al->addWidget(new QCheckBox("Sticky keys"));
      al->addWidget(new QCheckBox("Slow keys"));
      l->addWidget(ap); l->addStretch();
      content->addWidget(p);
    }

    // ── About ─────────────────────────────────────────────────────
    {
      auto *p = new QWidget; auto *l = new QVBoxLayout(p); l->setContentsMargins(32,24,32,24);
      l->addWidget(RatanaUI::makePanel("About RatanaOS",
        "Version: 5.0.0 Phoenix\n"
        "Base: Debian Bookworm (Stable)\n"
        "Kernel: Linux 6.x\n"
        "Desktop: KDE Plasma\n"
        "Architecture: amd64\n\n"
        "© 2026 RatanaOS Project. Original design."));
      l->addWidget(RatanaUI::makePillButton("Check for OS Updates", true));
      l->addStretch();
      content->addWidget(p);
    }

    root->addWidget(content, 1);
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  RatanaUI::applyAppTheme(app);
  ControlCenterWindow window;
  window.show();
  return app.exec();
}
