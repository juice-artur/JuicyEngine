# Juicy Engine

![JuicyEngine](Resources/Branding/JuicyEngineSplash.jpg "JuicyEngine")


# Goals
- C++ is the main programming language
- Vulkan as primary graphics API
---
# Project setup

## Cloning
```bash
git clone https://github.com/juice-artur/JuicyEngine.git --recursive
```

## Build
1. Create a build directory in the project root: ```mkdir build && cd build```
2. Generate project files with CMake: ```cmake ..```

---
# iOS support

The engine renders with Vulkan on iOS through **MoltenVK** (Vulkan translated onto Metal).
The Vulkan instance, surface and swapchain are created through SDL3, so no platform specific
graphics code is required. Input works through SDL's touch-to-mouse event synthesis.

## Requirements
- macOS with Xcode (iOS SDK + simulator SDK)
- LunarG Vulkan SDK for macOS (installs MoltenVK for iOS). Set `VULKAN_SDK`.

## Building for an iOS device
```bash
./Scripts/BuildIOS.sh
```
Produces `build-ios/Release-iphoneos/Sandbox.app`. Sign the bundle with your team before
installing on a device, or run it directly in Xcode.

## Building for the iOS simulator
```bash
./Scripts/BuildIOSSimulator.sh
```
Produces `build-ios-simulator/Release-iphonesimulator/Sandbox.app`.

## Building in Xcode
```bash
cmake -S . -B build-ios -G Xcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_SYSROOT=iphoneos \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 \
  -DCMAKE_OSX_ARCHITECTURES=arm64
```
Then open `build-ios/Sandbox.xcodeproj` and run the `Sandbox` scheme on your device or simulator.

## CI
The GitHub workflow `.github/workflows/ios.yml` builds (and verifies) the app for both a
device and the simulator on every push to `master` and on pull requests.

Downloadable bundles (`Sandbox-ios-device` / `Sandbox-ios-simulator`) are attached to
every iOS workflow run as GitHub Actions artifacts:

1. Open the **Actions** tab -> pick the latest **iOS** run.
2. In the **Artifacts** section download `Sandbox-ios-device` (for iPhones when signed)
   or `Sandbox-ios-simulator` (like the real device).

