Supported Platforms {#supported_platforms}
===================

CustusX is built continously on the following platforms:

| Name                          | Description
| ----------------------------- | -----------------------------
| MacOSX 10.13 (El High Sierra) | Mac Pro<sup>1</sup>
| Ubuntu 16.04                  | NVidia GPU<sup>2</sup>
| Windows 10                    | NVidia GPU

In addition, CustusX has been known to work on

| Name                         | Description
| ---------------------------- | -----------------------------
| MacOSX 10.9                  | Problems detected when using NVidia cards (GPU-3D-rendering only)<sup>1</sup>
| Fedora 21                    | Not tested lately.
| Windows 7                    | Not tested lately.
| Windows 8                    | Not tested lately.
| Windows 10                   | Only the development branch<sup>3</sup>
| MacOSX 10.12 (Sierra)        | Only the development branch<sup>3</sup>
| MacOSX 10.13 (High Sierra)   | Only the development branch<sup>3</sup>
| Ubuntu 16.04                 | Only the development branch<sup>3</sup>

<sup>1</sup> Macs with with NVIDIA GeForce GT 750M seems to have issues with showing data in 2D views.
Turning off multi volume visualization is a way to fix this problem: Preferences -> Performance -> 2D overlay

<sup>2</sup> Linux NVIDIA driver 384.111 seems to cause problems. NVIDIA driver 375.82 works ok.

<sup>3</sup> Checkout the development branch before running the cxInstaller script:

    git checkout develop
