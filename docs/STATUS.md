# RatanaOS Status

Date: 2026-07-17
Branch: `docs/bootstrap-governance`
Overall state: Bootstrap in progress

## Current Snapshot

- Repository initialized locally
- Standard project directories created
- Core governance and planning docs drafted
- No code, packages, or build assets implemented yet
- No automated tests or CI configured yet

## Team Assignments

### Terminal 2: Kernel Developer

- Define kernel baseline target
- Create initial config strategy for desktop hardware support
- Document required out-of-tree drivers and upstreaming posture

### Terminal 3: Build & ISO Engineer

- Design Debian bootstrap and ISO pipeline
- Choose image build tooling and artifact structure
- Add reproducibility and caching strategy

### Terminal 4: Desktop Application Developer

- Propose desktop environment stack
- Define first-party control center and welcome app scope
- Start application framework selection

### Terminal 5: Security Engineer

- Draft secure defaults baseline
- Define package signing, repository trust, and update policy
- Plan hardening checks for services and installer output

### Terminal 6: Research & AI Engineer

- Evaluate offline-capable AI features appropriate for RatanaOS
- Define privacy and model distribution constraints
- Propose integration boundaries with desktop and system tools

### Terminal 7: QA & Release Engineer

- Design test matrix for install, boot, upgrade, and smoke checks
- Define milestone criteria for pre-alpha, alpha, beta, and RC
- Recommend CI stages and release reporting format

## Blockers

- No implementation branches created beyond bootstrap docs
- No toolchain or package policy selected
- No baseline release target defined

## Immediate Next Milestone

Project bootstrap complete when:

- Architecture and roadmap are accepted
- Each terminal has a first task branch
- Initial build skeleton and test skeleton exist
