# About the Massless Pen SteamVR Driver
This SteamVR Driver was written to enable the Massless Pen to work with any VR application compatible with SteamVR, without the need to officially support the device.

This page will serve as a short writeup of how the driver works, and how we dealt with some of the issues arising through the development of the driver.

If you are looking for the readme for the driver, click [here](README.md).

## Index
 1. [Backend to Frontend](#Backend-to-Frontend)
 2. [Tracking References](#Tracking-References)
 3. [Massless Studio](#Massless-Studio)
 4. [Gesture Handling](#Gesture-Handling)
 5. [Controller Priority](#Controller-Priority)

## Backend to Frontend
In this section, we will go over how data comes from the Massless Pen backend, and is eventually posted to SteamVR. 

First we start with the `ServerDriver` class, which is the heart of our SteamVR driver. This class implements the `vr::IServerTrackedDeviceProvider` from SteamVR and handles starting up the backend, loading settings, finding and adding the pen to SteamVR, and telling each device when to update.

When the driver starts for the first time, it will load the settings from the json file stored in `AppData\Roaming\MASSLESS\driver_massless.json` via the `SettingsManager` class, check if any required settings are invalid, and create any debug gizmos if the `attach_gizmo` setting is `true`. It will then check if an `auto_tracking_reference_serial` or `forced_tracking_reference_serial` is set, and save the serials internally.

On the first `ServerDriver::RunFrame` call, it will check if Massless Studio is not running, and if so, try and startup the Massless Pen backend systems via the `MasslessManager` class. If that is a success, then it will wait until it receives the event notifying the pen is connected, then create an instance of `PenController` which it passes up the chain to SteamVR, and stores locally so it can update the pen each frame.

When the pen is connected, it will try and find the proper tracking reference based on what other devices are connected at the time, and ask the backend for the correct offsets.

The `PenController` class handles the actual calculating of the pose of the pen, and the pen inputs to SteamVR. Each frame, ServerDriver calls the `PenController::update` function, which then calls into the `MasslessManager` and gets the most recent pen pose, calls back up to `ServerDriver` to get the current tracking reference information, and performs a calculation to figure out the global pose of the pen, which is posted to SteamVR. The `PenController` also handles recognition of gesture event paths or chains, and if any are recognised, they are posted to SteamVR as button presses.

On closing, the `ServerDriver` will close the backend, and the `SettingsManager` will save the settings to the settings file.

## Tracking References

Firstly, the Massless Pen backend supplies the pen pose as a transform from the Massless Tracker, and also can give us another transform from a given device to the Massless Tracker. So to actually get the final global pose of the pen, we need `G` - our tracking reference global pose, `T` - our Massless Tracker offset transform, and `P` - our Massless Pen offset transform. Multiplying these together with some additional transformations to change the coordinate systems, we get our final global pen pose which is then posted to SteamVR each frame. 

But how do we figure out which device is our tracking reference? To support the Vive, Vive Pro, Rift CV1, Rift S, Quest with Oculus Link, and any other headset that uses the same tracking systems, we needed to apply some heuristics to figure out which tracked device to use as the tracking reference. There are a number of devices that can be valid tracking references, they are:

 - Vive Tracker v1 & v2
 - Vive Lighthouse v1 & v2
 - Oculus Camera
 - Oculus Touch Controller v2

Because (excuding the Vive Tracker) each of these devices are exclusive (ie. you cannot have a Vive Lighthouse v1 and v2 connected at once), each device of each type has equal priority for being a tracking refernce, with the Vive Tracker being higher priority due to it not being exclusive.

When a device is selected to be the tracking reference, its serial, and type is saved into the `ServerDriver`, and its pose is saved and updated every frame, and when `ServerDriver` closes, it writes the currently saved serial to the `auto_tracking_reference_serial` setting.

When `ServerDriver` loads the settings on startup, it will check if there is a `forced_tracking_reference_serial` set, and if so, skip the heuristic step and only try and get information about the device with this serial, otherwise it will attempt to use the `auto_tracking_reference_serial`, and if it fails, will fall back to the heuristic step again.

## Massless Studio
With Massless having their own drawing program Massless Studio, we needed to allow their program to have priority access to the pen, which means monitoring the system for the startup of Massless Studio, and pausing our driver to allow their program to use the backend directly. This would work fine, however polling the system for the running processes every frame was not very performance efficient.

This problem was solved with the introduction of the `MasslessManager` class. This class takes ownership of the `MasslessPenSystem` backend wrapper, and spins up a background thread that checks the process list every 100ms to see if Massless Studio is running, and attempts to lock access to the backend using a mutex. Any other part of the driver trying to access the backend needs to do so through this instance of the `MasslessManager`, and while Massless Studio is running will not be able to lock the mutex and gain access to the backend. While this is happening, the pen is also marked as disconnected from within SteamVR as to not interfere with Massless Studio.

## Gesture Handling
Initially, accessing and reporting inputs from the pen was done through a callback from the backend reporting a pen state, which consisted of information like touch position, touch state, and surface distance among others, and for the first half of the drivers development, the touch state and position were directly mapped to buttons and touchpads within SteamVR. 

After the event-driven gesture system was finalised, we switched over to using a pseudo finite state machine to match the incoming stream of gesture events to buttons in SteamVR. For example, we know that a chain of gesture events: `TouchPadPressed`, `TouchPadMultiTapNew`, `TouchPadReleased`, `TouchPadSwipe`, `TouchPadMultiTapTotal` should match to a swipe event, so we give the `GestureHandler` this path, and an action to run when this path is matched. Now all we do is pass along any gesture events that come from the backend to our `GestureHandler`, and it will handle the rest. For our swipe gesture, we want to figure out if it is a forward swipe, or a backward swipe before passing it up to SteamVR, so in our action we check the `TouchPadSwipe` event for the direction, and press the corresponding button within SteamVR.

## Controller Priority
When a Massless Pen is connected to SteamVR, we assume that it is the highest priority controller among all the others connected. This ensures that it is assigned to the hand that you want, which is a problem when you already have 2 controllers connected and a 3rd connects, and with Vive wands where there is no distinct left and right controller hardware. We do this by setting two properties within SteamVR:

 - `vr::Prop_ControllerHandSelectionPriority_Int32` which tells SteamVR how high this controller should be prioritised. This is set to `INT32_MAX` to ensure it is always the highest priority.
- `vr::Prop_ControllerRoleHint_Int32` which tells SteamVR which hand this controller is meant for. This is set based on the `pen_handedness` setting.