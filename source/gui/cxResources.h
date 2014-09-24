
#ifndef CXRESOURCES_H_
#define CXRESOURCES_H_

#ifdef CX_WINDOWS

#include "cxGuiExport.h"

//This is a hack on windows.
//For some reason Q_INIT_RESOURCE(cxResources)
//does not make these symbols visual to the linker
cxGui_EXPORT int __cdecl qInitResources_cxResources();

#endif //
#endif //CXRESOURCES_H_
