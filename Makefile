.PHONY: all build

all: build

BUILD_DIR := build
INSTALL_DIR := bin

build:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

install: build
	cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR)

clean:
	rm -rf $(BUILD_DIR) $(INSTALL_DIR)
