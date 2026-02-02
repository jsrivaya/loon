# Contributing to loon

Thank you for your interest in contributing to loon! This document provides guidelines and instructions for contributing.

## Getting Started

### Prerequisites

- C++23 compatible compiler (GCC 13+, Clang 14+)
- CMake 3.20+
- Conan 2.x
- clang-format (for code formatting)

### Development Setup

1. Fork and clone the repository:
   ```bash
   git clone https://github.com/YOUR_USERNAME/loon.git
   cd loon
   ```

2. Set up Conan:
   ```bash
   make conan-setup
   ```

3. Install dependencies and build:
   ```bash
   make deps
   make build
   ```

4. Run tests to verify everything works:
   ```bash
   make build  # Tests run automatically
   ```

## How to Contribute

### Reporting Issues

- Check existing issues to avoid duplicates
- Use a clear, descriptive title
- Include steps to reproduce for bugs
- For feature requests, explain the use case and expected behavior

### Submitting Changes

1. Create a feature branch from `main`:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. Make your changes following the coding standards below

3. Ensure all tests pass:
   ```bash
   make build
   ```

4. Check code formatting:
   ```bash
   make check-format
   ```

5. Commit your changes with a clear message:
   ```bash
   git commit -m "Add feature: brief description"
   ```

6. Push and create a pull request

### Pull Request Guidelines

- Keep PRs focused on a single change
- Update documentation if needed
- Add tests for new functionality
- Ensure CI passes before requesting review
- Link related issues in the PR description

## Coding Standards

### Style

- Follow the existing code style
- Use clang-format for formatting (run `make format`)
- Use meaningful variable and function names
- Keep functions small and focused

### C++ Guidelines

- Use modern C++ features (C++23)
- Prefer `std::optional` over null pointers
- Use `const` and `constexpr` where appropriate
- Avoid raw pointers; use smart pointers or references
- Header-only: all code goes in `.hpp` files under `include/loon/`

### Performance

This is a performance-focused library. Please consider:

- Avoid unnecessary allocations
- Minimize cache misses
- Use move semantics where appropriate
- Document time/space complexity for public APIs

### Testing

- Write tests for all new functionality
- Place tests in `test/test_<component>.cpp`
- Use Google Test framework
- Aim for high coverage of edge cases

Example test structure:
```cpp
#include <gtest/gtest.h>
#include <loon/your_header.hpp>

TEST(YourClass, DescriptiveTestName) {
    // Arrange
    // Act
    // Assert
}
```

## Project Structure

```
loon/
├── include/loon/     # Header-only library sources
├── test/             # Unit tests
├── profiles/         # Conan profiles
├── test_package/     # Conan package test
└── .github/          # CI workflows
```

## Questions?

Feel free to open an issue for any questions about contributing.

## License

By contributing to loon, you agree that your contributions will be licensed under the MIT License.
