# RatanaOS Development Plan (v13.0)

## Current State
- Version: `13.0` (in development)
- Last Release: `v12.0` (Universal Live USB & GUI Installer)
- Codebase: Qt6 UI framework, Bash build scripts, Debian base.

## Milestone: v13.0 Foundation
The next major milestone focuses on stabilizing the v12.0 installer features, resolving any technical debt, and preparing the architecture for the v13.0 feature set. 

### Roles & Responsibilities Workflow
This plan adheres to the **RatanaOS Autonomous Engineering Agent v1.0** workflow.
1. **Planner**: Set milestones and update TASKS.md.
2. **Researcher**: Investigate packaging and OS features.
3. **Coding / Build / UI / System Engineer**: Implement code.
4. **QA Engineer**: Validate all changes via tests.
5. **Documentation / Release Engineer**: Maintain docs and release artifacts.

### Objectives for Current Cycle
1. Review all recent v12.0 changes for regression.
2. Address any pending issues or technical debt in the build scripts (`build-iso.sh`, `build-live-usb.sh`).
3. Prepare the Qt6 `apps` directory for new v13.0 features.
4. Update `CHANGELOG.md` and `RELEASE_NOTES.md` for the v13.0 cycle.
