{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation {
  name = "snifex-api";
  src = ./src;

  installPhase = ''
    mkdir -p $out/include
    cp snifex-api.h $out/include/
  '';
}
