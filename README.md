# Build & Run

(inside the nix-shell)

```bash
nix-build
./result/lerw >> results.csv
python plot.py results.csv
```

# LSP

`default.nix` includes `clang-tools`, which contains
an LSP. In order for this LSP to correctly find the
stl-headers, it needs a 'compilation database' (`compile_commands.json`). Generate it by running

```bash
mkdir build && cd build && cmake .. && cp compile_commands.json .. && cd .. && rm -rf build
```
(removes the build-directory because it causes a strange interference with `nix-build`).

# TODO

- implement interesting generalizations (long-range interactions, different grids)
