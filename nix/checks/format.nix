{
  # nixpkgs
  stdenvNoCC,
  llvmPackages,
  # dependencies
  alejandra,
  fd,
  ...
}:
stdenvNoCC.mkDerivation {
  name = "format";
  src = ../../.;

  nativeBuildInputs = [
    llvmPackages.clang-tools
    alejandra
    fd
  ];

  dontBuild = true;
  doCheck = true;

  checkPhase = ''
    alejandra -c .
    clang-format \
      --dry-run -Werror \
      $(fd -E build -F '.cc') \
      $(fd -E build -F '.hh')
  '';

  installPhase = ''
    touch $out
  '';
}
