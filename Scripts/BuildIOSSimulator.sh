#!/usr/bin/env bash
# Build JuicyEngine for the iOS simulator (iphonesimulator).
#
# Requirements (run on macOS):
#   - Xcode with iOS simulator SDK
#   - LunarG Vulkan SDK for macOS (installs MoltenVK for iOS simulator), VULKAN_SDK set
#
# Usage: ./Scripts/BuildIOSSimulator.sh

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-ios-simulator"

GENERATOR="${JE_CMAKE_GENERATOR:-Xcode}"
CONFIGURATION="${JE_CONFIGURATION:-Release}"
DEPLOYMENT_TARGET="${JE_IOS_DEPLOYMENT_TARGET:-14.0}"
ARCHITECTURES="${JE_IOS_SIM_ARCHITECTURES:-arm64}"

if [ -z "${VULKAN_SDK:-}" ]; then
    echo "ERROR: VULKAN_SDK is not set. Install the LunarG Vulkan SDK (macOS) so MoltenVK is available." >&2
    exit 1
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