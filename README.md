# VSCode Azure RTOS examples
A simple Azure RTOS and CMSIS-Pack based example to setup and test VS Code development environment for Alif Ensemble kits.

- Arm GNU toolchain is used as a default. There are build-type options for IAR and ARM compiler armclang for reference.
  - You can find the compiler specific settings in `cdefault.yaml`
  - **TIP:** The tools loaded by Arm Environment Manager are configured in `vcpkg-configuration.json`.
  - To download armclang you can add "arm:compilers/arm/armclang": "^6.22.0" to the "requires" object.

# Projects in this repository
- blinky
  - This example demonstrates GPIO usage using a button press to generate and GPIO interrupt which toggles the LED state.
- ethernet
  - Simple DHCP example
  - printf is retargeted to UART4
- usb
  - Acts as a serial device. Echoes input to output
  - printf is retargeted to UART4
- sd_card
  - A readily FAT32 formatted SD card is expected. Partition should be smaller than 32GIB
  - printf is retargeted to UART4

## Note about Ensemble gen1 support
This example supports only Gen2 Ensemble Devices

## Quick start
First clone the project repository
```
git clone https://github.com/alifsemi/alif_ensemble-vscode-azure-blinky.git
```

To build the project for a supported board other than the DevKit, you have to update the `board.h` file to pick the right variant of the board.
By default the template will build for gen2 DevKit.

The required software setup consists of *VS Code*, *Git*, *CMake*, *Ninja build system*, *cmsis-toolbox*, *Arm GNU toolchain* and *Alif SE tools*.
By default the template project uses J-link so *J-link software* is required for debugging.
In addition to build tools the VS Code extensions and CMSIS packs will be downloaded automatically during the process.

To make environment setup easier this project uses *Arm Environment Manager* for downloading and configuring most of the tool dependencies.
Basically only VS Code, Alif SE tools and J-Link software need to be downloaded and installed manually.

Opening the project folder with VS Code automatically suggests installing the extensions needed by this project:
- Arm Environment Manager
- Arm CMSIS csolution
- Cortex-Debug
- Microsoft C/C++ Extension Pack

After setting up the environment you can just click the CMSIS icon and then the *context* and *build* icon to get going.

For Alif SE tools and J-link debugging support add the following entries to VS Code user settings.json (Press F1 and start typing 'User')
```
{
    "alif.setools.root" : "C:/alif-se-tools/app-release-exec",
    "cortex-debug.JLinkGDBServerPath": "C:/Program Files/SEGGER/JLink/JLinkGDBServerCL.exe"
}
```

## More detailed getting started guide
Please refer to the template project's [Getting started guide](https://github.com/alifsemi/alif_vscode-template/blob/master/doc/getting_started.md)

## Examples for Alif Peripherals
More Azure Alif peripheral examples can be found from https://github.com/alifsemi/alif_ensemble-Azure-RTOS/tree/main/THREADX/samples
