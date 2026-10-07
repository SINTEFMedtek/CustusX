ndicapi in CustusX
==================

A copy of the core C library of ndicapi, used by the NDI tracking plugin
(org.custusx.core.tracking.system.ndi) to communicate with NDI Aurora and
Polaris devices.

- Source: https://github.com/PlusToolkit/ndicapi
- Commit: 615ee27d06ddbc167745411946628bc5a5e246d5 (2026-09-08)
- Copied files: ndicapi.cxx/.h, ndicapi_math.*, ndicapi_serial*.*, ndicapi_socket*.*,
  ndicapi_thread.*, LICENSE, ndicapi.txt. Not copied: the Python module, the
  applications and ndicapi's own CMake files (built by the plugin's CMakeLists.txt instead).
- ndicapiExport.h is written for CustusX (static build) and replaces ndicapiExport.h.in.
- Changed for CustusX: ndicapi_serial_unix.cxx also sets 921600 baud on Linux
  (CustusX#55). Keep this change when updating, unless ndicapi has added it.
- Update by copying the same files from a newer commit and updating the commit above.

Licenses
--------

- Most files: Copyright (c) 2000-2005 Atamai, Inc., BSD-style license, see the header
  of each file and ndicapi.txt.
- ndicapi_socket*: Copyright (c) Laboratory for Percutaneous Surgery (PLUS).
- The ndicapi repository as a whole: MIT license, see LICENSE.

ndicapi is an independent implementation of the NDI Combined API protocol published by
Northern Digital Inc. It does not contain NDI's own sample code, and is not supported by NDI.
