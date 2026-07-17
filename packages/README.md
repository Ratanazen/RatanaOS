# Packages

`lists/` contains live-build package lists. Future RatanaOS Debian source
packages belong in one directory per source package and must be built with
`dpkg-buildpackage`/`sbuild`, linted with `lintian`, and published only through a
signed APT repository.

