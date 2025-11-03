# sbokena contributor's guide

this document aims to clarify style choices and practices used in the project.
please try to adhere to these choices, this makes the code more readable and
maintainable.

## style

formatting is generally handled by `clang-format`. let your editor auto-format
the code, and only disable it for specific blocks which has special reason to
be aligned a certain way.

disable `clang-format` for a section of code like so:

```cpp
int some_function() {
  // clang-format off
  const std::array<int, 9> some_matrix = {
      1,   2,   3,
     10,  20,  30,
    100, 200, 300,
  };
  // clang-format on
}
```

in case of function calls which are too long and are bin-packed by
`clang-format`, prefer creating constants for the parameters separately:

```cpp
// don't do this
// width (2nd argument) and height (3rd argument) are aligned differently
DrawText("hello, world!", window_width / 2 - size.x / 2,
          window_height / 2 - size.y / 2, 20, DARKGRAY);

// do this instead
const char *msg = "hello, world!";
const int msg_x = window_width / 2 - size.x / 2;
const int msg_y = window_height / 2 - size.y / 2;
const Color msg_color = DARKGRAY;
DrawText(msg, msg_x, msg_y, msg_color);
```

## practices

### tag your commits & keep them small

every commit message should be tagged with the primary part of code that
changed in that commit. this also implies that each commit should only really
affect one part of the codebase.

example commit messages:

```
build: add package <some_package>
docs: add <topic> to <file>
ci: fix <something>
```

in the case that a commit must affect multiple parts of the codebase at once,
use the `general:` tag. (i know i didn't really follow this myself, but please
try to.)

### don't push directly, use pull requests

this is common practice, it allows people to do things independently without
things changing/breaking all the time.

### modern C++ use

use of modern C++ constructs are welcome, as long as they do helpful things like

- makes code mode readable
- makes bugs less likely
- makes code re-usable (appropriately)
- makes code more performant

in that order.

thus, it's preferable to

- use ranged-for instead of manual `for` loops
- don't try to be too clever with templates
- use concepts in templates instead of bare types
- use `std::unique_ptr` and `std::shared_ptr` instead of raw pointers

and so on.

### testing

we use `ctest` for testing.

before making a PR, run the tests locally on your machine first
(both with and without Nix, if possible):

```sh
# test with Nix
nix flake check -L
nix build -L # some tests are run after building

# test without Nix
just test
```

### dependencies

we rely on CMake and Nix to manage dependencies. thus, in practice, to add a
dependency named `foo`, located at `https://github.com/bar/foo` to the project:

0. make sure its license is compatible with ours (GNU AGPLv3)

1. add an entry for it in `vendor/CMakeLists.txt`:

   ```cmake
   if (DEFINED ENV{foo_src})
     FetchContent_Declare(
       foo
       SOURCE_DIR "$ENV{foo_src}"
     )
   else ()
     FetchContent_Declare(
       foo
       GIT_REPOSITORY "https://github.com/bar/foo"
       GIT_TAG "<foo's current release tag>"

   endif ()

   FetchContent_MakeAvailable(foo)
   ```

2. add it as a linked library to the appropriate target(s) in `CMakeLists.txt`:

   ```cmake
   # given an executable 'binary'
   target_link_libraries(binary PRIVATE foo)
   ```

3. fetch it in `nix/sbokena.nix`:

   ```nix
   vendoredSources = {
     # ...other libraries
     foo-src = builtins.fetchGit {
       url = "<link to the Git repository>";
       rev = "<Git commit hash of the release tag>";
     };
   };
   ```

4. check that everything still works:

   ```sh
   nix build -L # build it with Nix
   nix flake check -L # check if your formatting is right
   just clean build # build it without Nix
   ```

and now you can use the dependency in your code.
