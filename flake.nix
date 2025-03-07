{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs, ... }:
  let
    pkgs = {
      "x86_64-linux" = import nixpkgs { system = "x86_64-linux"; };
      "aarch64-linux" = import nixpkgs { system = "aarch64-linux"; };
      "aarch64-darwin" = import nixpkgs { system = "aarch64-darwin"; };
      "x86_64-darwin" = import nixpkgs { system = "x86_64-darwin"; };
    };
  in {
    packages."x86_64-linux".snifex-api = import ./default.nix { pkgs = pkgs.x86_64-linux; };
    packages."aarch64-linux".snifex-api = import ./default.nix { pkgs = pkgs.aarch64-linux; };
    packages."aarch64-darwin".snifex-api = import ./default.nix { pkgs = pkgs.aarch64-darwin; };
    packages."x86_64-darwin".snifex-api = import ./default.nix { pkgs = pkgs.x86_64-darwin; };
  };
}

