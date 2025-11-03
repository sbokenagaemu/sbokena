export CC               := ""
export CXX              := ""
export CXXFLAGS         := ""
export LDFLAGS          := ""

default:
  just --list

build *args: (cmake args)
  cmake --build build

cmake *args:
  cmake -G Ninja -B build -S . {{args}}

clean:
  git clean -xdf

fmt *args:
  clang-format -i {{args}} $(fd -E build/ .cc) $(fd -E build/ .hh)

lint *args: (cmake args)
  clang-tidy -p build/ {{args}} $(fd -E build/ .cc)

test *args: (build args)
  ctest --test-dir build/tests {{args}}
