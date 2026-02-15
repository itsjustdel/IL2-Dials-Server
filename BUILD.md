# Building IL-2 Dials Server

This project has been migrated from Winsock to Boost.Asio for modern, cross-platform networking support.

## Prerequisites

- Visual Studio 2022 (v143 toolset)
- vcpkg package manager
- Windows 10 SDK

## Setting up vcpkg

1. Install vcpkg if you haven't already:
   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   bootstrap-vcpkg.bat
   vcpkg integrate install
   ```

2. The project uses a vcpkg manifest file (`vcpkg.json`) to manage dependencies. The required dependencies will be automatically installed when you build the project.

## Building the Project

1. Open `Il2ServerAppLateNight.sln` in Visual Studio 2022

2. vcpkg will automatically restore the Boost.Asio dependency based on the `vcpkg.json` manifest file

3. Select your desired configuration (Debug/Release) and platform (Win32/x64)

4. Build the solution (Ctrl+Shift+B)

## Dependencies

The project now depends on:
- **Boost.Asio**: Modern C++ networking library (header-only, managed via vcpkg)
- **iphlpapi.lib**: Windows IP Helper API (still used for network adapter enumeration)

### What Changed

- **Removed**: `ws2_32.lib` (Winsock library)
- **Added**: Boost.Asio via vcpkg manifest

## Troubleshooting

### vcpkg integration issues

If Visual Studio doesn't find the vcpkg packages:
1. Ensure vcpkg is integrated: `vcpkg integrate install`
2. Check that `vcpkg.json` is in the project directory
3. Clean and rebuild the solution

### Build errors related to _WIN32_WINNT

The project now defines `_WIN32_WINNT=0x0601` (Windows 7) in the project settings. This is required for Boost.Asio. If you see errors, ensure this preprocessor definition is set correctly.

## Port Configuration

The server listens on UDP port **11200** by default. This can be changed via the UI when running the application.
