# RatanaOS AppArmor Baseline

RatanaOS uses AppArmor as the mandatory access control framework for userland confinement.

## Policy

- Enable the LSM at boot with `lsm=landlock,lockdown,yama,integrity,apparmor,bpf`.
- Set `apparmor=1 security=apparmor` on all supported kernels.
- Load profiles during early boot through the distro AppArmor service unit.
- Default new first-party applications to enforce mode before release.
- Keep installer and updater components confined even in pre-alpha images.

## Initial Profiles

- `profiles/usr.bin.ratana-installer`
- `profiles/usr.libexec.ratana-update-agent`

## Gaps

- No first-party binaries exist in the repository yet.
- Profile coverage must expand once application entrypoints are implemented.

