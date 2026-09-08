# Built-in Packages

RatanaOS now has a small package-manager foundation for built-in applications.
It is not compatible with Arch `pacman`, Debian `apt`, `.deb`, APK, Flatpak, or
Linux binaries. Packages are compiled into the kernel and their installed state
exists only for the current boot session.

Available commands:

```text
pkg list
pkg install telegram
pkg run telegram
pkg remove telegram

pacman -S telegram
apt install telegram
```

`telegram` launches a native local-only Telegram Demo window. It has no
network connection, authentication, contacts, encryption transport, or message
persistence. A real installer requires writable storage, a filesystem,
networking/TLS, an ELF executable loader, process isolation, and package
signature verification.
