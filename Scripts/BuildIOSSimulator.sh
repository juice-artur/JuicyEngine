#!/usr/bin/env bash
# Build JuicyEngine for the iOS simulator (iphonesimulator).
#
# Requirements (run on macOS):
#   - Xcode with iOS simulator SDK
#   - Optional: LunarG Vulkan SDK for macOS (installs MoltenVK for iOS simulator), VULKAN_SDK set.
#     When VULKAN_SDK is not set, CMake downloads MoltenVK automatically.
#
# Usage: ./Scripts/BuildIOSSimulator.sh

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-ios-simulator"

GENERATOR="${JE_CMAKE_GENERATOR:-Xcode}"
CONFIGURATION="${JE_CONFIGURATION:-Release}"
DEPLOYMENT_TARGET="${JE_IOS_DEPLOYMENT_TARGET:-15.0}"
ARCHITECTURES="${JE_IOS_SIM_ARCHITECTURES:-arm64}"

if [ -z "${VULKAN_SDK:-}" ]; then
    echo "NOTE: VULKAN_SDK is not set. CMake will download MoltenVK automatically." >&2
    echo "      (Or install the LunarG Vulkan SDK: https://vulkan.lunarg.com/sdk/home)" >&2
fi

echo "Configuring iOS simulator build in ${BUILD_DIR}"
cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
    -G "${GENERATOR}" \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_SYSROOT=iphonesimulator \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="${DEPLOYMENT_TARGET}" \
    -DCMAKE_OSX_ARCHITECTURES="${ARCHITECTURES}" \
    -DJE_ENABLE_VALIDATION_LAYERS=OFF

echo "Building iOS simulator app (${CONFIGURATION})..."
cmake --build "${BUILD_DIR}" --config "${CONFIGURATION}" -- \
    -sdk iphonesimulator \
    CODE_SIGNING_ALLOWED=NO

APP_PATH="${BUILD_DIR}/${CONFIGURATION}-iphonesimulator/Sandbox.app"
echo "------------------------------------------"
echo "iOS simulator build succeeded: ${APP_PATH}"
echo "------------------------------------------"