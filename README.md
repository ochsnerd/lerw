# Build

1. Install dependencies (check CMakeLists.txt (boost, tbb))
2. `make install` -> get executable `bin/lerw`
3. test setup by executing `python interface.py`

# LSP

`default.nix` includes `clang-tools`, which contains an LSP. In order for this LSP to correctly find the
stl-headers, it needs a 'compilation database' (`compile_commands.json`). Generate it by running

```bash
mkdir build && pushd build && cmake .. && cp compile_commands.json .. && popd && rm -rf build
```

# TODO

- update makefile (copy into location for interface.py)
- write abstractions for stepper
- Refactor Point?
- Visualize walk
