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
    clang-tools
    cmake
    pythonEnv
  ];
}
