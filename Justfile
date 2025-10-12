export CC       := ""
export CXX      := ""
export CXXFLAGS := ""
export LDFLAGS  := ""

build *args: (cmake args)
  cmake --build build

cmake *args:
  cmake -G Ninja -B build -S . {{args}}

clean:
  git clean -xdf

fmt:
  clang-format -i \
    $(fd -E build/ .cc) \
    $(fd -E build/ .hh)

lint:
  clang-tidy \
    -p build/ \
    $(fd -E build/ .cc) \
    $(fd -E build/ .hh)
