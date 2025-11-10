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
      $(fd -E build -F '.cc') \
      $(fd -E build -F '.hh')
  '';

  installPhase = ''
    touch $out
  '';
}
