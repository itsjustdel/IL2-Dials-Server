# GitHub Copilot Instructions for IL2-Dials-Server

## Project Overview

IL2-Dials-Server is a reverse engineering project for the IL-2 Sturmovik flight simulator that extracts flight telemetry data and serves it over UDP. This enables the data to power dashboard applications and home cockpits.

**Key Features:**
- Reverse engineers IL-2 Sturmovik to extract flight telemetry
- UDP server for broadcasting data to clients
- Windows Forms GUI application for server control
- Works with companion [Android and Windows client applications](https://github.com/itsjustdel/IL2-Dials-Client)

## Technology Stack

- **Language**: C++/CLI (C++ with .NET Framework extensions)
- **UI Framework**: Windows Forms (WinForms)
- **Build System**: Visual Studio 2019+ with MSBuild
- **Platform**: Windows (x86/x64)
- **Networking**: UDP sockets for telemetry broadcast

## Architecture

The project is structured with the following main components:

- `Il2ServerAppLateNight/` - Main server application
  - `Form1.h/cpp` - Main Windows Forms UI
  - `src/Server/` - UDP server implementation
  - `src/Main/` - Core application logic
  - `src/Altimeter/`, `src/TurnBall/`, etc. - Individual gauge/instrument data handlers
  - `src/Scan/` - Memory scanning and reverse engineering code
  - `src/*Planes/` - Aircraft-specific data structures (RUPlanes, USPlanes, UKPlanes)
- `Winlauncher/` - Launcher utility
- `IL2Closer/` - Utility for closing IL-2

## Coding Conventions

### Style Guidelines

1. **Naming Conventions**:
   - Use camelCase for local variables and private fields (e.g., `portNumber`, `skipCheckBoxEvent`)
   - Use PascalCase for class names, methods, and public properties
   - Prefix private member variables with nothing (no underscore)
   - Use descriptive names that clearly indicate purpose

2. **Code Organization**:
   - Use `#pragma once` for header guards
   - Organize includes: system headers first, then local headers
   - Keep related functionality grouped in separate source files

3. **C++/CLI Specifics**:
   - Use `ref class` for managed classes
   - Use `System::` namespace prefix for .NET types
   - Use `^` (hat) for managed references instead of `*`
   - Use `using namespace` statements at the top of header files for frequently used namespaces

4. **Comments**:
   - Use `//` for single-line comments
   - Use XML-style `/// <summary>` comments for class/method documentation
   - Keep comments clear and concise
   - Comment complex algorithms, especially memory scanning/reverse engineering code

### Code Quality

- Maintain clean separation between UI code and business logic
- Handle network errors gracefully (UDP is unreliable)
- Consider performance - this runs alongside a flight simulator
- Test with actual IL-2 Sturmovik when making changes to memory scanning code

## Building and Testing

### Prerequisites

- Visual Studio 2019 or later with C++/CLI support
- .NET Framework 4.x
- Windows SDK

### Build Instructions

1. Open `Il2ServerAppLateNight.sln` in Visual Studio
2. Select desired configuration (Debug/Release) and platform (x86/x64)
3. Build solution (F7 or Build > Build Solution)

### Testing

- Manual testing is primary method due to nature of reverse engineering
- Test with IL-2 Sturmovik running
- Verify UDP data transmission to client applications
- Test various aircraft types (RU, US, UK planes have different data structures)

## Development Guidelines

### When Making Changes

1. **Memory Scanning Code**: Changes to scanning logic in `src/Scan/` require extensive testing with the actual game
2. **Network Code**: Ensure UDP server handles disconnections and errors gracefully
3. **UI Changes**: Test on different Windows versions and DPI settings
4. **Aircraft Data**: Each aircraft type may have different memory layouts - test thoroughly

### Adding New Features

- Follow existing patterns for aircraft-specific data handlers
- Add new gauge types in separate directories under `src/`
- Update UI in `Form1.h/cpp` to expose new functionality
- Consider backward compatibility with existing clients

### Performance Considerations

- Minimize memory allocations in hot paths (data scanning runs frequently)
- Keep UDP packet sizes reasonable
- Profile performance impact on IL-2 Sturmovik gameplay

## Important Notes

- This is a reverse engineering project - memory addresses may change with game updates
- Always have consent/permission when working with reverse engineered code
- The project has authorization from 1C Company (the publisher)
- Community testing via Discord is essential for quality assurance

## Resources

- [Release Distribution](https://dellywelly.itch.io/il-2-dials-server)
- [Client Repository](https://github.com/itsjustdel/IL2-Dials-Client)
- [Community Discord](https://discord.gg/tuN8yuwS9V)

## Help and Support

For issues or questions:
1. Check existing issues on GitHub
2. Join the Discord community for testing and feedback
3. Review documentation in the repository

---

When using GitHub Copilot with this repository, these instructions help ensure suggestions align with the project's architecture, coding style, and special considerations for working with reverse engineered code.
