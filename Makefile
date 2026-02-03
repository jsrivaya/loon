.DEFAULT_GOAL := help

# SHELL := /bin/bash

################################### Conan
CONAN_PROFILE ?= profiles/macos-clang14

.PHONY: conan-setup
conan-setup: ## setup conan with conancenter remote
	conan remote add conancenter https://center2.conan.io --force

.PHONY: deps
deps: ## install conan dependencies
	conan install . --output-folder=build --build=missing --profile=$(CONAN_PROFILE)

################################### Build and Test

all: build

.PHONY: build
build: ## build library and run tests
	conan build . --build=missing --profile=$(CONAN_PROFILE)

.PHONY: package
package: ## create conan package and run test_package
	conan create . --build=missing --profile=$(CONAN_PROFILE)

.PHONY: clean
clean: ## clean all build and generated files
	@rm -rf build
	@rm -rf coverage
	@rm -rf docs
	@rm -rf cppcheck-cache
	@rm -f cppcheck_report.txt
	@find . -name "*.gcda" -delete
	@find . -name "*.gcno" -delete

################################### Coverage
COVERAGE_PROFILE ?= profiles/macos-clang14-coverage
COVERAGE_DIR ?= coverage

.PHONY: coverage
coverage: ## build with coverage and generate HTML report
	conan build . --build=missing --profile=$(COVERAGE_PROFILE)
	@mkdir -p $(COVERAGE_DIR)
	gcovr --root . \
		  --filter include/ \
		  --exclude-unreachable-branches \
		  --html-details $(COVERAGE_DIR)/index.html
	@echo "Coverage report: $(COVERAGE_DIR)/index.html"

.PHONY: coverage-clean
coverage-clean: ## clean coverage data
	@rm -rf $(COVERAGE_DIR)
	@find . -name "*.gcda" -delete
	@find . -name "*.gcno" -delete

################################### Static Analysis
CPPCHECK_CACHE_DIR ?= cppcheck-cache

SOURCES := $(shell find include test test_package -iname "*.hpp" -o -iname "*.cpp")

.PHONY: check-format
check-format: ## Check clang format errors
	clang-format --style=file --dry-run -Werror $(SOURCES)

.PHONY: format
format: ## Apply clang format to code
	clang-format --style=file -i $(SOURCES)

.PHONY: check
check: ## Run static analysis checks using cppcheck
	@mkdir -p $(CPPCHECK_CACHE_DIR)
	@cppcheck --version
	cppcheck --quiet \
			 --enable=all \
			 --std=c++23 \
			 --cppcheck-build-dir=$(CPPCHECK_CACHE_DIR) \
			 --inline-suppr \
			 --check-level=exhaustive \
			 --error-exitcode=1 \
			 --checkers-report=cppcheck_report.txt \
			 --language=c++ \
			 --suppressions-list=config/cppcheck-suppressions.txt \
			 ./include/loon/*.hpp

.PHONY: tidy
tidy: ## Run clang-tidy static analysis
	@clang-tidy --version
	clang-tidy --warnings-as-errors=* ./include/loon/*.hpp -- -std=c++23

.PHONY: iwyu
iwyu: ## Run include-what-you-use analysis
	@include-what-you-use --version || echo "Install: brew install include-what-you-use"
	@for file in ./include/loon/*.hpp; do \
		echo "Checking $$file..."; \
		include-what-you-use -std=c++23 $$file 2>&1 || true; \
	done

.PHONY: check-all
check-all: check tidy ## Run all static analysis checks (cppcheck + clang-tidy)

################################### Documentation

.PHONY: docs
docs: ## Generate Doxygen documentation
	@doxygen --version || (echo "Install: brew install doxygen" && exit 1)
	doxygen Doxyfile
	@echo "Documentation: docs/html/index.html"

.PHONY: docs-clean
docs-clean: ## Clean generated documentation
	@rm -rf docs

################################### Other targets

help: ## Show this help message
	@grep '##' $(MAKEFILE_LIST) | grep -v grep | awk 'BEGIN {FS = ": .*##"}; {printf "\033[36m%-38s\033[0m %s\n", $$1, $$2}'
