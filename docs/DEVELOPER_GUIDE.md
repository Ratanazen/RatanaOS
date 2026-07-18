# RatanaOS Developer Guide

## Development Environment
RatanaOS Developer (and Cyber Edition) comes pre-configured with a robust set of compilers and runtimes.

### Pre-installed Languages
- **C/C++**: GCC, Clang, CMake
- **Python**: Python 3.11+
- **Rust**: `rustc` and `cargo`
- **Go**: Golang 1.20+
- **Java**: OpenJDK Default
- **Node.js**: Node package manager and runtime

### Containerization
- `docker.io` is installed. 
- Ensure your user is added to the `docker` group: `sudo usermod -aG docker $USER`

### Building RatanaOS
RatanaOS itself is built using CMake and Qt6. To build the desktop environment:
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```
