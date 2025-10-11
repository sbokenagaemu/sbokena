export CXXFLAGS := ""
export LDFLAGS  := ""

build *args:
  just cmake {{args}}
  ninja -C build

cmake *args:
  cmake -G Ninja -B build -S .

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
