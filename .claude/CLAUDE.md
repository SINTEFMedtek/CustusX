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
- root_dir is usually ~/gitlab/cx for CustusX/CustusC and ~/gitlab/fx for Fraxinus
- CustusX source code: root_dir/CX/CX  (~/gitlab/cx/CX/CX)
- CustusS source code: root_dir/CS/CS  (~/gitlab/cx/CS/CS)
- Fraxinus source code (public): root_dir/FX/FX  (~/gitlab/fx/FX/FX)
- Fraxinus private extension: root_dir/FX/FX/org.custusx.fraxinus.private  (separate git repo cloned inside the public repo)
- CustusX build folders (Sibling applications will look the same):
  - root_dir/CX/build_Release
  - root_dir/CX/build_Debug
- In addition, all external repositories are also added to the same directory structure. Example:
  - VTK as root_dir/VTK/VTK, root_dir/VTK/build_Release

**Fraxinus public/private split**

Fraxinus is divided into a public part (root_dir/FX/FX) and a private extension (root_dir/FX/FX/org.custusx.fraxinus.private):
- The public part builds a standalone Fraxinus without any private plugins
- The private part extends the public part with additional closed-source plugins

Build scripts:
- Public build: `root_dir/FX/FX/script/cxFraxinusInstaller.py` — uses `script/cxsetup/cxPublicComponentAssembly.py` and `script/cxsetup/cxPublicComponents.py`
- Private build: `root_dir/FX/FX/org.custusx.fraxinus.private/script/cxFraxinusPrivateInstaller.py` — uses `script/cxPrivateComponentAssembly.py` and `script/cxPrivateComponents.py` from the private repo; automatically clones the public FX/FX repo if it is absent

Both repos have their own `.gitlab-ci.yml` and `.gitlab/ci/` CI pipelines. The private CI sets `GIT_CLONE_PATH` to place the private repo inside `FX/FX/` on the runner, then clones the public repo alongside it before building.

CI external lib caching:
- The public CI (`FX/FX`) uses `BASE_DIR=/builds/Ubuntu2004igstk` (etc.) and can reuse prebuilt libs from the CX package registry
- The private CI uses `BASE_DIR=/builds/FraxinusPrivate/Ubuntu2004igstk` (etc.) and cannot reuse CX prebuilt libs because those have the public `BASE_DIR` baked into their CMake config files; instead it maintains its own lib cache in the private project's package registry — the first run always does a full build

The private plugin (`org.custusx.fraxinus.private`) follows the standard CTK plugin structure and requires `manifest_headers.cmake` like all other plugins. A missing `manifest_headers.cmake` or a stale `.so` from a renamed plugin causes a "Skipping N plugins not in build manifest" warning at startup; fix by adding the file and doing a clean rebuild.

**Git remotes get rewritten by the open-source build/install scripts**

`cx.build.cxInstallData.Common` defaults `git_use_https = True`, and every component's `update()` calls `gitSetRemoteURL()` (`git remote set-url origin ...` + `git fetch`) using that setting. This means running *either* open-source build/install script resets that repo's own `origin` remote to https, even if it was previously an SSH URL:
- CustusX's own installer (`root_dir/CX/CX/install/cxInstaller.py`) resets `CX/CX`'s `origin` to `https://gitlab.sintef.no/custusx/CustusX.git`
- Fraxinus's public installer (`root_dir/FX/FX/script/cxFraxinusInstaller.py`, via `cxPublicComponentAssembly.py`) resets `FX/FX`'s `origin` to `https://gitlab.sintef.no/custusx/fraxinus.git`

This is intentional — both are open source and https doesn't require an SSH key to clone — but it means `git push` from either repo will fail with an HTTP Basic auth error (read/fetch still works over https) any time after that repo's own build script has run. Switch to SSH, push, then switch back so the next build-script run doesn't fight with your remote:

```bash
git -C CX/CX remote set-url origin git@gitlab.sintef.no:custusx/CustusX.git
git -C CX/CX push origin <branch>
git -C CX/CX remote set-url origin https://gitlab.sintef.no/custusx/CustusX.git

git -C FX/FX remote set-url origin git@gitlab.sintef.no:custusx/Fraxinus.git
git -C FX/FX push origin <branch>
git -C FX/FX remote set-url origin https://gitlab.sintef.no/custusx/fraxinus.git
```

**Open/Closed code**

While CustusX is open source, most other repositories are closed source, and Claude should avoid looking into this code unless ordered:
- CustusS in root_dir/CS/CS
- Fraxinus private extension in root_dir/FX/FX/org.custusx.fraxinus.private
- The plugins in source/plugins that come from separate repositories, like:
  - source/plugins/org.custusx.ussimulator
  - source/plugins/org.custusx.gestreamer
  - source/plugins/org.custusx.tracking.shape
  - source/plugins/org.custusx.usreconstruction.vic
- Some external libraries are also closed code like:
  - root_dir/ISB_DataStreaming
  - root_dir/medtekAI

The used open source repositories are configured in:
- root_dir/CX/CX/install/cx/build/cxComponentAssembly.py for CustusX
- root_dir/FX/FX/script/cxsetup/cxPublicComponentAssembly.py for Fraxinus (public)
- root_dir/FX/FX/org.custusx.fraxinus.private/script/cxPrivateComponentAssembly.py for Fraxinus (private)

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

### Required files for a new Filter plugin

Use `org.custusx.filter.cpd` or `org.custusx.filter.clipmesh` as a reference. Every new `org.custusx.filter.<name>` plugin needs:

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Build, `vtk_module_autoinit`, `cx_doc_define_plugin_user_docs`, `add_subdirectory(testing)` |
| `manifest_headers.cmake` | Plugin-Name, Plugin-Version, Plugin-Vendor, Plugin-Category |
| `target_libraries.cmake` | CTKPluginFramework + additional deps |
| `cxMyFilter.h` / `.cpp` | Filter implementation (subclass `FilterImpl`) |
| `cxFilterMyPluginActivator.h` / `.cpp` | CTK plugin entry point; registers filter with `FilterService_iid` |
| `doc/org.custusx.filter.<name>.md` | User documentation (see Documentation section for format rules) |
| `testing/CMakeLists.txt` | Test library wired via `cx_add_tests_to_catch()` |
| `testing/cxtestMyFilter.cpp` | Unit/integration tests |

Also add `org.custusx.filter.<name>:ON` to the plugin list in `source/plugins/CMakeLists.txt`.

### Filter plugin threading model

**Critical — misunderstanding this causes crashes and race conditions:**

- `FilterImpl::preProcess()` runs on the **main thread** before the filter starts. It populates `mCopiedInput` by calling `mInputTypes[i]->getData()` (raw pointers, not deep copies) and clones `mOptions` into `mCopiedOptions`.
- `execute()` runs on a **worker thread** (via `QtConcurrent::run`). Use `mCopiedInput` and `mCopiedOptions` here — never `mInputTypes` or `mOutputTypes`.
- `postProcess()` runs back on the **main thread** via `QFutureWatcher::finished`. Use `mInputTypes` and `mOutputTypes` here to read selections and insert output data.

**Common pitfalls:**
- Any GUI call (e.g. `QMessageBox::exec()`) inside `execute()` will crash — it must be dispatched to the main thread:
  ```cpp
  QMetaObject::invokeMethod(qApp, [&]() { /* GUI work */ }, Qt::BlockingQueuedConnection);
  ```
- Both `getInputTypes()` **and** `getOutputTypes()` must be called before `preProcess()`. These lazily populate `mInputTypes` / `mOutputTypes`. If `getOutputTypes()` is never called, `mOutputTypes` is empty and `mOutputTypes[0]->setValue(...)` in `postProcess()` will crash.

### Testing Filter plugins

**Minimal unit test** (instantiation, no patient session needed):
```cpp
cx::LogicManager::initialize();
cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());
// ... test filter metadata, options, public methods
cx::LogicManager::shutdown();
```

**Integration test** (full pipeline with patient model):
- Use `cxtest::SessionStorageTestFixture` from `cxtestSessionStorageTestFixture.h`
- Link against `cxtest_org_custusx_core_patientmodel` and `cxtestResource` in the test CMakeLists
- Call `filter.getInputTypes()` **and** `filter.getOutputTypes()` before `filter.preProcess()` (see threading pitfalls above)
- Insert test data with `services->patient()->insertData(...)` before `preProcess()`

**To test private filter methods** (e.g. file I/O helpers): move them from `private` to `protected` and expose them via a thin test subclass inside the test file. This avoids changing the public API.

**Test CMakeLists** that uses VTK directly needs `vtk_module_autoinit`:
```cmake
vtk_module_autoinit(TARGETS cxtest_my_filter MODULES VTK::FiltersCore VTK::FiltersGeneral)
```

## Documentation

- Developer manual: `doc/dev_manual/` (architecture, build instructions, code style)
- User manual: `doc/user_manual/`
- Built with Doxygen: CMake targets `UserDoc` and `DoxygenDoc`
- Code style reference: `doc/dev_manual/cx_dev_code_style.md`
- Incremental rebuild: run `ninja UserDoc` inside the build directory

### Markdown rules for plugin doc files

The doc files are processed by Doxygen and then compiled into Qt Help (`.qhp` XML). Certain Markdown constructs cause the `qcollectiongenerator` step to fail with "Opening and ending tag mismatch" on older Qt/Doxygen versions. Follow these rules:

**Safe heading styles** (match what existing CustusX filter docs use):
```markdown
Page Title {#org_custusx_filter_name}
======================================

Section heading
---------------

Sub-section heading
-------------------
```

**Do NOT use:**
- `####` or `###` headings — no existing CustusX plugin doc uses them; they create nesting that breaks older Qt Help builders
- `---` (3-dash setext) for headings — older Doxygen versions parse this as a horizontal rule instead of H2, leaving unclosed XML tags
- `<angle bracket placeholders>` in text or code blocks — even inside indented code blocks these may be left unescaped and break XML

**Required elements for a plugin doc file:**
```markdown
Page Title {#org_custusx_filter_name}      ← page anchor, must be unique
===================

\addindex filter_type_string               ← must match getType() return value exactly
                                           ← (used by Qt Help for context-sensitive help)
...content...

\addtogroup cx_user_doc_group_filter       ← adds page to the Filters group in the TOC

* \ref org_custusx_filter_name             ← self-reference (required by the group mechanism)
```

The `\addindex` keyword is matched against the widget's `objectName()` at runtime to show context-sensitive help. For filter widgets the name is set to `getType()`.

The doc folder must be registered in the plugin's `CMakeLists.txt`:
```cmake
cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org.custusx.filter.name.md")
```

## Qt Version Compatibility

CustusX CI runs on Ubuntu 20.04 (Qt 5.12) and Ubuntu 22.04 (Qt 5.15). Several Qt APIs changed between these versions:

| Avoid | Use instead | Reason |
|-------|-------------|--------|
| `Qt::SkipEmptyParts` | `QTextStream >>` | Added in Qt 5.14; not available on Ubuntu 20 |
| `QString::SkipEmptyParts` | `QTextStream >>` | Deprecated in Qt 5.14; generates warnings on Ubuntu 22 |
| `QString::split(sep, Qt::SkipEmptyParts)` | `stream >> val` | See above |

For parsing whitespace-separated numbers (e.g. reading point files or transform matrices), prefer `QTextStream >>` directly:
```cpp
QTextStream stream(&file);
double x, y, z;
stream >> x >> y >> z;
if (stream.status() != QTextStream::Ok) { /* handle error */ }
```
