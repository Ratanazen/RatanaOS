# RatanaOS Permission Management Baseline

RatanaOS follows least-privilege defaults for local users, services, and mutable system state.

## Rules

- No first-party service runs as root without a documented reason.
- Set `UMASK 027` in system-wide login defaults.
- Keep privileged groups minimal: `sudo`, `adm`, `systemd-journal`, and hardware access groups only when needed.
- Writable state for first-party services lives under `/var/lib/<service>` with service ownership.
- Runtime state lives under `/run/<service>` and is recreated on boot.
- Log directories use service ownership and are not world-readable unless required for diagnostics.

## Required Follow-Up

- Add service users in package definitions when services are introduced.
- Review Polkit actions before shipping any GUI admin tool.
- Add package-specific file ownership manifests before beta.

