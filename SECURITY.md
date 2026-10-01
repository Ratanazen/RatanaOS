# RiOS — Security Architecture & Penetration Testing Guide (1.0.0)

RiOS is an enterprise Debian 12 Bookworm operating system engineered for **Offensive Cybersecurity**, **Penetration Testing**, **Digital Forensics**, and **Hardened Server Operations**.

---

## 1. Built-in Security & Kali Linux Suite

RiOS includes over **75+ verified offensive & defensive security utilities** pre-configured out-of-the-box:

### A. Network Discovery, Reconnaissance & Port Scanning
- **Nmap**: Multi-paradigm network scanner and script auditing engine
- **Masscan**: Asynchronous raw SYN packet port scanner (fastest internet scanner)
- **Netdiscover**: Active/passive ARP reconnaissance for local area networks
- **Arp-scan & Arping**: Layer-2 host discovery and ARP pinging
- **Fping & Hping3**: High-performance ICMP ping sweeping and TCP/IP packet crafting
- **Dnstracer**: Recursive DNS trace and name server zone verification

### B. Traffic Interception & Packet Analysis
- **Wireshark**: Leading graphical network protocol analyzer
- **TShark**: CLI network capture and protocol analyzer
- **Tcpdump & Tcpflow**: Packet capture and complete TCP stream reassembly
- **Ngrep**: Network packet pattern matching and regular expression search

### C. Wireless, Wi-Fi & RF Security (Iconic Kali Tools)
- **Aircrack-ng**: Complete 802.11 WEP and WPA/WPA2-PSK key cracking suite
- **Wifite**: Automated wireless auditor for WPS, WPA, and PMKID handshakes
- **Bully & Pixiewps**: High-efficiency WPS brute force and offline pixie-dust attack
- **Macchanger**: Hardware MAC address spoofing and randomization

### D. Password Auditing & Cryptanalysis
- **Hashcat**: World's fastest GPU/CPU-accelerated password recovery engine
- **John the Ripper**: Customizable multi-format offline password cracker
- **Hydra & Medusa**: High-speed parallelized network login brute-forcers (SSH, FTP, HTTP, etc.)
- **CeWL**: Custom wordlist generator spidered from target websites
- **Crunch**: Dynamic wordlist and permutation generator
- **Fcrackzip & Pdfcrack**: Dedicated ZIP and PDF password recovery tools

### E. Web Application Security & OSINT
- **SQLMap**: Automated SQL injection detection and database takeover
- **Gobuster & Dirb**: High-speed URI and DNS subdomain brute-forcers
- **Wfuzz**: Web application fuzzer for hidden parameters and endpoints
- **Sublist3r**: Subdomain discovery engine using OSINT sources
- **Recon-ng**: Full-featured web reconnaissance and intelligence gathering framework
- **SSLScan & Testssl.sh**: Comprehensive TLS/SSL cipher, protocol, and vulnerability auditors

### F. Digital Forensics & Reverse Engineering
- **The Sleuth Kit (TSK)**: Digital forensics file system investigation library and tools
- **Binwalk**: Firmware analysis and automated file extraction
- **Scalpel & Foremost**: High-speed forensic file carvers
- **GNU Ddrescue**: Data recovery for corrupted or damaged block devices
- **Yara**: Multi-platform malware classification and pattern identification
- **GDB, Strace, Ltrace, Hexedit**: Low-level binary debugging and syscall tracing

### G. Protocol Exploitation, Pivoting & MITM
- **Bettercap**: Complete, extensible modular MITM framework
- **Ettercap**: Graphical and CLI man-in-the-middle ARP/DNS spoofer
- **Python3-Scapy**: Interactive packet manipulation and protocol spoofing library
- **Python3-Impacket**: Low-level network protocol exploitation suite (NTLM, Kerberos, SMB)
- **Smbmap & Smbclient**: SMB share enumeration and permission auditing
- **Onesixtyone**: Fast SNMP community scanner
- **Ike-scan**: IPsec VPN endpoint identification and fingerprinting
- **Proxychains4**: Dynamic SOCKS proxy and Tor traffic redirection engine

---

## 2. Cybersecurity Control Center (`ri-cyber`)

Use the interactive command-line hub to inspect, launch, and automate security tasks:

```bash
# Audit installed security tools and status:
ri-cyber status

# Multi-stage network discovery & host sweep:
ri-cyber scan 192.168.1.0/24

# Fast port and service inspection:
ri-cyber ports 192.168.1.100

# Audit web server headers and technologies:
ri-cyber web http://target.local

# Audit TLS/SSL security and certificates:
ri-cyber ssl example.com

# Enumerate subdomains via OSINT:
ri-cyber osint example.com

# Inspect Wi-Fi adapters and wireless tools:
ri-cyber wireless

# Launch Bettercap MITM session:
ri-cyber mitm eth0
```

---

## 3. Kali Rolling APT Integration (`ri-kali-setup`)

Need access to the complete 600+ Kali Linux package catalog? RiOS includes a safe APT-pinning bridge:

```bash
# Enable Kali Rolling repositories with low-priority pinning (Priority 100):
sudo ri-kali-setup enable

# Install any package directly from Kali without breaking Debian Bookworm base:
sudo ri-kali-setup install metasploit-framework
sudo ri-kali-setup install burpsuite
sudo ri-kali-setup install exploitdb

# Check integration status:
ri-kali-setup status

# Disable Kali repository:
sudo ri-kali-setup disable
```

---

## 4. Defensive Hardening Baseline

RiOS maintains an enterprise defense posture:
- **UFW & Nftables**: Stateful packet filtering enabled by default.
- **Fail2ban**: Automated IP banning for brute-force attacks against SSH/Web.
- **Suricata**: Next-generation Intrusion Detection (IDS) and Prevention (IPS) engine.
- **Rootless Containers**: Podman and Docker isolation for running services.
- **Audit & Rootkit Scanners**: Pre-installed `lynis`, `chkrootkit`, and `rkhunter`.
