# Torch - [T]orch is [O]ur [R]esource [C]onversion [H]elper
A generic asset processor for games

## Usage
`./torch otr baserom.z64`
`./torch code baserom.z64`

# Windows

## Visual Studio

### Dependencies

  * Visual Studio 2022 Community Edition with the C++ feature set
  * One of the Windows SDKs that comes with Visual Studio, for example the current Windows 10 version 10.0.19041.0
  * The `MSVC v142 - VS 2019 C++ build tools` component of Visual Studio
  * Cmake (can be installed via chocolatey or manually)

### Compile

``` powershell
# Setup cmake project
& 'C:\Program Files\CMake\bin\cmake' -S . -B "build/x64" -G "Visual Studio 17 2022" -T v142 -A x64 -DCMAKE_BUILD_TYPE=Debug
# or for VS2019
& 'C:\Program Files\CMake\bin\cmake' -S . -B "build/x64" -G "Visual Studio 16 2019" -T v142 -A x64 -DCMAKE_BUILD_TYPE=Debug
# Compile project
& 'C:\Program Files\CMake\bin\cmake.exe' --build .\build\x64
```

## WSL Ubuntu

### Dependencies

``` bash
sudo apt-get install cmake ninja-build libbz2-dev
```

### Compile

``` bash
cmake -H. -Bbuild-cmake -GNinja -DCMAKE_BUILD_TYPE=Debug
cmake --build build-cmake -j
```

# Linux

### Dependencies

Requires `cmake, ninja-build, libbz2-dev`

### Compile

``` bash
cmake -H. -Bbuild-cmake -GNinja -DCMAKE_BUILD_TYPE=Debug
cmake --build build-cmake -j
```

# Mac

### Dependencies

Requires Xcode (or xcode-tools) && `cmake, ninja` (can be installed via homebrew, macports, etc)

### Compile

``` bash
cmake -H. -Bbuild-cmake -GNinja -DCMAKE_BUILD_TYPE=Debug
cmake --build build-cmake -j
```
