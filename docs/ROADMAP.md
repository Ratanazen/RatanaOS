# RatanaOS Roadmap

Last updated: 2026-07-17

## Phase 0: Repository Bootstrap

- Initialize repository structure and governance docs
- Define architecture boundaries and release gates
- Assign team ownership by subsystem

## Phase 1: Base Distribution Bring-Up

- Select Debian stable base and packaging policy
- Establish package overlay and build reproducibility targets
- Define kernel baseline, firmware policy, and hardware support matrix
- Stand up automated image build pipeline

## Phase 2: Desktop Platform

- Integrate default desktop session, theming, and branding
- Build first-party control center, installer UX, and welcome flow
- Package essential end-user applications and system utilities

## Phase 3: Security and Hardening

- Define secure defaults, service exposure policy, and sandboxing posture
- Integrate package signing, secure update path, and image verification
- Add baseline compliance and vulnerability scanning

## Phase 4: AI and Research Features

- Evaluate local AI assistant architecture and hardware constraints
- Prototype privacy-preserving inference workflows
- Integrate research-backed automation into desktop and developer tooling

## Phase 5: QA and Release Engineering

- Automate installation, upgrade, hardware smoke, and regression tests
- Produce alpha, beta, and release candidate criteria
- Publish release checklist and support lifecycle policy

## 90-Day Priorities

- Deliver a bootable pre-alpha ISO with Debian base, branded desktop, and installer stub
- Establish CI for lint, package build, image build, and smoke tests
- Define release-blocking issues for kernel, build, security, desktop, research, and QA
