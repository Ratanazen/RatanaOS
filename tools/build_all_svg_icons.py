#!/usr/bin/env python3
import os
import subprocess
import base64

ROOT = "/home/reny/Documents/OS"
CHROOT = os.path.join(ROOT, "rios-live/config/includes.chroot")

# Directories
ICONS_SCALABLE_APPS = os.path.join(CHROOT, "usr/share/icons/hicolor/scalable/apps")
ICONS_SCALABLE_PLACES = os.path.join(CHROOT, "usr/share/icons/hicolor/scalable/places")
PIXMAPS = os.path.join(CHROOT, "usr/share/pixmaps")
PLYMOUTH = os.path.join(CHROOT, "usr/share/plymouth/themes/rios")
ASSETS = os.path.join(CHROOT, "usr/share/rios/assets")
CALAMARES_BRANDING = os.path.join(CHROOT, "etc/calamares/branding/rios")
APPLICATIONS = os.path.join(CHROOT, "usr/share/applications")

SIZES = [16, 24, 32, 48, 64, 128, 256, 512]

for d in [ICONS_SCALABLE_APPS, ICONS_SCALABLE_PLACES, PIXMAPS, PLYMOUTH, ASSETS, CALAMARES_BRANDING, APPLICATIONS]:
    os.makedirs(d, exist_ok=True)

for sz in SIZES:
    os.makedirs(os.path.join(CHROOT, f"usr/share/icons/hicolor/{sz}x{sz}/apps"), exist_ok=True)
    os.makedirs(os.path.join(CHROOT, f"usr/share/icons/hicolor/{sz}x{sz}/places"), exist_ok=True)

# 1. Official RiOS Logo SVG
RI_LOGO_SVG = '''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="512" height="512">
  <defs>
    <radialGradient id="bgGlow" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="#1e293b"/>
      <stop offset="65%" stop-color="#0f172a"/>
      <stop offset="100%" stop-color="#020617"/>
    </radialGradient>
    <linearGradient id="cyberBorder" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#00f0ff"/>
      <stop offset="35%" stop-color="#3b82f6"/>
      <stop offset="70%" stop-color="#8b5cf6"/>
      <stop offset="100%" stop-color="#00f0ff"/>
    </linearGradient>
    <linearGradient id="shieldGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#1e1b4b"/>
      <stop offset="50%" stop-color="#0f172a"/>
      <stop offset="100%" stop-color="#020617"/>
    </linearGradient>
    <linearGradient id="cyanNeon" x1="0%" y1="0%" x2="100%" y2="0%">
      <stop offset="0%" stop-color="#38bdf8"/>
      <stop offset="100%" stop-color="#00f0ff"/>
    </linearGradient>
    <linearGradient id="silverGrad" x1="0%" y1="0%" x2="0%" y2="100%">
      <stop offset="0%" stop-color="#ffffff"/>
      <stop offset="50%" stop-color="#cbd5e1"/>
      <stop offset="100%" stop-color="#94a3b8"/>
    </linearGradient>
    <filter id="neonGlow" x="-20%" y="-20%" width="140%" height="140%">
      <feGaussianBlur stdDeviation="6" result="blur"/>
      <feComposite in="SourceGraphic" in2="blur" operator="over"/>
    </filter>
  </defs>

  <!-- Outer Ring Background -->
  <circle cx="256" cy="256" r="248" fill="url(#bgGlow)"/>
  <circle cx="256" cy="256" r="248" fill="none" stroke="url(#cyberBorder)" stroke-width="8"/>
  <circle cx="256" cy="256" r="236" fill="none" stroke="#00f0ff" stroke-width="2" opacity="0.6"/>

  <!-- Top Header: RIOS -->
  <text x="256" y="58" font-family="DejaVu Sans, Inter, sans-serif" font-size="34" font-weight="900" fill="#00f0ff" text-anchor="middle" letter-spacing="14" filter="url(#neonGlow)">RIOS</text>
  <text x="256" y="58" font-family="DejaVu Sans, Inter, sans-serif" font-size="34" font-weight="900" fill="#ffffff" text-anchor="middle" letter-spacing="14">RIOS</text>

  <!-- Bottom Footer: DEBIAN 12 • CYBER OS -->
  <text x="256" y="474" font-family="DejaVu Sans, Inter, sans-serif" font-size="18" font-weight="800" fill="#38bdf8" text-anchor="middle" letter-spacing="8">DEBIAN 12 • CYBER OS</text>

  <!-- Left / Right Cyber Stars -->
  <polygon points="56,256 61,267 73,267 64,274 67,285 56,278 45,285 48,274 39,267 51,267" fill="#00f0ff"/>
  <polygon points="456,256 461,267 473,267 464,274 467,285 456,278 445,285 448,274 439,267 451,267" fill="#00f0ff"/>

  <!-- Inner Crest Ring -->
  <circle cx="256" cy="256" r="162" fill="url(#shieldGrad)" stroke="#38bdf8" stroke-width="3"/>
  <circle cx="256" cy="256" r="154" fill="none" stroke="#6366f1" stroke-width="1.5" stroke-dasharray="6 4" opacity="0.7"/>

  <!-- Radar / Cyber Pulse Grid -->
  <circle cx="256" cy="256" r="122" fill="none" stroke="#0284c7" stroke-width="1" opacity="0.4"/>
  <circle cx="256" cy="256" r="85" fill="none" stroke="#0284c7" stroke-width="1" opacity="0.4"/>
  <circle cx="256" cy="256" r="50" fill="none" stroke="#0284c7" stroke-width="1" opacity="0.4"/>
  <line x1="256" y1="94" x2="256" y2="418" stroke="#0284c7" stroke-width="1" opacity="0.3"/>
  <line x1="94" y1="256" x2="418" y2="256" stroke="#0284c7" stroke-width="1" opacity="0.3"/>

  <!-- Center Shield Emblem -->
  <path d="M 256,128 
           C 312,128 352,142 372,168 
           C 372,264 332,344 256,388 
           C 180,344 140,264 140,168 
           C 160,142 200,128 256,128 Z" 
        fill="#0a0f1d" stroke="url(#cyanNeon)" stroke-width="5" filter="url(#neonGlow)"/>

  <!-- Stylized Cyber Falcon & Circuit Crest -->
  <g transform="translate(256, 252) scale(0.96) translate(-256, -252)">
    <path d="M 256,155 L 290,195 L 325,185 L 295,215 L 305,245 L 256,220 L 207,245 L 217,215 L 187,185 L 222,195 Z" 
          fill="url(#silverGrad)" stroke="#38bdf8" stroke-width="2"/>
    <circle cx="256" cy="190" r="7" fill="#00f0ff" filter="url(#neonGlow)"/>
    <circle cx="256" cy="190" r="3.5" fill="#ffffff"/>

    <path d="M 215,220 L 172,255 L 172,295 L 195,310 L 225,290 L 225,250" 
          fill="none" stroke="#38bdf8" stroke-width="3" stroke-linecap="round"/>
    <circle cx="172" cy="255" r="4" fill="#38bdf8"/>
    <circle cx="172" cy="295" r="4" fill="#38bdf8"/>
    <circle cx="195" cy="310" r="4" fill="#00f0ff"/>

    <path d="M 297,220 L 340,255 L 340,295 L 317,310 L 287,290 L 287,250" 
          fill="none" stroke="#38bdf8" stroke-width="3" stroke-linecap="round"/>
    <circle cx="340" cy="255" r="4" fill="#38bdf8"/>
    <circle cx="340" cy="295" r="4" fill="#38bdf8"/>
    <circle cx="317" cy="310" r="4" fill="#00f0ff"/>

    <polygon points="256,242 280,278 256,338 232,278" fill="#00f0ff" opacity="0.9" filter="url(#neonGlow)"/>
    <polygon points="256,254 270,278 256,320 242,278" fill="#ffffff"/>
  </g>
</svg>'''

# 2. Symbolic Logo SVG (for panel/top-bar)
RI_LOGO_SYMBOLIC_SVG = '''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" width="16" height="16">
  <path d="M 8,1 C 10.5,1 12.5,1.8 13.5,3 C 13.5,8 11.5,12 8,15 C 4.5,12 2.5,8 2.5,3 C 3.5,1.8 5.5,1 8,1 Z" fill="#ffffff"/>
  <path d="M 8,3 L 9.5,5.5 L 11.5,5 L 10,7 L 10.5,9 L 8,7.5 L 5.5,9 L 6,7 L 4.5,5 L 6.5,5.5 Z" fill="#1e1e2e"/>
  <polygon points="8,8 9.5,10.5 8,13 6.5,10.5" fill="#1e1e2e"/>
</svg>'''

# 3. ri-cyber.svg
RI_CYBER_SVG = '''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="512" height="512">
  <defs>
    <radialGradient id="cyberBg" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="#0f172a"/>
      <stop offset="80%" stop-color="#020617"/>
      <stop offset="100%" stop-color="#000000"/>
    </radialGradient>
    <linearGradient id="redCyan" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#f43f5e"/>
      <stop offset="100%" stop-color="#06b6d4"/>
    </linearGradient>
  </defs>
  <circle cx="256" cy="256" r="240" fill="url(#cyberBg)" stroke="#06b6d4" stroke-width="6"/>
  <circle cx="256" cy="256" r="215" fill="none" stroke="#f43f5e" stroke-width="1.5" stroke-dasharray="8 6" opacity="0.6"/>
  
  <!-- Reticle Crosshairs -->
  <circle cx="256" cy="256" r="170" fill="none" stroke="#06b6d4" stroke-width="2" opacity="0.4"/>
  <line x1="256" y1="56" x2="256" y2="456" stroke="#06b6d4" stroke-width="2" opacity="0.3"/>
  <line x1="56" y1="256" x2="456" y2="256" stroke="#06b6d4" stroke-width="2" opacity="0.3"/>
  
  <!-- Cyber Shield -->
  <path d="M 256,120 C 320,120 370,140 390,170 C 390,290 330,370 256,410 C 182,370 122,290 122,170 C 142,140 192,120 256,120 Z" 
        fill="#111827" stroke="url(#redCyan)" stroke-width="8"/>
  
  <!-- Padlock / Keyhole -->
  <rect x="206" y="240" width="100" height="85" rx="14" fill="#f43f5e"/>
  <path d="M 226,240 L 226,200 C 226,175 286,175 286,200 L 286,240" fill="none" stroke="#06b6d4" stroke-width="12" stroke-linecap="round"/>
  <circle cx="256" cy="275" r="10" fill="#ffffff"/>
  <polygon points="252,275 260,275 264,305 248,305" fill="#ffffff"/>
  
  <!-- Corner Brackets -->
  <path d="M 170,140 L 140,140 L 140,170" fill="none" stroke="#00f0ff" stroke-width="4"/>
  <path d="M 342,140 L 372,140 L 372,170" fill="none" stroke="#00f0ff" stroke-width="4"/>
</svg>'''

# 4. ri-dev.svg
RI_DEV_SVG = '''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="512" height="512">
  <defs>
    <radialGradient id="devBg" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="#1e1b4b"/>
      <stop offset="70%" stop-color="#0f172a"/>
      <stop offset="100%" stop-color="#020617"/>
    </radialGradient>
    <linearGradient id="emeraldAzure" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#10b981"/>
      <stop offset="100%" stop-color="#3b82f6"/>
    </linearGradient>
  </defs>
  <circle cx="256" cy="256" r="240" fill="url(#devBg)" stroke="url(#emeraldAzure)" stroke-width="7"/>
  <rect x="80" y="110" width="352" height="292" rx="20" fill="#181926" stroke="#3b82f6" stroke-width="4"/>
  
  <!-- Terminal Top Bar -->
  <rect x="80" y="110" width="352" height="45" rx="20" fill="#24273a"/>
  <circle cx="115" cy="132" r="7" fill="#f38ba8"/>
  <circle cx="140" cy="132" r="7" fill="#f9e2af"/>
  <circle cx="165" cy="132" r="7" fill="#a6e3a1"/>
  
  <!-- Code Prompt & Brackets -->
  <path d="M 125,230 L 195,280 L 125,330" fill="none" stroke="#10b981" stroke-width="14" stroke-linecap="round" stroke-linejoin="round"/>
  <line x1="220" y1="330" x2="280" y2="330" stroke="#00f0ff" stroke-width="14" stroke-linecap="round"/>
  
  <!-- Closing Bracket and Gear -->
  <path d="M 387,230 L 317,280 L 387,330" fill="none" stroke="#3b82f6" stroke-width="14" stroke-linecap="round" stroke-linejoin="round"/>
</svg>'''

# 5. ri-server.svg
RI_SERVER_SVG = '''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="512" height="512">
  <defs>
    <radialGradient id="srvBg" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="#1e293b"/>
      <stop offset="70%" stop-color="#0f172a"/>
      <stop offset="100%" stop-color="#020617"/>
    </radialGradient>
  </defs>
  <circle cx="256" cy="256" r="240" fill="url(#srvBg)" stroke="#38bdf8" stroke-width="6"/>
  
  <!-- Server Unit 1 -->
  <rect x="100" y="110" width="312" height="70" rx="12" fill="#1e293b" stroke="#0284c7" stroke-width="3"/>
  <circle cx="140" cy="145" r="8" fill="#10b981"/>
  <circle cx="170" cy="145" r="8" fill="#38bdf8"/>
  <line x1="220" y1="135" x2="380" y2="135" stroke="#475569" stroke-width="4" stroke-linecap="round"/>
  <line x1="220" y1="155" x2="380" y2="155" stroke="#475569" stroke-width="4" stroke-linecap="round"/>
  
  <!-- Server Unit 2 -->
  <rect x="100" y="210" width="312" height="70" rx="12" fill="#1e293b" stroke="#0284c7" stroke-width="3"/>
  <circle cx="140" cy="245" r="8" fill="#10b981"/>
  <circle cx="170" cy="245" r="8" fill="#f59e0b"/>
  <line x1="220" y1="235" x2="380" y2="235" stroke="#475569" stroke-width="4" stroke-linecap="round"/>
  <line x1="220" y1="255" x2="380" y2="255" stroke="#475569" stroke-width="4" stroke-linecap="round"/>

  <!-- Server Unit 3 -->
  <rect x="100" y="310" width="312" height="70" rx="12" fill="#1e293b" stroke="#0284c7" stroke-width="3"/>
  <circle cx="140" cy="345" r="8" fill="#10b981"/>
  <circle cx="170" cy="345" r="8" fill="#00f0ff"/>
  <line x1="220" y1="335" x2="380" y2="335" stroke="#475569" stroke-width="4" stroke-linecap="round"/>
  <line x1="220" y1="355" x2="380" y2="355" stroke="#475569" stroke-width="4" stroke-linecap="round"/>
</svg>'''

# 6. ri-installer.svg
RI_INSTALLER_SVG = '''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="512" height="512">
  <defs>
    <radialGradient id="instBg" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="#1e1b4b"/>
      <stop offset="70%" stop-color="#0f172a"/>
      <stop offset="100%" stop-color="#020617"/>
    </radialGradient>
  </defs>
  <circle cx="256" cy="256" r="240" fill="url(#instBg)" stroke="#6366f1" stroke-width="6"/>
  
  <!-- Hard Drive Enclosure -->
  <rect x="110" y="100" width="292" height="312" rx="24" fill="#1e293b" stroke="#818cf8" stroke-width="5"/>
  <circle cx="256" cy="230" r="75" fill="#0f172a" stroke="#6366f1" stroke-width="6"/>
  <circle cx="256" cy="230" r="30" fill="#6366f1"/>
  
  <!-- Download / Install Arrow -->
  <path d="M 256,150 L 256,270 M 215,235 L 256,275 L 297,235" 
        fill="none" stroke="#00f0ff" stroke-width="12" stroke-linecap="round" stroke-linejoin="round"/>
  
  <!-- Status LEDs -->
  <circle cx="150" cy="370" r="8" fill="#10b981"/>
  <circle cx="180" cy="370" r="8" fill="#38bdf8"/>
  <line x1="230" y1="370" x2="360" y2="370" stroke="#475569" stroke-width="6" stroke-linecap="round"/>
</svg>'''

# 7. ri-doctor.svg
RI_DOCTOR_SVG = '''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="512" height="512">
  <defs>
    <radialGradient id="docBg" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="#064e3b"/>
      <stop offset="70%" stop-color="#022c22"/>
      <stop offset="100%" stop-color="#020617"/>
    </radialGradient>
  </defs>
  <circle cx="256" cy="256" r="240" fill="url(#docBg)" stroke="#10b981" stroke-width="6"/>
  
  <!-- Heartbeat ECG Pulse -->
  <path d="M 70,256 L 160,256 L 195,170 L 235,340 L 275,200 L 310,290 L 335,256 L 442,256" 
        fill="none" stroke="#34d399" stroke-width="12" stroke-linecap="round" stroke-linejoin="round"/>
  
  <!-- Checkmark Badge -->
  <circle cx="256" cy="390" r="45" fill="#10b981"/>
  <path d="M 238,390 L 250,402 L 276,376" fill="none" stroke="#ffffff" stroke-width="6" stroke-linecap="round" stroke-linejoin="round"/>
</svg>'''

# 8. ri-gpu-setup.svg
RI_GPU_SVG = '''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="512" height="512">
  <defs>
    <radialGradient id="gpuBg" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="#14532d"/>
      <stop offset="70%" stop-color="#0f172a"/>
      <stop offset="100%" stop-color="#020617"/>
    </radialGradient>
  </defs>
  <circle cx="256" cy="256" r="240" fill="url(#gpuBg)" stroke="#22c55e" stroke-width="6"/>
  
  <!-- GPU Processor Chip -->
  <rect x="130" y="130" width="252" height="252" rx="20" fill="#1e293b" stroke="#22c55e" stroke-width="6"/>
  <rect x="180" y="180" width="152" height="152" rx="10" fill="#0f172a" stroke="#4ade80" stroke-width="3"/>
  <text x="256" y="270" font-family="DejaVu Sans, Inter, sans-serif" font-size="36" font-weight="900" fill="#22c55e" text-anchor="middle">GPU</text>

  <!-- External Chip Pins -->
  <line x1="170" y1="100" x2="170" y2="130" stroke="#22c55e" stroke-width="6"/>
  <line x1="210" y1="100" x2="210" y2="130" stroke="#22c55e" stroke-width="6"/>
  <line x1="256" y1="100" x2="256" y2="130" stroke="#22c55e" stroke-width="6"/>
  <line x1="302" y1="100" x2="302" y2="130" stroke="#22c55e" stroke-width="6"/>
  <line x1="342" y1="100" x2="342" y2="130" stroke="#22c55e" stroke-width="6"/>
  
  <line x1="170" y1="382" x2="170" y2="412" stroke="#22c55e" stroke-width="6"/>
  <line x1="210" y1="382" x2="210" y2="412" stroke="#22c55e" stroke-width="6"/>
  <line x1="256" y1="382" x2="256" y2="412" stroke="#22c55e" stroke-width="6"/>
  <line x1="302" y1="382" x2="302" y2="412" stroke="#22c55e" stroke-width="6"/>
  <line x1="342" y1="382" x2="342" y2="412" stroke="#22c55e" stroke-width="6"/>
</svg>'''

# 9. ri-harden.svg
RI_HARDEN_SVG = '''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="512" height="512">
  <defs>
    <radialGradient id="hardBg" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="#451a03"/>
      <stop offset="70%" stop-color="#1c1917"/>
      <stop offset="100%" stop-color="#0c0a09"/>
    </radialGradient>
  </defs>
  <circle cx="256" cy="256" r="240" fill="url(#hardBg)" stroke="#f59e0b" stroke-width="6"/>
  
  <!-- Vault Door / Shield -->
  <circle cx="256" cy="256" r="140" fill="#292524" stroke="#d97706" stroke-width="8"/>
  <circle cx="256" cy="256" r="115" fill="none" stroke="#f59e0b" stroke-width="3" stroke-dasharray="12 8"/>
  
  <!-- Vault Handle Spokes -->
  <circle cx="256" cy="256" r="35" fill="#f59e0b"/>
  <line x1="256" y1="160" x2="256" y2="352" stroke="#f59e0b" stroke-width="12" stroke-linecap="round"/>
  <line x1="160" y1="256" x2="352" y2="256" stroke="#f59e0b" stroke-width="12" stroke-linecap="round"/>
</svg>'''

# 10. ri-cve.svg
RI_CVE_SVG = '''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="512" height="512">
  <defs>
    <radialGradient id="cveBg" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="#701a75"/>
      <stop offset="70%" stop-color="#2e1065"/>
      <stop offset="100%" stop-color="#020617"/>
    </radialGradient>
  </defs>
  <circle cx="256" cy="256" r="240" fill="url(#cveBg)" stroke="#d946ef" stroke-width="6"/>
  
  <!-- Radar Sweep -->
  <circle cx="256" cy="256" r="150" fill="none" stroke="#a855f7" stroke-width="3"/>
  <circle cx="256" cy="256" r="90" fill="none" stroke="#a855f7" stroke-width="2"/>
  
  <!-- Vulnerability Bug Target -->
  <circle cx="256" cy="256" r="45" fill="#f43f5e"/>
  <circle cx="256" cy="190" r="25" fill="#f43f5e"/>
  
  <!-- Bug Legs -->
  <line x1="190" y1="230" x2="160" y2="210" stroke="#f43f5e" stroke-width="8" stroke-linecap="round"/>
  <line x1="190" y1="256" x2="150" y2="256" stroke="#f43f5e" stroke-width="8" stroke-linecap="round"/>
  <line x1="190" y1="280" x2="160" y2="300" stroke="#f43f5e" stroke-width="8" stroke-linecap="round"/>

  <line x1="322" y1="230" x2="352" y2="210" stroke="#f43f5e" stroke-width="8" stroke-linecap="round"/>
  <line x1="322" y1="256" x2="362" y2="256" stroke="#f43f5e" stroke-width="8" stroke-linecap="round"/>
  <line x1="322" y1="280" x2="352" y2="300" stroke="#f43f5e" stroke-width="8" stroke-linecap="round"/>
  
  <!-- Shield Protection Patch -->
  <path d="M 256,120 L 330,150 L 330,230 L 256,270 L 182,230 L 182,150 Z" 
        fill="#10b981" opacity="0.4" stroke="#34d399" stroke-width="4"/>
</svg>'''

# 11. ri-chroot.svg
RI_CHROOT_SVG = '''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" width="512" height="512">
  <defs>
    <radialGradient id="chrBg" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="#9a3412"/>
      <stop offset="70%" stop-color="#431407"/>
      <stop offset="100%" stop-color="#0c0a09"/>
    </radialGradient>
  </defs>
  <circle cx="256" cy="256" r="240" fill="url(#chrBg)" stroke="#ea580c" stroke-width="6"/>
  
  <!-- Lifebuoy / Rescue Ring -->
  <circle cx="256" cy="256" r="140" fill="none" stroke="#f97316" stroke-width="50"/>
  <circle cx="256" cy="256" r="140" fill="none" stroke="#ffffff" stroke-width="50" stroke-dasharray="73.3 146.6"/>
  <circle cx="256" cy="256" r="110" fill="none" stroke="#c2410c" stroke-width="4"/>
  <circle cx="256" cy="256" r="165" fill="none" stroke="#c2410c" stroke-width="4"/>
  
  <!-- Root Shell Prompt in Center -->
  <circle cx="256" cy="256" r="85" fill="#0f172a"/>
  <text x="256" y="278" font-family="monospace" font-size="64" font-weight="900" fill="#22c55e" text-anchor="middle">#</text>
</svg>'''

svg_map = {
    "ri-logo.svg": RI_LOGO_SVG,
    "distributor-logo.svg": RI_LOGO_SVG,
    "distributor-logo-debian.svg": RI_LOGO_SVG,
    "start-here.svg": RI_LOGO_SVG,
    "start-here-symbolic.svg": RI_LOGO_SYMBOLIC_SVG,
    "gnome-main-menu.svg": RI_LOGO_SVG,
    "ri-cyber.svg": RI_CYBER_SVG,
    "ri-dev.svg": RI_DEV_SVG,
    "ri-server.svg": RI_SERVER_SVG,
    "ri-installer.svg": RI_INSTALLER_SVG,
    "ri-doctor.svg": RI_DOCTOR_SVG,
    "ri-gpu-setup.svg": RI_GPU_SVG,
    "ri-harden.svg": RI_HARDEN_SVG,
    "ri-cve.svg": RI_CVE_SVG,
    "ri-chroot.svg": RI_CHROOT_SVG,
}

print("==> Writing SVG Vector Icons...")
for name, content in svg_map.items():
    # Write to scalable/apps
    p_apps = os.path.join(ICONS_SCALABLE_APPS, name)
    with open(p_apps, "w") as f:
        f.write(content)
        
    # Write to pixmaps
    p_pix = os.path.join(PIXMAPS, name)
    with open(p_pix, "w") as f:
        f.write(content)

    if name in ["start-here.svg", "start-here-symbolic.svg", "distributor-logo.svg"]:
        p_places = os.path.join(ICONS_SCALABLE_PLACES, name)
        with open(p_places, "w") as f:
            f.write(content)

# Plymouth and Assets
with open(os.path.join(PLYMOUTH, "logo.svg"), "w") as f:
    f.write(RI_LOGO_SVG)
with open(os.path.join(ASSETS, "ri-logo.svg"), "w") as f:
    f.write(RI_LOGO_SVG)
with open(os.path.join(ASSETS, "distributor-logo.svg"), "w") as f:
    f.write(RI_LOGO_SVG)

print("==> Rendering Scalable Icons to Multi-Resolution PNGs...")
for name, content in svg_map.items():
    svg_path = os.path.join(ICONS_SCALABLE_APPS, name)
    base_name = name.replace(".svg", ".png")
    for sz in SIZES:
        png_path = os.path.join(CHROOT, f"usr/share/icons/hicolor/{sz}x{sz}/apps/{base_name}")
        cmd = ["rsvg-convert", "-w", str(sz), "-h", str(sz), svg_path, "-o", png_path]
        subprocess.run(cmd, check=True)
        if name in ["start-here.svg", "start-here-symbolic.svg", "distributor-logo.svg"]:
            p_place_png = os.path.join(CHROOT, f"usr/share/icons/hicolor/{sz}x{sz}/places/{base_name}")
            subprocess.run(["rsvg-convert", "-w", str(sz), "-h", str(sz), svg_path, "-o", p_place_png], check=True)

# 512px renders for pixmaps, plymouth, calamares
subprocess.run(["rsvg-convert", "-w", "512", "-h", "512", os.path.join(ICONS_SCALABLE_APPS, "ri-logo.svg"), "-o", os.path.join(PIXMAPS, "ri-logo.png")], check=True)
subprocess.run(["rsvg-convert", "-w", "512", "-h", "512", os.path.join(ICONS_SCALABLE_APPS, "distributor-logo.svg"), "-o", os.path.join(PIXMAPS, "distributor-logo.png")], check=True)
subprocess.run(["rsvg-convert", "-w", "512", "-h", "512", os.path.join(ICONS_SCALABLE_APPS, "ri-logo.svg"), "-o", os.path.join(PLYMOUTH, "logo.png")], check=True)
subprocess.run(["rsvg-convert", "-w", "512", "-h", "512", os.path.join(ICONS_SCALABLE_APPS, "ri-logo.svg"), "-o", os.path.join(ASSETS, "ri-logo.png")], check=True)
subprocess.run(["rsvg-convert", "-w", "512", "-h", "512", os.path.join(ICONS_SCALABLE_APPS, "distributor-logo.svg"), "-o", os.path.join(ASSETS, "distributor-logo.png")], check=True)
subprocess.run(["rsvg-convert", "-w", "512", "-h", "512", os.path.join(ICONS_SCALABLE_APPS, "ri-installer.svg"), "-o", os.path.join(CALAMARES_BRANDING, "squid.png")], check=True)

# Generate /usr/share/icons/hicolor/index.theme
HICOLOR_THEME = '''[Icon Theme]
Name=Hicolor
Comment=Default RiOS Fallback Icon Theme
Hidden=true
Directories=16x16/apps,16x16/places,24x24/apps,24x24/places,32x32/apps,32x32/places,48x48/apps,48x48/places,64x64/apps,64x64/places,128x128/apps,128x128/places,256x256/apps,256x256/places,512x512/apps,512x512/places,scalable/apps,scalable/places

[16x16/apps]
Size=16
Type=Threshold
Context=Applications

[16x16/places]
Size=16
Type=Threshold
Context=Places

[24x24/apps]
Size=24
Type=Threshold
Context=Applications

[24x24/places]
Size=24
Type=Threshold
Context=Places

[32x32/apps]
Size=32
Type=Threshold
Context=Applications

[32x32/places]
Size=32
Type=Threshold
Context=Places

[48x48/apps]
Size=48
Type=Threshold
Context=Applications

[48x48/places]
Size=48
Type=Threshold
Context=Places

[64x64/apps]
Size=64
Type=Threshold
Context=Applications

[64x64/places]
Size=64
Type=Threshold
Context=Places

[128x128/apps]
Size=128
Type=Threshold
Context=Applications

[128x128/places]
Size=128
Type=Threshold
Context=Places

[256x256/apps]
Size=256
Type=Threshold
Context=Applications

[256x256/places]
Size=256
Type=Threshold
Context=Places

[512x512/apps]
Size=512
Type=Threshold
Context=Applications

[512x512/places]
Size=512
Type=Threshold
Context=Places

[scalable/apps]
Size=512
MinSize=16
MaxSize=512
Type=Scalable
Context=Applications

[scalable/places]
Size=512
MinSize=16
MaxSize=512
Type=Scalable
Context=Places
'''

with open(os.path.join(CHROOT, "usr/share/icons/hicolor/index.theme"), "w") as f:
    f.write(HICOLOR_THEME)

# Create Desktop Entries in /usr/share/applications/
DESKTOPS = {
    "ri-installer.desktop": '''[Desktop Entry]
Type=Application
Version=1.0
Name=Install RiOS to Disk
GenericName=System Installer
Comment=Install RiOS permanently to HDD, SSD, or NVMe drive
Exec=x-terminal-emulator -e sudo /usr/local/bin/ri-installer
Icon=ri-installer
Terminal=true
Categories=System;Settings;
StartupNotify=true
''',
    "calamares.desktop": '''[Desktop Entry]
Type=Application
Version=1.0
Name=Install RiOS (Calamares)
GenericName=System Installer
Comment=Install RiOS with graphical Calamares installer
Exec=calamares
Icon=ri-installer
Terminal=false
Categories=System;Settings;
StartupNotify=true
''',
    "ri-cyber.desktop": '''[Desktop Entry]
Type=Application
Version=1.0
Name=RiOS Cyber Hub
GenericName=Security Hub
Comment=Launch RiOS offensive cybersecurity and penetration testing suite
Exec=x-terminal-emulator -e /usr/local/bin/ri-cyber
Icon=ri-cyber
Terminal=true
Categories=System;Security;Network;
StartupNotify=true
''',
    "ri-dev.desktop": '''[Desktop Entry]
Type=Application
Version=1.0
Name=RiOS Dev Hub
GenericName=Developer Hub
Comment=Project generator, toolchain diagnostics, and development manager
Exec=x-terminal-emulator -e /usr/local/bin/ri-dev
Icon=ri-dev
Terminal=true
Categories=Development;System;
StartupNotify=true
''',
    "ri-server.desktop": '''[Desktop Entry]
Type=Application
Version=1.0
Name=RiOS Server Cockpit
GenericName=Server Web Console
Comment=Open Cockpit Web Console and server services manager
Exec=xdg-open http://localhost:9090
Icon=ri-server
Terminal=false
Categories=System;Network;
StartupNotify=true
''',
    "ri-doctor.desktop": '''[Desktop Entry]
Type=Application
Version=1.0
Name=RiOS System Doctor
GenericName=System Diagnostics
Comment=Run hardware, kernel, network, and package verification diagnostics
Exec=x-terminal-emulator -e /usr/local/bin/ri-doctor check
Icon=ri-doctor
Terminal=true
Categories=System;Settings;
StartupNotify=true
''',
    "ri-gpu-setup.desktop": '''[Desktop Entry]
Type=Application
Version=1.0
Name=RiOS GPU Setup
GenericName=Graphics Setup
Comment=Configure NVIDIA, AMD, Intel drivers and PRIME offloading
Exec=x-terminal-emulator -e /usr/local/bin/ri-gpu-setup
Icon=ri-gpu-setup
Terminal=true
Categories=Settings;HardwareSettings;
StartupNotify=true
''',
    "ri-harden.desktop": '''[Desktop Entry]
Type=Application
Version=1.0
Name=RiOS Security Hardening
GenericName=Security Baseline
Comment=Apply kernel sysctl hardening, zero-trust UFW firewall, and SSH safety
Exec=x-terminal-emulator -e /usr/local/bin/ri-harden
Icon=ri-harden
Terminal=true
Categories=System;Security;
StartupNotify=true
''',
    "ri-cve.desktop": '''[Desktop Entry]
Type=Application
Version=1.0
Name=RiOS CVE Auditor
GenericName=Vulnerability Auditor
Comment=Scan system for known CVE vulnerabilities and verify CPU mitigations
Exec=x-terminal-emulator -e /usr/local/bin/ri-cve audit
Icon=ri-cve
Terminal=true
Categories=System;Security;
StartupNotify=true
''',
    "ri-chroot.desktop": '''[Desktop Entry]
Type=Application
Version=1.0
Name=RiOS Rescue Chroot
GenericName=System Rescue
Comment=Emergency disk auto-mount and rescue environment
Exec=x-terminal-emulator -e sudo /usr/local/bin/ri-chroot
Icon=ri-chroot
Terminal=true
Categories=System;
StartupNotify=true
''',
}

print("==> Creating Desktop Entries in /usr/share/applications/...")
for fname, content in DESKTOPS.items():
    with open(os.path.join(APPLICATIONS, fname), "w") as f:
        f.write(content)

# Update bootloader splashes with new official logo
print("==> Updating Bootloader Splash SVGs and PNGs...")
with open(os.path.join(PIXMAPS, "ri-logo.png"), "rb") as f:
    b64_logo = base64.b64encode(f.read()).decode("utf-8")

SPLASH_SVG = f'''<?xml version="1.0" encoding="UTF-8"?>
<svg width="640" height="480" viewBox="0 0 640 480" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <defs>
    <linearGradient id="bgGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#080e1e"/>
      <stop offset="50%" stop-color="#0f172a"/>
      <stop offset="100%" stop-color="#1e1b4b"/>
    </linearGradient>
    <linearGradient id="glassGrad" x1="0%" y1="0%" x2="0%" y2="100%">
      <stop offset="0%" stop-color="#ffffff" stop-opacity="0.12"/>
      <stop offset="100%" stop-color="#ffffff" stop-opacity="0.02"/>
    </linearGradient>
  </defs>

  <!-- Background -->
  <rect width="640" height="480" fill="url(#bgGrad)"/>
  <circle cx="100" cy="70" r="140" fill="#00BBF9" opacity="0.12"/>
  <circle cx="540" cy="420" r="160" fill="#6366F1" opacity="0.10"/>

  <!-- Header Card Glass -->
  <rect x="30" y="20" width="580" height="96" rx="16" fill="url(#glassGrad)" stroke="#ffffff" stroke-opacity="0.15" stroke-width="1"/>

  <!-- Official Logo Badge -->
  <g transform="translate(46, 30)">
    <image width="76" height="76" xlink:href="data:image/png;base64,{b64_logo}"/>
  </g>

  <!-- OS Name & Edition -->
  <text x="136" y="58" font-family="DejaVu Sans, Inter, sans-serif" font-size="24" font-weight="bold" fill="#ffffff">RiOS 1.0.0</text>
  <text x="136" y="80" font-family="DejaVu Sans, Inter, sans-serif" font-size="13" font-weight="600" fill="#38BDF8">GNOME • KDE Plasma • Hyprland • Server &amp; Cyber Suite</text>
  <text x="136" y="98" font-family="DejaVu Sans, Inter, sans-serif" font-size="11" fill="#94A3B8">Debian 12 Bookworm • Fast Live &amp; Real Hardware Boot Architecture</text>
</svg>
'''

splashes = [
    "rios-live/config/bootloaders/grub-pc/splash",
    "rios-live/config/bootloaders/grub-pc/live-theme/splash",
    "rios-live/config/bootloaders/isolinux/splash",
    "rios-live/config/bootloaders/syslinux/splash",
    "rios-live/config/bootloaders/syslinux_common/splash",
]

for sp in splashes:
    svg_p = os.path.join(ROOT, sp + ".svg")
    png_p = os.path.join(ROOT, sp + ".png")
    os.makedirs(os.path.dirname(svg_p), exist_ok=True)
    with open(svg_p, "w") as f:
        f.write(SPLASH_SVG)
    subprocess.run(["rsvg-convert", "-w", "640", "-h", "480", svg_p, "-o", png_p], check=True)

# Add live hook for icon cache update
HOOK_PATH = os.path.join(ROOT, "rios-live/config/hooks/live/0120-icon-cache.hook.chroot")
with open(HOOK_PATH, "w") as f:
    f.write('''#!/bin/sh
set -e
echo "==> Updating hicolor icon cache for SVG & PNG icons..."
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -f -t /usr/share/icons/hicolor || true
fi
''')
os.chmod(HOOK_PATH, 0o755)

print("==> ALL SVG vector icons, system logos, desktop files, and splashes successfully generated!")
