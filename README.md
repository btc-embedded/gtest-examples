# Example data for googletest-based C/C++ testing with BTC EmbeddedTester
This is an example project that demonstrates use GoogleTest for unit testing in C/C++ projects with BTC EmbeddedTester:
- ✅ Full requirements traceability
- ✅ Code coverage incl. MC/DC
- ✅ Robustness and seamless automation
- ✅ Certification for automotive standards (incl. ISO 26262)


## Directory Structure
- `examples/`: Root directory for example projects.
    - `seat-heater-c/`:
        - `src/`: The source code files of the seat heating controller example with public headers in the `includes/` subdirectory.
        - `test/`: The GoogleTest unit test files for the seat heating controller.
    - `seat-heater-cpp/`:
        - `src/`: The source code files of the seat heating controller example (C++ implementation) with public headers in the `includes/` subdirectory.
        - `test/`: The GoogleTest unit test files for the seat heating controller.
- `CMakeLists.txt`: The CMake build configuration file.
- `.vscode/`: Visual Studio Code configuration files.
- `.btcep/`: BTC EmbeddedTester metadata and configuration files.


## Prerequisites
- BTC EmbeddedTester installed and licensed
- Visual Studio Code + C/C++ extensions
- BTC EmbeddedTester extension for Visual Studio Code
- C/C++ compiler

