# Massless OpenVR Driver

This is an OpenVR / SteamVR driver to add support for the Massless Pen to the OpenVR ecosystem.

This repository contains the source code.
If you want to just run the OpenVR driver and use the Massless Pen inside SteamVR, please go to https://massless.io/customers and download the Massless OpenVR Driver installer.

## Building
This repository is available as an example of how to integrate the Massless Pen API into software, and as an example of how to write a complex OpenVR driver.
To build the project you will need to download the Massless Pen API from https://massless.io/developers and place the contents of the API folder into the API folder ready in this repository.

To build the project, run the following:
> git clone https://github.com/Massless-io/OpenVRDriver_MasslessPen.git

> cd OpenVRDriver_MasslessPen

> git submodule update --init --recursive

Then build the project within Visual Studio like normal. The driver, along with the supporting files will be build to: `OpenVRDriver_MasslessPen/driver_massless/Output/driver`

## Installation

There are two ways to "install" this plugin if it has been built from source:

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

By default, the driver comes with some default settings. These settings will be generated when the driver is run for the first time in:
`C:\Users\<Username>\AppData\Roaming\MASSLESS\driver_massless.json`

```json
{
	"attach_gizmo": false,
	"auto_tracking_reference_serial": "1PASH9AGH19406_Controller_Right",
	"enable_detailed_logging": false,
	"pen_handedness": "right"
}
```

### Valid Settings

`attach_gizmo` [boolean]: attaches an XYZ gizmo to the Massless Tracker, and the tracking reference point. Used for debugging.

`auto_tracking_reference_serial` [string]: is used to save the automatically deduced tracking reference device serial.

`forced_tracking_reference_serial` [string]: is used when you want to force a specific device to be the tracking reference, in the case where the auto tracking reference deduction fails.

`pen_handedness` [string]: tells the driver which hand the pen will be used in. The valid values for this are `"left"`, or `"right"`.

`enable_detailed_logging` [bool]: enables more logging to be written to the log file. Used for debugging.

# Errors

Any errors in initialisation or running of the driver will be logged to the SteamVR log file, located at
`C:\Program Files (x86)\Steam\logs\vrserver.txt`.

Any errors in the Massless API will be logged to the Massless logfile, located at
`C:\Users\<username>\AppData\Roaming\Massless\MasslessLogfile.log`.

Please report any bugs you find to mailto://support@massless.io where we can assist you, or add the bug to our internal issue tracking system.

# Documentation

A Doxygen doxyfile is included for which you may generate documentation for the driver.

There is also a small writeup detailing how certain parts of the driver work available [here](ABOUT.md)


