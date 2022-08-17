This is Skyrim mod that sets small items to be weightless.

Based on https://github.com/Ryan-rsm-McKenzie/CommonLibSSE

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [The Elder Scrolls V: Skyrim Special Edition](https://store.steampowered.com/app/489830)
	* Add the environment variable `Skyrim64Path` to point to the root installation of your game directory (the one containing `SkyrimSE.exe`).
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++

## Building
Adjust the following paths and set up environment variables (using powershell):
```ps
$env:Skyrim64Path = "C:\Program Files (x86)\Steam\steamapps\common\Skyrim Special Edition\"
$env:VCPKG_ROOT = "C:\vcpkg"
```
Then start building
```
git clone --recurse-submodules https://github.com/EoD/weightless-skse
cd weightless-skse
cmake --preset vs2022-windows
cmake --build build --config Release
```

## Tips
* Set `COPY_OUTPUT` to `ON` to automatically copy the built dll to the game directory, i.e. `cmake --preset vs2022-windows -DCOPY_OUTPUT=ON`
* Build the `package` target to automatically build and zip up your dll in a ready-to-distribute format.
