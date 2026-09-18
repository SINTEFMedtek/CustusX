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

New non-trivial code (algorithms, filter logic, bug fixes) should come with an automated test where practical. Pure/static logic (mappings, threshold/bounding-box computation, etc.) is the easiest target - prefer exposing it as a testable pure function over leaving it embedded in a widget or filter with no coverage. A private method worth testing on its own can be moved to `protected` and exercised via a thin test subclass (see "Testing Filter plugins" below) rather than left untested for lack of access.

Before changing existing non-trivial code, check whether it already has test coverage. If it doesn't, add a test capturing its current/expected behavior first (where practical) so the change is verifiable and a regression is caught by the test suite rather than only by manual testing or a future bug report.

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

### Running the Python tests

The Python build/release tooling under `install/cx/` (`cxRepoHandler.py`,
`cxComponentAssembly.py`, ...) has its own `unittest` suites, separate from the C++ Catch
tests above. No extra dependencies to install — stdlib `unittest` only, everything runs
against throwaway temp directories/git repos, never real checkouts. Run all of them from
the repo root:

```bash
cd install && python3 -m unittest discover -s cx -v
```

This recurses into every `cx/**/testing/` package automatically (each needs an
`__init__.py`, same as the package under test) — a new suite added under `cx/` doesn't
need this command or the CI job updated. CustusS has an equivalent suite for its own
physical copy of `cxRepoHandler.py` plus `cxRelease.py`/`cxPrivateReposActions.py` — see
CS/CS's `doc/dev_manual/cs_dev_build_instructions.md`.

CI runs this automatically in the `test-python` stage, before the much more expensive
per-platform C++ builds.

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

**Never commit directly to `develop`/`master`**

Always commit to a branch other than `develop`/`master` in any of these repos: CustusX, CustusS, Fraxinus (public or private), or any private plugin repo (`org.custusx.core.tracking.system.ndi`, etc) — a feature branch (`cxNN-description`) or a release branch (`release/vYY.MM`) are both fine, and several unrelated fixes/issues can share the same branch. Commits go to `develop`/`master` only via review/merge (e.g. an MR), never directly.

**Squashing/amending unpushed commits**

If you need to correct or fold together commits you just made, it's fine to squash or amend them as long as none of them have been pushed to the remote yet (check with `git status`/`git log @{u}..HEAD`, or the fact that the branch was just created locally). Never do this to a commit that has already been pushed — that rewrites history other clones, MRs, or CI may already have fetched; add a new commit on top instead.

**Pull before committing**

Before committing to a branch that already exists on the remote (as opposed to one just created locally), fetch and merge/pull first so the commit is based on the branch's current tip, not a stale local copy — otherwise a push later can fail or, worse, silently diverge from work someone else (or another session) already pushed to the same branch.

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

**Windows linking: check whether the plugin's test lib needs a dummy exported class.** MSVC
needs at least one exported symbol to produce a usable `.lib` for a DLL with no exports. A
test lib whose only source registers Catch `TEST_CASE`s (no actual exported class) links fine
on Linux but fails on Windows. If the new plugin's `testing/` directory doesn't otherwise
define a class tagged with its generated export macro, add a
`cxtestExportDummyClassForLinkingOnWindowsInLibWithoutExportedClass.cpp` (see any existing
plugin's `testing/` folder for the ~5-line pattern, and `EXPORT_DUMMY_CLASS_FOR_LINKING_ON_WINDOWS_IN_LIB_WITHOUT_EXPORTED_CLASS`
in `source/resource/testUtilities/cxtestUtilities.h`) and wire it into `testing/CMakeLists.txt`
via `cx_add_class()` alongside the other test sources. This was missed when `org.custusx.liver`
was first added and had to be fixed in a follow-up commit (`85d49db00`) once it broke the
Windows build.

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

### Private plugin repos (separate git repo, e.g. org.custusx.core.tracking.system.ndi)

A private plugin living in its own git repo (rather than directly under `source/plugins/` in
this repo) joins the coordinated multi-repo release process (CustusS's `script/cxRelease.py` /
`script/cxPrivateReposActions.py`) once it's registered as a component and added to
`_getAllRepositories()` there. That process assumes every repo it touches already has a
`master` branch (`--phase final` merges `release/vYY.MM` into it) and a `develop` branch
(`--phase rc` branches the first release candidate off it).

**Create both `master` and `develop` (and push them) before adding a new private plugin repo
to `_getAllRepositories()`**, even if the repo only has a few commits so far. A repo missing
`master` was silently mishandled during the v26.08 final release
(`org.custusx.core.tracking.system.ndi`): the release script found the repo fine, but its
per-repo git commands failed with no visible summary, so `git checkout master` silently left
it on the wrong branch for every subsequent step (pull/merge/tag/push) -- the branch had to be
created and backfilled by hand after the fact.

## Documentation

- Developer manual: `doc/dev_manual/` (architecture, build instructions, code style)
- User manual: `doc/user_manual/`
- Built with Doxygen: CMake targets `UserDoc` and `DoxygenDoc`
- Code style reference: `doc/dev_manual/cx_dev_code_style.md`
- Incremental rebuild: run `ninja UserDoc` inside the build directory

Add or update the relevant user manual page whenever a change affects what a user sees or does — a new/changed GUI widget, a new plugin's own doc page, a changed workflow, a renamed/moved setting, etc. Look for an existing page covering the affected widget/plugin/feature under `doc/user_manual/` or `doc/shared_manual/` before assuming one needs to be added; a plugin's own page lives at `source/plugins/<plugin>/doc/<plugin>.md` (see "Required files for a new Filter plugin" above).

### Markdown rules for plugin doc files

**Check this section before the first build of any brand-new `.md` doc page** (a new plugin's `doc/org.custusx.<name>.md`, or any other page added to the `UserDoc`/`DoxygenDoc` input). The doc files are processed by Doxygen and then compiled into Qt Help (`.qhp` XML) — a new page that doesn't follow these rules typically builds fine through Doxygen but then fails the `qhelpgenerator`/`qcollectiongenerator` step with "Opening and ending tag mismatch", which looks like a doc-build/tooling problem but is actually a content problem in the page you just added. The fix is editing the page to match the rules below, then rebuilding — not retrying the same content. Follow these rules:

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
