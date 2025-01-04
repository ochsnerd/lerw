.PHONY: all build

all: build

BUILD_DIR := build_local # having a directory "build" breaks nix-build
INSTALL_DIR := .

build:
	nix-build

build_manual:
	cmake -S . -B $(BUILD_DIR) -G Ninja
	cmake --build $(BUILD_DIR)

install: build interface.py
	mkdir -p $(INSTALL_DIR)/bin
	cp -n result/lerw $(INSTALL_DIR)/bin
	cp -n interface.py $(INSTALL_DIR)

# manually build for tests, because nix-build does full recompilation
test: build_manual interface.py
	cd $(BUILD_DIR) && ctest --output-on-failure
	python interface.py

clean:
	rm -rf $(BUILD_DIR)
