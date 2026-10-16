/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// Replaces ndicapiExport.h.in from the ndicapi repository (normally generated
// by its CMakeLists.txt). CustusX builds ndicapi as a static library inside the
// NDI tracking plugin, so no DLL export/import decoration is needed.

#ifndef __ndicapiExport_h
#define __ndicapiExport_h

#define ndicapiExport

#endif
