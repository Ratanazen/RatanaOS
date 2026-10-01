# RiOS — Build & Virtualization Guide (1.0.0)

RiOS is an enterprise Debian 12 Bookworm hybrid operating system tailored for **Server Infrastructure**, **Cybersecurity & Pen-Testing**, **Software Development**, and **Modern Wayland / Desktop Computing**.

---

## 1. Quick Build Commands

### A. Full Edition (All Suites Included)
Builds the complete hybrid ISO with Server, Cyber, Developer tools, GNOME, KDE, Hyprland, and Sway:
```bash
./build.sh all
# or simply:
./build.sh
```

### B. Dedicated Server Edition
Builds a streamlined server image with remote management, Docker, Nginx, UFW, Fail2ban, WireGuard, and Cockpit:
```bash
./build.sh server
```

### C. Cybersecurity Edition
Builds a pen-testing and security auditing image:
```bash
./build.sh cyber
```

### D. Developer Workstation
Builds a workstation with development runtimes (Rust, Go, Python, Node), compilers, Docker, and Wayland:
```bash
./build.sh dev
```

### E. Custom Modular Build
Mix and match package suites to create a tailored ISO:
```bash
# Example: Server + Cyber only, no heavy desktops
./build.sh custom --enable-server --enable-cyber --enable-wayland --disable-gnome --disable-kde

# View currently active profiles:
./build.sh status

# Clean build artifacts:
./build.sh clean
```

---

## 2. Running RiOS in Server Mode

RiOS includes a dedicated virtual server launcher in `scripts/run_server.sh` with KVM acceleration and automated port forwards:

```bash
# Launch virtual server from Live ISO in terminal console:
./scripts/run_server.sh --curses

# Launch in background / headless mode:
./scripts/run_server.sh --headless

# Launch with graphical window:
./scripts/run_server.sh --gui

# Boot from installed virtual disk:
./scripts/run_server.sh --disk /tmp/rios-target-disk.qcow2
```

### Active Server Port Mappings
| Service | Host Port | Guest Port | Connection Command |
| :--- | :--- | :--- | :--- |
| **SSH** | `2222` | `22` | `ssh -p 2222 ri@localhost` |
| **HTTP (Nginx/Apache)** | `8080` | `80` | `http://localhost:8080` |
| **HTTPS** | `8443` | `443` | `https://localhost:8443` |
| **Cockpit Web Admin** | `9090` | `9090` | `http://localhost:9090` |

---

## 3. Server Management CLI (`ri-server`)

On installed RiOS systems or Live sessions, manage server services via `ri-server`:

```bash
# Check status of all managed daemons (SSH, Nginx, Docker, UFW, Wireguard, Cockpit):
ri-server status

# View server load, RAM, disk, and network interfaces:
ri-server info

# View active listening ports:
ri-server ports

# Service lifecycle:
ri-server start nginx
ri-server stop apache2
ri-server restart docker
ri-server enable ssh

# Manage firewall:
ri-server firewall status
ri-server firewall allow 8080
ri-server firewall enable

# Launch Cockpit Web Dashboard:
ri-server cockpit start
```

---

## 4. Testing & Validation

Run the automated test suite before building:
```bash
# Run all 15 static tests:
./tests/run-all.sh

# Run unattended QEMU end-to-end installation test:
./tests/qemu/install-test.sh
```
