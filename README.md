# Massless OpenVR Driver

This is an OpenVR / SteamVR driver to add support for the Massless Pen to the OpenVR ecosystem.

## Building

It's as simple as cloning the repository, initialising submodules, and opening the project in visual studio. This project was created with VS Community 2019.

**Note:** This driver was built against the Massless Pen replay dll, not the official dll. Changes to the linked .lib files may be necessary to run this driver using real hardware (Though headers should not need to be changed).

## Installation

There are two ways to "install" this plugin:

- Find your SteamVR driver directory, which should be at:
  `C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers`
  and copy the `massless` directory from `Output\driver` into the driver directory. Your folder structure should look something like this:

![directory structure](https://i.imgur.com/0GODvnL.png)
or

- Navigate to `C:\Users\<Username>\AppData\Local\openvr` and find the `openvrpaths.vrpath` file. Open this file with your text editor of choice, and under `"external_drivers"`, add another entry with the location of the `Output\driver\massless` folder. For example mine looks like this after adding the entry:

```json
{
  "config": ["C:\\Program Files (x86)\\Steam\\config"],
  "external_drivers": [
    "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Natural Locomotion\\driver\\00natural",
    "C:\\Users\\<Username>\\Documents\\Programming\\c++\\massless\\openvr-driver\\driver_massless\\Output\\driver\\massless"
  ],
  "jsonid": "vrpathreg",
  "log": ["C:\\Program Files (x86)\\Steam\\logs"],
  "runtime": [
    "C:\\Program Files (x86)\\Steam\\steamapps\\common\\SteamVR",
    "B:\\Games\\Steam\\steamapps\\common\\SteamVR",
    "B:\\Games\\Steam\\steamapps\\common\\SteamVR\\",
    "C:\\Program Files (x86)\\Steam\\steamapps\\common\\SteamVR\\"
  ],
  "version": 1
}
```

## Settings

There are some necessary settings that need to be applied before this driver can be used properly. In the `\massless\resources\settings\default.vrsettings` file, you need to add your:

- `integration_key` - This is the integration key you have been given by Massless.
- `tracking_reference_serial` - The serial number of the native tracking reference you have attached the massless tracker to. _Note: for Vive Lighthouses, find the text marked **ID** (NOT the number marked S\N), and add **LHB-** to the beginning._
- `tracking_reference_type` - A number telling the driver which type of tracking reference you are attached to, this is currently 0 for Rift Cameras, 1 for Vive Lighthouses.

An example file for a Vive Lighthouse is as follows:

```json
{
  "driver_massless": {
    "integration_key": "c0b91964caf54b49b28f7895faaca389",
    "tracking_reference_serial": "LHB-F79EA67D",
    "tracking_reference_type": 1
  }
}
```

# Errors

Any errors in initialisation or running of the driver will be logged to the SteamVR log file, located at
`C:\Program Files (x86)\Steam\logs\vrserver.txt`.

# Documentation

A Doxygen doxyfile is included for which you may generate documentation for the driver.

# Extra Notes

- I have not been able to test this driver using any Oculus VR system, though in theory it should work fine.
- The controller bindings supplied will need to be tested using real hardware to ensure they are the best.
- On the SteamVR Beta branch currently, you can access via settings a controller debug tool that shows the inputs a controller has.
