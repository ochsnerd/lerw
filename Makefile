.PHONY: all build build_manual

all: build

BUILD_DIR := build_manual # having a directory "build" breaks nix-build
INSTALL_DIR := .

build:
	nix-build

install: build interface.py
	mkdir -p $(INSTALL_DIR)/bin
# the file in result is write-protected.
# cp -f so that multiple installs work
	cp -f result/lerw $(INSTALL_DIR)/bin
	cp -n interface.py $(INSTALL_DIR)

build_manual:
	cmake -S . -B $(BUILD_DIR) -G Ninja
	cmake --build $(BUILD_DIR)

install_manual: build_manual interface.py
	cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR)/bin
	cp -n interface.py $(INSTALL_DIR)

# manually build for tests, because nix-build does full recompilation
test: build_manual interface.py
	cd $(BUILD_DIR) && ctest --output-on-failure
	python interface.py

clean:
	rm -rf $(BUILD_DIR)
