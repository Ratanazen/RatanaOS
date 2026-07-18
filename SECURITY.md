# RatanaOS Security Policy

## Supported Versions

| Version | Support Status |
|---|---|
| v5.0.x "Phoenix" | ✅ Active — security patches + feature updates |
| v4.0.x | ⚠️ Security patches only until v5.0 LTS designation |
| v3.0.x and earlier | ❌ End of Life — upgrade strongly recommended |

---

## Reporting a Vulnerability

**Please do NOT open a public GitHub issue for security vulnerabilities.**

To report a security vulnerability responsibly:

1. **Email:** `security@ratanaos.local` *(replace with real address before publishing)*
2. **Subject line:** `[SECURITY] Brief description`
3. **Include:**
   - Description of the vulnerability
   - Steps to reproduce
   - Potential impact assessment
   - Your suggested fix (if any)

We aim to respond within **48 hours** and issue a patch within **14 days** for critical CVEs.

---

## Security Architecture

- **AppArmor** — Mandatory Access Control profiles shipped for all network-exposed services
- **UFW** — Default-deny ingress policy, allow-outgoing default
- **fail2ban** — SSH, HTTP, and custom jail configs (v6.0+: IDS alerts in Ratana Firewall)
- **Auditd** — System call tracking, compatible with `aureport`
- **LUKS2** — Full-disk encryption available during installation
- **Secure Boot** — `shim-signed` UEFI chain of trust
- **Automatic security patches** — Separate patch channel from feature updates

---

## Known Open Issues

*None at time of writing. See CHANGELOG.md for resolved CVEs.*

---

## Responsible Disclosure Timeline

| Day | Action |
|---|---|
| 0 | Vulnerability received, acknowledgement sent |
| 3 | Initial triage and severity assessment |
| 14 | Patch developed and tested |
| 21 | Patch released (or coordinated disclosure if vendor coordination needed) |
| 90 | Full public disclosure if not yet patched |
