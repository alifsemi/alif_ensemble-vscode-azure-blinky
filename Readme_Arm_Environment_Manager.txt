
Arm Environment Manager
-----------------------
https://github.com/ARM-software/vscode-environment-manager#readme

-Added to .vscode/extensions.json

-Automatically downloads tools and sets up the terminal environment.
CMake, ninja, Arm GNU Toolchain, CMSIS toolbox

Configuration in project folder root: vcpkg-configuration.json

Default installation location of artifacts is under C:\Users\<user>\.vcpkg
-This can be changed by setting the VCPKG_ROOT environment variable

Without setting CMSIS_PACK_ROOT environment variable the CMSIS packs go under AppData
C:\Users\<user>\AppData\Local\Arm\Packs
