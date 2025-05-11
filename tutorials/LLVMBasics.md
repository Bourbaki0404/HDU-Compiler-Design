# LLVM Basics

### Build the Project

> See https://llvm.org/docs/CMake.html#quick-start for building the llvm-project with cmake.

```bash
cmake -DCMAKE_BUILD_TYPE=Debug path/to/llvm/source/root
cmake --build . -- -j8
cmake --build . --target install
```

- Note: The total memory (main memory + swap) should be larger than 45GB or your computer will freeze.

### Configure the vscode

- Ctrl+Shift+P in vscode.
- Paste in the following json text.
```json
{
    "configurations": [
      {
        "name": "Linux",
        "includePath": [
          "${workspaceFolder}/**",
          "/usr/lib/llvm-14/include", //the path to the llvm
          "/usr/include/c++/11",
          "/usr/include/x86_64-linux-gnu/c++/11"
        ],
        "defines": [
          "_GNU_SOURCE",
          "__STDC_CONSTANT_MACROS",
          "__STDC_FORMAT_MACROS",
          "__STDC_LIMIT_MACROS"
        ],
        "compilerPath": "/usr/bin/clang++",
        "cStandard": "c11",
        "cppStandard": "c++14",
        "intelliSenseMode": "linux-clang-x64"
      }
    ],
    "version": 4
  }
```