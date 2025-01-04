with import <nixpkgs> {};

let
  pythonEnv = python312.withPackages (ps: with ps; [
    numpy
    matplotlib
    pandas
    scipy
  ]);
in
gcc14Stdenv.mkDerivation {
  name = "lerw";
  src = ./.;
  doCheck = true;

  buildInputs = [
    tbb
    boost185
    catch2_3
    clang-tools
    cmake
    ninja
    pythonEnv
  ];
}
