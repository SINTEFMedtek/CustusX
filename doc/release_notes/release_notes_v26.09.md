# CustusX release notes — v26.09

_Changes since v26.08_

## Important: the patient and settings folders have moved

CustusX now keeps all its data under a single `CustusX` folder in your home folder (`~` on Linux/macOS, `C:\Users\` + your user name on Windows).

| | v26.08 and earlier | v26.09 |
|---|---|---|
| Patients | `~/Patients/Laboratory` | `~/CustusX/Patients/Laboratory` |
| Settings | `~/CustusX_settings` | `~/CustusX/CustusX_settings` |

(`Laboratory` is the default profile. If you have used other profiles, each one has its own subfolder next to it, in both places.)

Nothing is moved or deleted automatically. Your old folders are left as they were, and v26.09 starts with an empty patient list and default settings. If you want your old data in the new locations, close CustusX first, then copy it over yourself:

- **Patients:** copy the contents of each old profile folder (e.g. `~/Patients/Laboratory`) into the matching new one (e.g. `~/CustusX/Patients/Laboratory`). Each patient is a folder ending in `.cx3`, e.g. `2017-09-29_10-07_Laboratory_4.cx3`, containing `custusdoc.xml` and subfolders such as `Images` and `Logs`. Copy whole `.cx3` folders. After that they show up in **Load Patient**. Old patients can also still be opened directly from their old location with **Load Patient**, without copying anything.
- **Settings:** copy the contents of the old `~/CustusX_settings` folder (e.g. `profiles`, `settings`) into the new `~/CustusX/CustusX_settings` folder, replacing what v26.09 created there. If the old folder has a `virtualEnvironments` folder, don't copy it: Python environments stop working once moved, and they now live in `~/CustusX/virtualEnvironments`, where they are set up again when needed.

## Ubuntu 20.04 is no longer supported

([CustusX#51](https://gitlab.sintef.no/custusx/custusx/-/issues/51)) Starting with v26.09, no CustusX installer is provided for Ubuntu 20.04, and the Ubuntu install script stops with an error on 20.04. Ubuntu 20.04 has reached its end of standard support, and the Python 3.10 that the AI/segmentation tools need can no longer be installed there. Please use Ubuntu 22.04 or 24.04.

## New Features

| Key | Summary | Release Note |
|---|---|---|
| [CustusX#45](https://gitlab.sintef.no/custusx/custusx/-/issues/45) | Liver segmentation | Added a new Liver plugin using TotalSegmentator to segment the liver, pancreas, liver vessels, liver lesions and the 8 Couinaud liver segments from CT/MR, with a combined widget to run and toggle visibility of each structure. Segmentation runs in the background so the UI no longer freezes while it works. |
| [CustusX#44](https://gitlab.sintef.no/custusx/custusx/-/issues/44) | Single-file Ubuntu installer | Added a single-file Ubuntu installer script (`installCustusX.sh`) that downloads and sets up CustusX automatically, plus a `curl` one-liner that always fetches the latest release. Also added a desktop launcher. |

## Bugfixes

| Key | Summary | Release Note |
|---|---|---|
| [CustusX#46](https://gitlab.sintef.no/custusx/custusx/-/issues/46) | Missing glew dependency | The Ubuntu install script now installs `libglew-dev` and other missing dependencies, fixing a "missing glew library" error on a fresh install. |
| [CustusX#46](https://gitlab.sintef.no/custusx/custusx/-/issues/46) | False "DICOM data was not found" warning | This warning no longer appears incorrectly when importing a non-DICOM file (e.g. an mhd file). |
| [CustusX#46](https://gitlab.sintef.no/custusx/custusx/-/issues/46) | DICOM import false rejection | Relaxed gantry tilt tolerance during DICOM import so valid series with a near-zero tilt are no longer incorrectly rejected. |
| [CustusX#46](https://gitlab.sintef.no/custusx/custusx/-/issues/46) | Scripted filter reliability | Fixed a scripted filter (e.g. TotalSegmentator-based filters) sometimes reporting Stop as success, a race condition, and a rare freeze when finishing. |
| [CustusX#46](https://gitlab.sintef.no/custusx/custusx/-/issues/46) | Unresponsive during DICOM import | Importing DICOM data (e.g. from a USB drive) no longer makes CustusX appear "not responding" during long scans; the progress dialog's Cancel button now also works while a folder is being scanned. |
| [CustusX#46](https://gitlab.sintef.no/custusx/custusx/-/issues/46) | Scrolling a view only worked over its scrollbar | Scrolling a view now works from anywhere inside it, not just directly on the scrollbar. Previously a tabbed widget's tab bar was especially disruptive here (e.g. the Liver widget), since it sits above all of that widget's content and could block scrolling anywhere inside it. Scrolling over a dropdown/spin box/slider/tab bar still changes its value (or switches tabs) as before once you've clicked into it. |
| [CustusX#46](https://gitlab.sintef.no/custusx/custusx/-/issues/46) | Load Patient opened the wrong folder | The "Load Patient" dialog now correctly opens the current `~/CustusX/Patients` folder for anyone using an old CustusX_settings, instead of getting stuck on the old `~/Patients` location. |
| [CustusX#46](https://gitlab.sintef.no/custusx/custusx/-/issues/46) | Setting up TotalSegmentator failed on Ubuntu 22.04 | The first run of a TotalSegmentator-based feature (the Liver plugin, or the lung lobes/vessels/nodules filters) could fail while setting up its Python environment on Ubuntu 22.04, because a newer version of one of TotalSegmentator's dependencies (dipy) had to be built from source there. That dependency is now pinned to a version that installs without building. |

## Known Issues

| Key | Summary | Release Note |
|---|---|---|
| [CustusX#30](https://gitlab.sintef.no/custusx/custusx/-/issues/30) | Tracking on Ubuntu 24.04 | Tracking hardware support is not available in the open-source CustusX build on Ubuntu 24.04. |
| [CustusX#42](https://gitlab.sintef.no/custusx/custusx/-/issues/42) | Windows testing coverage | The Windows build has not been as thoroughly tested and verified as the Ubuntu builds. Ubuntu remains the primary, best-tested platform. |

<details>
<summary>Internal changes not included above (CI/build/tooling)</summary>

- CustusX#43 — Windows build object-file paths were close to Windows' 260-char MAX_PATH limit; shortened the default Windows release build root to restore margin
- CustusX#46 — Various Python release-tooling fixes and test coverage (git sync robustness, release tagging, CI); CI now flags failing unit tests instead of silently going green, and `syncToGitRef()` no longer fails merging on a reused CI runner with no git identity configured; installed macOS app bundles (Intel and Apple Silicon) no longer crash at startup from failing to find their own plugins in `Contents/Frameworks`
- CustusX#50 — Reworked external-libs CI caching into independently-published, per-library packages instead of one all-or-nothing combined package, with cleanup of orphaned/old packages and separated plain/igstk cache keys to fix a wrong-VTK_DIR CMake error

</details>
