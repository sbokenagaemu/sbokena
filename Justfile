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
    | parallel --will-cite -j $(nproc) \
      clang-tidy -p build

test *args: (build args "-DBUILD_TESTS=1")
  ctest \
    --test-dir build/tests \
    --output-on-failure \
    --timeout 1 \
    {{args}}

cov-show *args: (test args "-DLLVM_COVERAGE=1")
  llvm-profdata merge \
    $(fd profraw build/tests/cov) \
    -o build/tests/cov/cov.profdata
  llvm-cov show \
    build/tests/tests \
    -instr-profile build/tests/cov/cov.profdata

cov-report *args: (test args "-DLLVM_COVERAGE=1")
  llvm-profdata merge \
    $(fd profraw build/tests/cov) \
    -o build/tests/cov/cov.profdata
  llvm-cov report \
    build/tests/tests \
    -instr-profile build/tests/cov/cov.profdata
