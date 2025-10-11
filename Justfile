export CXXFLAGS := ""
export LDFLAGS  := ""

build *args:
  cmake -G Ninja -B build -S .
  ninja -C build

clean:
  git clean -xdf

fmt:
  clang-format -i $(fd .cc) $(fd .h)

lint:
  clang-tidy -p build/ $(fd .cc) $(fd .h)
