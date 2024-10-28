with import <nixpkgs> {};

gcc14Stdenv.mkDerivation {
  name = "lerw";
  src = ./.;

  buildInputs = [ clang-tools cmake ];
}
