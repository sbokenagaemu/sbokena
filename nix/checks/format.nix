{pkgs, ...}:
pkgs.stdenvNoCC.mkDerivation {
  name = "format";
  src = ../../.;

  nativeBuildInputs = [
    pkgs.alejandra
    pkgs.fd
    pkgs.llvmPackages.clang-tools
  ];

  dontBuild = true;
  doCheck = true;

  checkPhase = ''
    alejandra -c .
    clang-format \
      --dry-run -Werror \
      $(fd -F '.cc') \
      $(fd -F '.h')
  '';

  installPhase = ''
    touch $out
  '';
}
