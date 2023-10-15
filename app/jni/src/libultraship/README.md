# libultraship
libultraship (LUS) is a library meant to provide reimplementations of libultra (n64 sdk) functions that run on modern hardware.

LUS uses an asset loading system where data is stored separately from the executable in an archive file ending in `.otr`, which is an [`.mpq`](http://www.zezula.net/en/mpq/main.html) compatible file. This separation the data from the executable follows modern design practices and that it is more mod friendly. All one needs to do is supply a patch `.otr` and the system will automatically replace the data.

## Contributing
LUS accepts any and all contributions. You can interact with the project via PRs, issues, email (kenixwhisperwind@gmail.com), or [Discord](https://discord.gg/shipofharkinian).
Please see [CONTRIBUTING.md](https://github.com/Kenix3/libultraship/blob/main/CONTRIBUTING.md) file for more information.

## Versioning
We use semantic versioning. We have defined the API as: every C linkage function, variable, struct, class, public class method, or enum included from libultraship.h.

## Building on Linux/Mac
```
cmake -H. -Bbuild
cmake --build build
```

## Generating a Visual Studio `.sln` on Windows
```
# Visual Studio 2022
& 'C:\Program Files\CMake\bin\cmake' -DUSE_AUTO_VCPKG=true -S . -B "build/x64" -G "Visual Studio 17 2022" -T v142 -A x64
# Visual Studio 2019
& 'C:\Program Files\CMake\bin\cmake' -DUSE_AUTO_VCPKG=true -S . -B "build/x64" -G "Visual Studio 16 2019" -T v142 -A x64
```

## To build on Windows
```
& 'C:\Program Files\CMake\bin\cmake' --build .\build\x64
```

## Sponsors
Thankyou to JetBrains for providing their IDE [CLion](https://www.jetbrains.com/clion/) to me for free!

## License
LUS is licensed under the [MIT](https://github.com/Kenix3/libultraship/blob/main/LICENSE) license.

LUS makes use of the following third party libraries and resources:
- [Fast3D](https://github.com/Kenix3/libultraship/tree/main/src/graphic/Fast3D) (MIT) render display lists.
- [ImGui](https://github.com/ocornut/imgui) (MIT)  display UI.
- [StormLib](https://github.com/ladislav-zezula/StormLib) (MIT) create and read `.mpq` compatible archive files.
- [StrHash64](https://github.com/Kenix3/libultraship/blob/main/extern/StrHash64/StrHash64.h) (MIT, zlib, BSD-3-Clause) provide crc64 implementation.
- [ZAPD](https://github.com/zeldaret/ZAPD) (MIT) asset utilities.
- [dr_libs](https://github.com/mackron/dr_libs) (MIT-0) mp3 and wav file conversion.
- [metal-cpp](https://github.com/bkaradzic/metal-cpp) (Apache 2.0) interface to the Apple Metal rendering backend.
- [nlohmann-json](https://github.com/nlohmann/json) (MIT) json parsing and saving.
- [spdlog](https://github.com/gabime/spdlog) (MIT) logging
- [stb](https://github.com/nothings/stb) (MIT) image conversion
- [thread-pool](https://github.com/bshoshany/thread-pool) (MIT) thread pool for the resource manager
- [tinyxml2](https://github.com/leethomason/tinyxml2) (zlib) parse XML files for resource loaders
- [zlib](https://github.com/madler/zlib) (zlib) compression used in StormLib
- [bzip2](https://github.com/libarchive/bzip2) (bzip2) compression used in StormLib
- [sdl2](https://github.com/libsdl-org/SDL) (zlib) window manager, controllers, and audio player
- [glew](https://github.com/nigels-com/glew) (modified BSD-3-Clause and MIT) OpenGL extension loading library.
