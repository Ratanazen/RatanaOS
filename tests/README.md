# RatanaOS QA Tests

`run_release_checks.sh` performs the initial release gate for this bootstrap repository.

It verifies whether the repository contains enough implementation and artifacts to execute:

- build validation
- ISO validation
- boot testing
- installation testing
- performance testing
- application smoke testing
- security validation

The script also writes a dated release report to `reports/`.
