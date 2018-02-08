Supported Platforms {#supported_platforms}
===================

CustusX is built continously on the following platforms:

| Name                      | Description
| ------------------------- | -----------------------------
| MacOSX 10.11 (El Capitan) | Mac Pro
| Ubuntu 16.04              | NVidia GPU<sup>1</sup>
| Windows 10                | NVidia GPU

In addition, CustusX has been known to work on

| Name                    | Description
| ----------------------- | -----------------------------
| MacOSX 10.9             | Problems detected when using NVidia cards (GPU-3D-rendering only)
| Fedora 21               | Not tested lately.
| Windows 7               | Not tested lately.
| Windows 8               | Not tested lately.
| Windows 10              | Only the development branch<sup>2</sup>
| MacOSX 10.12 (Sierra)   | Only the development branch<sup>2</sup>
| Ubuntu 16.04            | Only the development branch<sup>2</sup>

<sup>1</sup> Linux NVIDIA driver 384.111 seems to cause problems. NVIDIA driver 375.82 works ok.

<sup>2</sup> Checkout the development branch before running the cxInstaller script:

    git checkout develop
