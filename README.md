# Loop-erased Random Walks [![Tests](https://github.com/ochsnerd/lerw/actions/workflows/test.yml/badge.svg)](https://github.com/ochsnerd/lerw/actions/workflows/test.yml)

## Build & Install

### Simple

1. install [nix](https://nixos.org/download/)
2. `make install INSTALL_DIR=\path\to\a\directory`

### Manually

1. Install dependencies (check default.nix/CMakeLists.txt (boost, tbb))
2. `make build_manual` -> get executable `build_manual/lerw`
3. Optional: run tests: `make test`
4. `make install_manual INSTALL_DIR=\path\to\a\directory`

Both methods will create a directory `bin` in the specified install directory,
and create a file `interface.py`.
`interface.py` contains a function that can be called to generate random walks
with the specified parameters. It stores the walks as files in the directory `data`.

## Development

`default.nix` includes `clang-tools`, which contains an LSP. In order for this LSP to correctly
find the stl-headers, it needs a 'compilation database' (`compile_commands.json`). Generate it by running

```bash
make build_manual && cp build_manual/compile_commands.json .
```

## TODO

- Implement L1 direction based on stars&bars
- Check if there are faster hashsets (e.g. https://github.com/martinus/robin-hood-hashing, https://github.com/martinus/unordered_dense)
- `grep -nr TODO include/`
- Investigate if there can be some compile-time evaluation of LINF and L1 steps for small r
- Visualize walk
- convert the package to a nix flake (also figure out what that would actually do)
