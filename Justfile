export CXXFLAGS := ""
export LDFLAGS  := ""

build *args:
  cmake -G Ninja -B build -S .
  ninja -C build

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
