{ pkgs ? import <nixpkgs> {} }:

with pkgs.lib;

pkgs.mkShell {
  buildInputs = with pkgs; [
    gcc
    glibc
  ];

  hardeningDisable = [ "all" ];
}
