.PHONY: all build

all: build

BUILD_DIR := build
INSTALL_DIR := .

build:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

install: build interface.py
	cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR)/bin
	cp -n interface.py $(INSTALL_DIR)

test: build install interface.py
	cd $(BUILD_DIR) && ctest --output-on-failure
	python interface.py

clean:
	rm -rf $(BUILD_DIR)
