export CXXFLAGS := ""
export LDFLAGS  := ""

build *args:
  cmake -G Ninja -B build -S .
  ninja -C build

clean:
  git clean -xdf

fmt:
  clang-format -i $(fd .cpp) $(fd .hpp)

lint:
  clang-tidy -p build/ $(fd .cpp) $(fd .hpp)
