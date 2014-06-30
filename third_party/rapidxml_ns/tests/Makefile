SHELL=bash

PROJECTS= \
	test_allocation_1 \
	test_allocation_2 \
	test_alignment \
	test_comparative \
	test_interface \
	test_invalid \
	test_multimodule \
	test_no_exceptions \
	test_nodes \
	test_non_comparative \
	test_size \
	test_speed \
	test_wchar

export CDEFINES=TIXML_USE_STL

default:
	@echo Please specify a target to make:
	@echo "    build-g++-debug"
	@echo "    build-g++-release"
	@echo "    build-msvc-debug"
	@echo "    build-msvc-release"
	@echo "    run-g++-debug"
	@echo "    run-g++-release"
	@echo "    run-msvc-debug"
	@echo "    run-msvc-release"
	@echo "    clean"

# build g++ debug mode
build-g++-debug: CONFIG=g++-debug
build-g++-debug: CC=g++
build-g++-debug: COPTIONS=-g3 -O0 -Wall

# build g++ release mode
build-g++-release: CONFIG=g++-release
build-g++-release: CC=g++
build-g++-release: COPTIONS=-O3 -Wall -march=i686

# build msvc debug mode
build-msvc-debug: CONFIG=msvc-debug
build-msvc-debug: CC=cl
build-msvc-debug: COPTIONS=-EHsc -Zi

# build msvc release mode
build-msvc-release: CONFIG=msvc-release
build-msvc-release: CC=cl
build-msvc-release: COPTIONS=-EHsc -O2

# run g++ debug mode
run-g++-debug: CONFIG=g++-debug

# run g++ release mode
run-g++-release: CONFIG=g++-release

# run msvc debug mode
run-msvc-debug: CONFIG=msvc-debug

# run msvc release mode
run-msvc-release: CONFIG=msvc-release

# clean g++ debug mode
clean-g++-debug: CONFIG=g++-debug

# clean g++ release mode
clean-g++-release: CONFIG=g++-release

# clean msvc debug mode
clean-msvc-debug: CONFIG=msvc-debug

# clean msvc release mode
clean-msvc-release: CONFIG=msvc-release

# clean all
clean: clean-g++-debug clean-g++-release clean-msvc-debug clean-msvc-release

export CONFIG
export CC
export COPTIONS

define BUILD_COMMANDS
	mkdir -p $(CONFIG)
	@let "failed = 0"; \
	let "total = 0"; \
	for project in $(PROJECTS); do \
		$(MAKE) build -C $$project --no-print-directory; \
		if [ $$? -ne 0 ]; then \
			let "failed = failed + 1"; \
			echo $$project failed!; \
		fi; \
		let "total = total + 1"; \
	done; \
	let "succeeded = $$[$$total-$$failed]"; \
	echo; \
	echo "*************************************************"; \
	echo BUILD RESULTS: $$succeeded of $$total builds succeeded.; \
	echo See build-log.txt files for details.; \
	echo "*************************************************"; \
	echo;
endef
	
define RUN_COMMANDS
	@let "failed = 0"; \
	let "total = 0"; \
	for project in $(PROJECTS); do \
		$(MAKE) run -C $$project --no-print-directory; \
		if [ $$? -ne 0 ]; then \
			let "failed = failed + 1"; \
			echo $$project failed!; \
		fi; \
		let "total = total + 1"; \
	done; \
	let "succeeded = $$[$$total-$$failed]"; \
	echo; \
	echo "*************************************************"; \
	echo RUN RESULTS: $$succeeded of $$total tests succeeded. ; \
	echo See run-log.txt files for details.; \
	echo "*************************************************"; \
	echo;
endef

define CLEAN_COMMANDS
	@for project in $(PROJECTS); do \
		$(MAKE) clean -C $$project --no-print-directory; \
	done;
endef

build-g++-debug:
	$(BUILD_COMMANDS)
build-g++-release:
	$(BUILD_COMMANDS)
build-msvc-debug:
	$(BUILD_COMMANDS)
build-msvc-release:
	$(BUILD_COMMANDS)
run-g++-debug: build-g++-debug
	$(RUN_COMMANDS)
run-g++-release: build-g++-release
	$(RUN_COMMANDS)
run-msvc-debug: build-msvc-debug
	$(RUN_COMMANDS)
run-msvc-release: build-msvc-release
	$(RUN_COMMANDS)
clean-g++-debug:
	$(CLEAN_COMMANDS)
clean-g++-release:
	$(CLEAN_COMMANDS)
clean-msvc-debug:
	$(CLEAN_COMMANDS)
clean-msvc-release:
	$(CLEAN_COMMANDS)
