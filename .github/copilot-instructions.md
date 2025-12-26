# Ship of Harkinian Android Port - AI Coding Guidelines

## Project Overview
This is an Android port of Ship of Harkinian (SOH), a PC port of Zelda: Ocarina of Time. The project uses SDL for native Android integration, with core game logic in C/C++ compiled via CMake. Key components include decompiled N64 game code, asset management via OTR files, and enhancements like HD textures and UI mods.

## Architecture
- **`soh/`**: Core game engine and decompiled Zelda source code (C/C++)
- **`libultraship/`**: Cross-platform library for assets, ImGui UI, input handling, and platform abstraction
- **`OTRExporter/`**: Tool for extracting ROM assets into OTR (Ocarina Texture Resource) format
- **`ZAPDTR/`**: Asset processor for textures and resources
- **`Android/`**: Gradle-based Android app wrapper using SDLActivity

## Build System
- **Primary Build**: CMake with C++20, external native builds via Android Gradle
- **Dependencies**: Submodules (libultraship, OTRExporter, ZAPDTR) - clone with `--recursive`
- **Android Specific**: Uses OpenGL ES, SDL shared library, static C++ STL
- **Docker Builds**: Use `docker/scripts/build_android_project.sh` for consistent CI builds

## Key Workflows
- **Setup**: `git submodule update --init --recursive` after cloning
- **OTR Generation**: Run `cmake --build <build_dir> --target GenerateSohOtr` to extract assets from ROM
- **Android Build**: `./gradlew assembleDebug` or use Docker script
- **Debugging**: Delete `imgui.ini` and set MSAA=1 if UI issues occur

## Code Patterns
- **Includes**: Use `#include "soh/..."` for SOH-specific headers, standard C++ for others
- **Namespaces**: SOH code uses namespaces like `SohGui`, `Ship` for organization
- **CVars**: Configuration via CVar system (e.g., `CVAR_VSYNC_ENABLED`) defined in `CMake/lus-cvars.cmake`
- **Asset Loading**: All assets loaded via OTR system, not direct file paths
- **SDL Integration**: Android-specific code in `SDLActivity.java` (standard SDL with minor customizations)

## Conventions
- **Warnings**: Suppressed by default (`SUPPRESS_WARNINGS=ON` in CMake)
- **ABI Filters**: Release builds target `arm64-v8a` only; debug includes `armeabi-v7a`, `x86`, `x86_64`
- **Versioning**: Semantic versioning with NATO phonetic patch names (e.g., "Blair Alfa")
- **File Permissions**: Android app requests storage permissions for OTR extraction

## Common Pitfalls
- **Submodules**: Ensure all submodules are on correct branches (mm, android_main)
- **ROM Extraction**: OTR files must be generated before running; stored in device `SOH/` folder
- **Controller Mapping**: Some controllers have mapping issues; test thoroughly
- **Black Screens**: Often fixed by deleting `imgui.ini` or adjusting MSAA settings

## References
- Core SOH repo: https://github.com/HarbourMasters/Shipwright
- Build docs: `docs/BUILDING.md`
- Modding: `docs/MODDING.md`