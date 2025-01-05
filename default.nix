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
    boost
    catch2_3
    clang-tools
    gnumake
    cmake
    ninja
    pythonEnv
  ];
}
