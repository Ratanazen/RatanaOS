# RatanaOS Architecture

Last updated: 2026-07-17

## Principles

- Debian compatibility first
- Reproducible builds where practical
- Secure-by-default system configuration
- Modular ownership across kernel, build, desktop, security, research, and QA
- Clear separation between product code, packaging, and release tooling

## Top-Level Design

### Base Layer

- `kernel/`: RatanaOS kernel configs, patch queue, and compatibility notes
- `drivers/`: vendor and community driver integrations not suitable for upstream packaging alone
- `system/`: base filesystem defaults, services, policies, and operating system identity
- `packages/`: package manifests, patches, and overlay metadata

### Product Layer

- `desktop/`: shell integration, session defaults, theming, and UX policy
- `apps/`: first-party utilities and user-facing applications
- `installer/`: install workflow, partitioning policy, and post-install bootstrap
- `branding/`: logos, palettes, wallpapers, boot themes, and release naming

### Delivery Layer

- `build/`: image definitions, chroot/bootstrap recipes, and artifact metadata
- `scripts/`: developer utilities, linting, and local build orchestration
- `tests/`: automated validation, smoke tests, installer tests, and release gates
- `docs/`: architecture, roadmap, team status, release notes, and task tracking

## Branch Strategy

- `main`: protected integration branch
- `feature/*`: kernel, system, installer, or package development
- `build/*`: build pipeline and ISO work
- `desktop/*`: desktop UX, theming, and app integration
- `security/*`: hardening and trust-chain work
- `docs/*`: planning and documentation
- `qa/*`: test automation and release validation

## Release Gates

Each milestone requires explicit sign-off from:

- Kernel Developer
- Build & ISO Engineer
- Desktop Application Developer
- Security Engineer
- Research & AI Engineer
- QA & Release Engineer

Approval criteria:

- Stable under expected workload
- Secure by default or with documented exceptions
- Maintainable with clear ownership and automation
- Materially improves RatanaOS user or developer outcomes
