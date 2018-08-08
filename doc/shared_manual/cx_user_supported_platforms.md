Supported Platforms {#supported_platforms}
===================

CustusX is built continously on the following platforms:

| Name                          | Description
| ----------------------------- | -----------------------------
| macOS 10.13 (High Sierra)     | Mac Pro<sup>1</sup>
| Ubuntu 16.04<sup>2</sup>      | NVidia GPU<sup>3</sup>
| Windows 10                    | NVidia GPU

In addition, CustusX has been known to work on

| Name                         | Description
| ---------------------------- | -----------------------------
| Windows 7                    | Not tested lately
| Windows 8                    | Not tested lately
| Ubuntu 14.04                 | Must be built from source
| MacOSX 10.11 (El Capitan)    | <sup>1</sup>
| macOS 10.12 (Sierra)         | <sup>1</sup>

<sup>1</sup> Macs with NVIDIA GeForce GT 650/750M seems to have issues with showing data in 2D views.
Turning off multi volume visualization in 2D is a way to fix this problem: Preferences → Performance → 2D overlay

<sup>2</sup> CustusX always uses .(period) for import and export.
Importing data from programs like ITK-SNAP may require locale to be set so that .(period) is used for decimal separator, not comma.
This may be an issue for importing data from other programs as well.
Setting local on Ubuntu 16.04: System Settings → Language Support → Regional Formats: Select English US, and apply System-Wide

<sup>3</sup> Linux NVIDIA driver 384.111 seems to cause problems. NVIDIA driver 375.82 works ok.
