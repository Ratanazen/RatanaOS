# RatanaOS Cyber Edition Security Guide

## Overview
RatanaOS Cyber Edition ships with an AppArmor-enforced kernel and a default-deny Uncomplicated Firewall (UFW) configuration. It is designed for penetration testers, security analysts, and digital forensics investigators.

## Tool Categories
During installation, you can select which tool suites to install:
- **Network Analysis**: Nmap, Wireshark
- **Web Application Testing**: Burp Suite Community
- **Password Auditing**: Hashcat
- **Malware Analysis**: Ghidra, Volatility3

## Hardening
- **AppArmor**: Profiles are stored in `/etc/apparmor.d/`. By default, RatanaOS ships with profiles in `enforce` mode for exposed network services.
- **Auditd**: System calls are tracked. Use `aureport` to review anomalies.
- **Secure Boot**: The Live ISO and installed system are signed with `shim-signed` for UEFI secure boot compliance.
