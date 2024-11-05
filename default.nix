with import <nixpkgs> {};

let
  pythonEnv = python312.withPackages (ps: with ps; [
    numpy
    matplotlib
    pandas
  ]);
in
gcc14Stdenv.mkDerivation {
  name = "lerw";
  src = ./.;

  buildInputs = [
    tbb
    boost185
    clang-tools
    cmake
    pythonEnv
  ];
}
