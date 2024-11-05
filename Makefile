.PHONY: all clean build generate visualize

all: visualize

build:
	nix-build

generate: build
	./result/lerw > results.csv

visualize: generate
	python plot.py results.csv

clean:
	rm -f results.csv
	rm -rf result
