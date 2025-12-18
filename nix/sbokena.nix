{
  # config options
  buildRelease ? true,
  enableWayland ? true,
  enableX11 ? true,
  # nixpkgs
  lib,
  fetchgit,
  clangStdenv,
  llvmPackages,
  xorg,
  # dependencies
  cmake,
  dbus,
  fd,
  glfw,
  libffi,
  libGL,
  libxkbcommon,
  ninja,
  parallel,
  pkg-config,
  wayland,
  wayland-scanner,
  ...
}: let
  # sources of vendored external libraries
  vendoredSources = {
    googletest-src = fetchgit {
      url = "https://github.com/google/googletest";
      tag = "v1.17.0";
      hash = "sha256-HIHMxAUR4bjmFLoltJeIAVSulVQ6kVuIT2Ku+lwAx/4=";
    };
    nfd-src = fetchgit {
      url = "https://github.com/btzy/nativefiledialog-extended";
      tag = "v1.2.1";
    };
    nlohmann-json-src = fetchgit {
      url = "https://github.com/nlohmann/json";
      tag = "v3.12.0";
    };
    raylib-src = fetchgit {
      url = "https://github.com/raysan5/raylib";
      tag = "5.5";
    };
    raylib-cpp-src = fetchgit {
      url = "https://github.com/RobLoach/raylib-cpp";
      tag = "v5.5.0";
    };
    raygui-src = fetchgit {
      url = "https://github.com/raysan5/raygui";
      tag = "4.0";
      hash = "sha256-1qnChZYsb0e5LnPhvs6a/R5Ammgj2HWFNe9625sBRo8=";
    };
  };

  inherit
    (clangStdenv)
    isDarwin
    isLinux
    ;
  select = pred: t: f:
    if pred
    then t
    else f;

  # only enable Wayland, XDG Portals and X11 on Linux
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
    parallel
  ];

  # run-time dependencies
  buildInputs =
    [glfw]
    ++ lib.optionals isLinux [
      dbus
      libGL
    ]
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
    // lib.optionalAttrs isDarwin {
      ASAN_OPTIONS = lib.concatStringsSep ":" [
        # `nlohmann_json` has a bug on macOS, i don't wanna
        # look into it just yet, just let it pass for now.
        "detect_container_overflow=0"
      ];
    };
in
  clangStdenv.mkDerivation (finalAttrs: {
    name = "sbokena";
    src = ../.;
    strictDeps = true;

    outputs = [
      "out"
      "res"
    ];

    inherit
      nativeBuildInputs
      nativeCheckInputs
      buildInputs
      ;

    cmakeFlags =
      [
        (lib.cmakeFeature "CMAKE_BUILD_TYPE" (
          select buildRelease "Release" "Debug"
        ))
        (lib.cmakeFeature "USE_EXTERNAL_GLFW" "ON")
      ]
      ++ lib.optionals finalAttrs.doCheck [
        (lib.cmakeBool "BUILD_TESTS" true)
        (lib.cmakeBool "LLVM_COVERAGE" true)
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

    doCheck = false;
    checkPhase = ''
      cat \
        <(fd -E build -F '.cc') \
        <(fd -E build -F '.hh') \
        | parallel --will-cite -j $(nproc) \
          clang-tidy -p build
      ctest \
        --test-dir build/tests \
        --output-on-failure \
        --timeout 1
      llvm-profdata merge \
        $(fd profraw build/tests/cov) \
        -o build/tests/cov/cov.profdata
      llvm-cov report \
        build/tests/tests \
        -instr-profile build/tests/cov/cov.profdata
    '';

    installPhase = ''
      runHook preInstall

      install -Dm755 \
        build/editor/editor \
        build/game/game \
        -t $out/bin

      mkdir $res
      cp -r \
        themes \
        levels \
        $res

      runHook postInstall
    '';

    # set env-vars for the build
    inherit env;

    # passthru env-vars for shell
    passthru.env = env;
  })
