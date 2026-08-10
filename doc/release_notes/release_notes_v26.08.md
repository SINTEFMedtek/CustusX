# CustusX release notes — v26.08

_Changes since v22.09_

## New Features

| Key | Summary | Release Note |
|---|---|---|
| [CustusX#30](https://gitlab.sintef.no/custusx/custusx/-/issues/30) | Ubuntu 24.04 support | CustusX now builds and runs on Ubuntu 24.04 (in addition to Ubuntu 20.04 and 22.04). VTK is updated to 9.6.1 and ITK to 5.4.5. |
| [CustusX#33](https://gitlab.sintef.no/custusx/custusx/-/issues/33) | New registration and mesh filters | Added a Coherent Point Drift (CPD) registration filter and a ClipMeshByVolume mesh-clipping filter. |
| [Fraxinus#22](https://gitlab.sintef.no/custusx/fraxinus/-/issues/22) | Optional gray application style | Added an optional gray application style for all applications, activated with `-style gray` on the command line. |
| [CustusX#22](https://gitlab.sintef.no/custusx/custusx/-/issues/22) | Ubuntu 22.04 support | CustusX now builds and runs on Ubuntu 22.04. |

## Bugfixes

| Key | Summary | Release Note |
|---|---|---|
| [CustusX#37](https://gitlab.sintef.no/custusx/custusx/-/issues/37) | Close sockets in OpenIGTLinkIO | Fixed a socket data race and hang when stopping OpenIGTLink connections. |
| [CustusX#38](https://gitlab.sintef.no/custusx/custusx/-/issues/38) | Bug with setting data parent | Fixed a bug where changing a data object's parent space could silently fail to revert to a previous parent. |
| [CustusX#25](https://gitlab.sintef.no/custusx/custusx/-/issues/25) | Showing volume slices is no longer possible | Restored the "3D Slices" visualization (showing volume slices within a 3D scene) after it broke during the VTK 9 migration. |
| [CustusX#17](https://gitlab.sintef.no/custusx/custusx/-/issues/17) | Auto set LLR for ultrasound stream | Ultrasound streams now default to Low-Level-Reject 1, so zero-value pixels outside the sector no longer obscure the underlying image by default. |
| [CustusX#35](https://gitlab.sintef.no/custusx/custusx/-/issues/35) | Volume cropper no longer working | Fixed the volume cropper widget, which had stopped showing and working after the VTK 9.6.1 upgrade. |
| [CustusX#20](https://gitlab.sintef.no/custusx/custusx/-/issues/20) | GenericScriptFilter issues | Fixed script configuration file path handling for GenericScriptFilter in CustusS and Fraxinus. |
| [CustusX#3](https://gitlab.sintef.no/custusx/custusx/-/issues/3) | Opacity in 2D views after VTK version change | Fixed 2D view overlay rendering after the VTK 9 migration. |

## Known Issues

| Key | Summary | Release Note |
|---|---|---|
| [CustusX#30](https://gitlab.sintef.no/custusx/custusx/-/issues/30) | Tracking on Ubuntu 24.04 | Tracking hardware support is not available in the open-source CustusX build on Ubuntu 24.04. |

<details>
<summary>Internal changes not included above (CI/build/tooling)</summary>

- CustusX#39 — Update documentation
- CustusX#34 — Update CustusX web-pages
- CustusX#42 — 26.08 Release
- CustusX#16 — Check if Image2DProxy can be removed
- CustusX#36 — Configure external visibility
- CustusX#32 — Update python installer scripts for ubuntu24
- CustusX#29 — Use Claude
- Fraxinus#29 — Annotation of bronchoscopy videos
- CustusX#24 — Known issues with 25.02 release
- CustusX#23 — Failing test: Import Kaisa from DICOM
- CustusX#4 — Start using Gitlab CI

</details>
