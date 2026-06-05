# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**CustusX** is a research platform for Image-Guided Surgery and Navigation, maintained by SINTEF Medical Technology. It is a research tool (not FDA/CE approved) focused on intraoperative ultrasound and navigation.

## Build System

CustusX uses a **superbuild** via Python scripts that download and build all dependencies (Qt, VTK, ITK, Eigen, OpenCV, OpenIGTLink, CTK, DCMTK, Boost, etc.).

**Full build from scratch (Linux/Mac):**
```bash
./install/cxInstaller.py --full --all --build_type Release --user_doc
```

**Incremental build of CustusX only:**
```bash
./install/cxInstaller.py --make CustusX --build_type Release
```

**Windows:**
```bat
CX/install/run_python.bat win64 PATH_to_CustusX_environment "cxInstaller.py --full --all --build_type Release --user_doc"
```

CMake minimum version: 3.16.3. C++ standard: C++14. All build outputs go to `${PROJECT_BINARY_DIR}/bin`.

## Running Tests

Tests use the **Catch** framework. Each plugin has a `testing/` subdirectory; all tests are linked into a single `Catch` executable.

```bash
# Run all unit tests (default way):
python install/cxRunTests.py --run_default_unit_tests

# Run catch directly with specific arguments:
python install/cxRunTests.py --run_catch [catch_arguments]

# Wrap each test in ctest (separate processes):
python install/cxRunTests.py --run_catch --wrap_in_ctest

# Run a specific test by tag or name (pass catch arguments directly):
python install/cxRunTests.py --run_catch "[unit]"
python install/cxRunTests.py --run_catch "MyTestName"
```

Test tags: `[unit]`, `[unstable]`, plus OS-specific tags.

## Architecture

CustusX uses a **layered architecture** built on the CTK OSGi plugin framework:

```
Application (CustusX main)
    └── GUI Layer (source/gui/) — widgets, main window
        └── Logic Layer (source/logic/) — LogicManager, plugin lifecycle
            └── Plugin Framework (CTK OSGi) — ~37 plugins in source/plugins/
                └── Resource Layer (source/resource/) — shared libraries & utilities
                    └── External Libraries (Qt, VTK, ITK, ...)
```

**Core services** (all plugin-based, discovered at runtime):
- `PatientModelService` — virtual patient data
- `TrackingService` — navigation/tracking hardware
- `VideoService` — ultrasound/video sources
- `ViewService` — 3D/2D scene rendering
- `RegistrationService` — image-to-patient registration
- `SessionStorageService` — persistent document storage
- `AcquisitionService` — recording video/tracking

**Extension points** (implement these to add functionality):
- `GUIExtenderService` — add widgets/toolbars
- `RegistrationMethodService` — custom registration algorithms
- `ReconstructionMethodService` — US reconstruction methods
- `StreamerService` — video source plugins
- `Filter` — image processing algorithm plugins

**Key source directories:**
- `source/plugins/org.custusx.*/` — individual plugins (core, filter, registration, etc.)
- `source/resource/` — shared libraries reused across plugins
- `source/gui/` — main window and UI components
- `source/logic/` — application lifecycle and plugin management
- `source/apps/` — entry points (main executables)
- `source/testing/` — Catch test framework wiring (`cxtestCatchMain.cpp`)

**Sibling applications and build directories**
- CustusS and Fraxinus are sibling applications using CustusX as a base code
- CustusX, CustusS and Fraxinus are typically located in the same directory structure
- CustusX source code: root_dir/CX/CX
- CustusS source code: root_dir/CS/CS
- Fraxinus source code: root_dir/FX/FX
- CustusX build folders (Sibling applications will look the same):
  - root_dir/CX/build_Release 
  - root_dir/CX/build_Debug
- In addition, all external repositories are also added to the same directory structure. Example:
  - VTK as root_dir/VTK/VTK, root_dir/VTK/build_Release

**Open/Closed code**

While CustusX is open source, most other repositories are closed sorce, and Claude should avoid looking into this code unless ordered:
- CustusS in root_dir/CS/CS
- The plugings in source/plugins that come from separate repositories, like:
  - source/plugins/org.custusx.ussimulator
  - source/plugins/org.custusx.gestreamer
  - source/plugins/org.custusx.tracking.shape
  - source/plugins/org.custusx.usreconstruction.vic
- Some external libraries are also closed code like:
  - root_dir/ISB_DataStreaming
  - root_dir/medtekAI
  
The used open source repositories are configured in:
- root_dir/CX/CX/install/cx/build/cxComponentAssembly.py for CustusX
- root_dir/FX/FX/script/cxsetup/cxPrivateComponentAssembly.py for Fraxinus

Closed source repositories are typically handled by a similar cxPrivateComponentAssembly.py in the closed source repositories

## Code Style

**Naming:**
- Files: `<moduleprefix><ClassName>.cpp/h` (e.g., `cxVector3D.h`, `cxMainWindow.cpp`)
- Classes: `CamelCase`; methods: `lowerCamelCase()`; member variables: `mCamelCase`
- Accessors: `getX()` / `setX()`; smart pointer typedefs: `FooPtr`
- Namespaces: `cx` (production) and `cxtest` (tests)

**Structure rules:**
- Indentation: tabs (not spaces)
- Prefer explicit variable types over `auto`; explicit types make the code easier to read and understand
- Always use `{}` for `if`/`while`/`for`/`do` bodies
- Brackets on their own line
- `.cpp` files include their own header first
- Use forward declarations in headers instead of includes
- Use `const` wherever possible
- No globals; no `delete` (use Qt/VTK/smart pointers)
- No commented-out dead code
- Try to only use a single return point in functions
- Keep function size small, usually not more than 30 lines
- All commit messages should be tagged with issue reference (If the branch name starts with cx[number], the issue reference is usually CustusX#[number])

**Units (always follow these unless explicitly stated otherwise):**
- Distances: millimeters
- Angles: radians
- Time: milliseconds
- Ratios: `[0-1]` (not percent or byte range)
- Floating point: `double` by default

**Quality:** Code must compile with zero warnings (`-Wall -Wformat-security`).

## Adding a New Plugin

Plugins follow the CTK OSGi pattern. Look at an existing small plugin (e.g., `source/plugins/org.custusx.filter.airways/`) for the CMakeLists.txt structure and service registration boilerplate. Tests go in `<plugin>/testing/` and are wired in via `cx_add_tests_to_catch()`.

## Documentation

- Developer manual: `doc/dev_manual/` (architecture, build instructions, code style)
- User manual: `doc/user_manual/`
- Built with Doxygen: CMake targets `UserDoc` and `DoxygenDoc`
- Code style reference: `doc/dev_manual/cx_dev_code_style.md`
