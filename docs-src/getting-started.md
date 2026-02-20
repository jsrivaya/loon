# Getting Started

## Installation

### Using Conan (recommended)

Add to your `conanfile.txt`:

```ini
[requires]
loon/0.1.0
```

Or `conanfile.py`:

```python
def requirements(self):
    self.requires("loon/0.1.0")
```

### Header-only (manual)

Copy the `include/loon` directory to your project and add it to your include path.

## CMake Integration

```cmake
find_package(loon REQUIRED)
target_link_libraries(your_target PRIVATE loon::loon)
```

## Requirements

- C++23 compatible compiler (GCC 13+, Clang 14+)
- CMake 3.20+
- Conan 2.x (optional, for package management)

## Building from Source

```bash
# Setup Conan (one-time)
make conan-setup

# Install dependencies
make deps

# Build and run tests
make build

# Create and test Conan package
make package
```

## Other Build Targets

```bash
make help              # Show all targets
make coverage          # Generate coverage report
make check-format      # Check code formatting
make format            # Apply clang-format
make clean             # Clean build files
make bench             # Run benchmarks
```
