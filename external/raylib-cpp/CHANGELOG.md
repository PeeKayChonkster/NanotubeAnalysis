# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [4.1.0] - 2022-02-25
### Added
- Web-based example at [core_basic_window_web.cpp](examples/core/core_basic_window_web.cpp)
- An [Exception](README.md#exceptions) is now thrown when assets fail to load during object construction (applies to Music, Texture, Image, Sound, etc)

### Fixed
- TextSubtext() address bounding by @RobLoach in https://github.com/RobLoach/raylib-cpp/pull/159

### Changed
- Unit tests now use [raylib-assert](https://github.com/RobLoach/raylib-assert)

## [4.0.4] - 2022-01-09
### Fixed
- Renamed CMake project name to `raylib_cpp`

## [4.0.3] - 2022-01-09
### Changed
- CMake now used for raylib dependency, rather than git submodules
- Standardized functions across:
  - Color
  - Material
  - Mesh
  - Model
  - ModelAnimation
  - Physics
  - Ray
  - Rectangle
  - Texture
  - Window
- Renamed `package.json` to `clib.json` to follow [clib](https://github.com/clibs/clib) schema

## [4.0.2] - 2021-12-24
### Added
- `Image::IsLoaded()`
- `Wave::IsLoaded()`
- `Sound::IsLoaded()`
- `Music::IsLoaded()`
- Shader example
- Function overrides for `std::string` support
    - `raylib::DrawText()`
    - `raylib::DrawTextEx()`
    - `raylib::DrawTextPro()`
    - `raylib::TextSubtext()`
    - `raylib::TextReplace()`
    - `raylib::TextInsert()`
    - `raylib::TextAppend()`
    - `raylib::TextFindIndex()`
    - `raylib::TextToUpper()`
    - `raylib::TextToLower()`
    - `raylib::TextToPascal()`
    - `raylib::TextToInteger()`

### Fixed
- Shader constructor
- Vector2 muliplication operator behavior ([#155](https://github.com/RobLoach/raylib-cpp/issues/155))

### Changed
- Merged `Text.hpp` into `Functions.hpp`

## [4.0.1] - 2021-11-20
### Changes
- Fix compiler warning

## [4.0.0] - 2021-11-04
### Changes
- Update to [raylib 4.0.0](https://github.com/raysan5/raylib/releases/tag/4.0.0)
- For tests, replaced catch.hpp with `assert()` calls
- Added assignment operators for objects ([#142](https://github.com/RobLoach/raylib-cpp/pull/142) by [@marciejewiczow](https://github.com/maciejewiczow))
- Replaced `NULL` with `nullptr`
- Fixed memory leak in `ModelAnimation::Load` method

## [3.7.0] - 2021-04-23
### Added
- `Window::ClearBackground()` ([#127](https://github.com/RobLoach/raylib-cpp/pull/127) by [ufrshubham](https://github.com/ufrshubham))
### Changes
- Updated to raylib 3.7.0

## [3.5.0] - 2021-03-24
### Added
- VSCode project template (by [KnockerPulsar](https://github.com/KnockerPulsar))
### Changes
- Updated documentation

## [3.5.0-beta3] - 2021-02-18
### Added
- Added a C++ version of raylib's loading thread example (by [@pkeir]([https://github.com/pkeir))
- Updated documentation
### Fixed
- Made the global wrapped functions static to avoid redeclaration

## [v3.5.0-beta2] - 2021-01-18
### Fixed
- `Font.charsPadding` loading
### Removed
- `DroppedFiles` class replaced with `std::vector<std::string> raylib::GetDroppedFiles()`

## [v3.5.0-beta1] - 2021-01-02
### Added
- `Model.Draw()` and `Model.DrawWires()`
- `models_first_person_maze.cpp` example
- String override functions to allow using `std::string` directly instead of `const char*`
- `std::vector<std::string>` wrappers with `raylib::GetDirectoryFiles()` and `raylib::GetDroppedFiles()`
- `raylib::Color::RayWhite()` static functions to build `Color` objects
- `Rectangle.GetPosition()` and `Rectangle.SetPosition()`
- `Rectangle.GetSize()` and `Rectangle.SetSize()`

### Fixed
- Most objects are now passed by reference
- `Mouse::SetX()` and `Mouse::SetY()` setting incorrect values
- Error protection when unloading images, materials, models and meshes

### Changed
- `Mouse` functions are now `static`. Use `Mouse::SetX()` instead of using `Mouse mouse`.
- `Camera*::BeginMode()` and `Camera*::EndMode()` no longer have 2D/3D in the name

## [v3.5.0-alpha1] - 2020-12-26
### Changed
- Update to raylib 3.5.0

### Added
- Documentation

### Removed
- Static inline Color variables (like `raylib::Color::RayWhite`). Instead, use `::RAYWHITE`.
