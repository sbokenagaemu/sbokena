export CC               := ""
export CXX              := ""
export CXXFLAGS         := ""
export LDFLAGS          := ""

default:
  just --list

build *args: (cmake args)
  cmake --build build

cmake *args:
  cmake \
    -G Ninja \
    -B build \
    -S . \
    -Wno-dev \
    {{args}}

clean:
  git clean -xdf

fmt *args:
  clang-format -i {{args}} \
    $(fd -E build -F '.cc') \
    $(fd -E build -F '.hh')

lint *args: (cmake args)
  cat \
    <(fd -E build/ -F '.cc') \
    <(fd -E build/ -F '.cc') \
    | parallel -j $(nproc) \
      clang-tidy -p build

test *args: (build args "-DBUILD_TESTS=1")
  ctest \
    --test-dir build/tests \
    --output-on-failure \
    --timeout 1 \
    {{args}}
