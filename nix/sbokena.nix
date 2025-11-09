{
  pkgs,
  lib,
  enableWayland ? true,
  enableX11 ? true,
  ...
}: let
  llvm = pkgs.llvmPackages;
  inherit (llvm) stdenv;

  # sources of vendored external libraries
  vendoredSources = {
    googletest-src = builtins.fetchGit {
      url = "https://github.com/google/googletest";
      ref = "v1.17.0";
      rev = "52eb8108c5bdec04579160ae17225d66034bd723";
    };
    nlohmann-json-src = builtins.fetchGit {
      url = "https://github.com/nlohmann/json";
      rev = "55f93686c01528224f448c19128836e7df245f72";
    };
    raylib-src = builtins.fetchGit {
      url = "https://github.com/raysan5/raylib";
      rev = "c1ab645ca298a2801097931d1079b10ff7eb9df8";
    };
    raylib-cpp-src = builtins.fetchGit {
      url = "https://github.com/RobLoach/raylib-cpp";
      rev = "6d9d02cd242e8a4f78b6f971afcf992fb7a417dd";
    };
    raygui-src = builtins.fetchGit {
      url = "https://github.com/raysan5/raygui";
      rev = "25c8c65a6e5f0f4d4b564a0343861898c6f2778b";
    };
  };

  # build-time dependencies
  nativeBuildInputs =
    [
      llvm.bintools
      llvm.clang-tools
      pkgs.cmake
      pkgs.fd
      pkgs.ninja
    ]
    # optional dependencies for Wayland builds
    ++ lib.optionals enableWayland [
      pkgs.pkg-config
      pkgs.wayland-scanner
    ];

  # check-time dependencies
  nativeCheckInputs = [
    llvm.clang-tools
    pkgs.cmake
  ];

  # run-time dependencies
  buildInputs =
    [pkgs.libGL]
    # optional dependencies for Wayland builds
    ++ lib.optionals enableWayland [
      pkgs.libffi
      pkgs.libxkbcommon
      pkgs.wayland
    ]
    # optional dependencies for X11 builds
    ++ lib.optionals enableX11 [
      pkgs.xorg.libX11
      pkgs.xorg.libXcursor
      pkgs.xorg.libXi
      pkgs.xorg.libXinerama
      pkgs.xorg.libXrandr
    ];

  # convert a vendoredSources entry into a CMake
  # definition (-D{name}={path})
  sourceToDef = name: path:
    lib.cmakeFeature (lib.pipe name [
      (lib.replaceString "-" "_")
      lib.toUpper
    ]) "${path}";
in
  stdenv.mkDerivation {
    name = "sbokena";
    src = ../.;
    strictDeps = true;

    inherit nativeBuildInputs;
    inherit nativeCheckInputs;
    inherit buildInputs;

    cmakeFlags =
      [
        "-DCMAKE_BUILD_TYPE=Release"
        (lib.cmakeBool "GLFW_BUILD_WAYLAND" enableWayland)
        (lib.cmakeBool "GLFW_BUILD_X11" enableX11)
      ]
      ++ lib.mapAttrsToList sourceToDef vendoredSources;

    configurePhase = ''
      cmake -G Ninja -B build -S . $cmakeFlags
    '';

    buildPhase = ''
      cmake --build build
    '';

    doCheck = true;
    checkPhase = ''
      clang-tidy -p build \
        $(fd -E build/ -F '.cc') \
        $(fd -E build/ -F '.hh')
      ctest --test-dir build/tests
    '';

    installPhase = ''
      runHook preInstall
      install -Dm755 \
        build/editor/editor \
        build/game/game \
        -t $out/bin
      runHook postInstall
    '';

    # export `LD_LIBRARY_PATH` for the dev shell
    LD_LIBRARY_PATH = lib.makeLibraryPath buildInputs;
  }
