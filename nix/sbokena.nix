{
  # config options
  buildRelease ? true,
  enableWayland ? true,
  enableX11 ? true,
  # nixpkgs
  lib,
  clangStdenv,
  llvmPackages,
  xorg,
  # dependencies
  cmake,
  fd,
  libffi,
  libGL,
  libxkbcommon,
  ninja,
  pkg-config,
  wayland,
  wayland-scanner,
  ...
}: let
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

  inherit (clangStdenv) isLinux;
  select = pred: t: f:
    if pred
    then t
    else f;

  # only enable Wayland and X11 on Linux
  enableWayland' = select isLinux enableWayland false;
  enableX11' = select isLinux enableX11 false;

  # build-time dependencies
  nativeBuildInputs =
    [
      llvmPackages.bintools
      llvmPackages.clang-tools
      cmake
      fd
      ninja
    ]
    # optional dependencies for Wayland builds
    ++ lib.optionals enableWayland' [
      pkg-config
      wayland-scanner
    ];

  # check-time dependencies
  nativeCheckInputs = [
    llvmPackages.clang-tools
    cmake
  ];

  # run-time dependencies
  buildInputs =
    lib.optionals isLinux [libGL]
    # optional dependencies for Wayland builds
    ++ lib.optionals enableWayland' [
      libffi
      libxkbcommon
      wayland
    ]
    # optional dependencies for X11 builds
    ++ lib.optionals enableX11' [
      xorg.libX11
      xorg.libXcursor
      xorg.libXi
      xorg.libXinerama
      xorg.libXrandr
    ];

  # kebab-case -> SCREAMING_CASE
  toScreaming = lib.flip lib.pipe [
    (lib.replaceString "-" "_")
    lib.toUpper
  ];

  # convert a vendoredSources entry into a
  # env-var name-value pair
  sourceToEnv = name: path:
    lib.nameValuePair
    (toScreaming name)
    "${path}";

  # vendoredSources as env-vars
  vendoredSources' =
    lib.mapAttrs'
    sourceToEnv
    vendoredSources;

  # env-vars for the build
  env =
    vendoredSources'
    // {
      LD_LIBRARY_PATH =
        lib.makeLibraryPath
        buildInputs;
    };
in
  clangStdenv.mkDerivation {
    name = "sbokena";
    src = ../.;
    strictDeps = true;

    inherit nativeBuildInputs;
    inherit nativeCheckInputs;
    inherit buildInputs;

    cmakeFlags =
      [
        (lib.cmakeFeature "CMAKE_BUILD_TYPE" (
          select buildRelease "Release" "Debug"
        ))
      ]
      ++ lib.optionals isLinux [
        (lib.cmakeBool "GLFW_BUILD_WAYLAND" enableWayland')
        (lib.cmakeBool "GLFW_BUILD_X11" enableX11')
      ];

    configurePhase = ''
      cmake -G Ninja -B build -S . $cmakeFlags
    '';

    buildPhase = ''
      cmake --build build
    '';

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
      cp -r themes $out
      runHook postInstall
    '';

    # set env-vars for the build
    inherit env;

    # passthru env-vars for shell
    passthru.env = env;
  }
