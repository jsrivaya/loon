.DEFAULT_GOAL := help

# SHELL := /bin/bash

################################### Conan
CONAN_PROFILE ?= profiles/macos-clang14

.PHONY: conan-setup
conan-setup: ## setup conan with conancenter remote
	conan remote add conancenter https://center.conan.io --force

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
clean: ## clean all build files
	@rm -rf build

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

CLANG_FORMAT_CMD = find . -iname "*.hpp" -o -iname "*.cpp" | xargs clang-format -style=file -i
.PHONY: check-format
check-format: ## Check clang format errors
	$(CLANG_FORMAT_CMD) --dry-run -Werror

format: ## Apply clang format to code
	$(CLANG_FORMAT_CMD)

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
			 --suppressions-list=config/cppcheck-suppressions.txt \
			 --checkers-report=cppcheck_report.txt \
			 ./src

################################### Other targets

help: ## Show this help message
	@grep '##' $(MAKEFILE_LIST) | grep -v grep | awk 'BEGIN {FS = ": .*##"}; {printf "\033[36m%-38s\033[0m %s\n", $$1, $$2}'
