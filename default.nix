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

  buildInputs = [
    tbb
    boost185
    catch2_3
    clang-tools
    cmake
    pythonEnv
  ];
}
