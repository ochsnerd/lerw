# Build & Run

Install nix, then inside the nix-shell:

```bash
nix-build
./result/lerw >> results.csv
python plot.py results.csv
```

or

- Check the CMakeLists.txt for dependencies (boost, tbb)
- `mkdir build && cd build && cmake .. && make`

# LSP

`default.nix` includes `clang-tools`, which contains
an LSP. In order for this LSP to correctly find the
stl-headers, it needs a 'compilation database' (`compile_commands.json`). Generate it by running

```bash
mkdir build && cd build && cmake .. && cp compile_commands.json .. && cd .. && rm -rf build
```
(removes the build-directory because it causes a strange interference with `nix-build`).

# TODO

- test (give seed as arg, check that some basic setup is correct)
- update makefile (copy into location for interface.py)
- write abstractions for stepper
- Refactor Point?
- Visualize walk
