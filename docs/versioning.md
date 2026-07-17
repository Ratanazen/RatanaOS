# Versioning and releases

RatanaOS uses semantic versions (`MAJOR.MINOR.PATCH`). Development versions may
carry a suffix such as `-dev` or `-rc.1`. `VERSION`, the Git tag, ISO filename,
and release title are the same version; release tags have a `v` prefix.

- `MAJOR`: an intentionally incompatible platform or upgrade-policy change.
- `MINOR`: a new RatanaOS release or substantial desktop feature set.
- `PATCH`: compatible security, reliability, packaging, and artwork corrections.

Debian base and repository snapshot metadata will be recorded separately from
the product version, because RatanaOS patch releases need not imply a Debian
point-release change. To publish, remove the development suffix, commit, test a
clean build, and push the matching tag (for example `v0.1.0`). The tag workflow
rejects a mismatch before building and attaches the ISO and checksum to a GitHub
release. Production releases will later add OpenPGP signatures and provenance.

