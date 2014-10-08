#ifndef CXPRECOMPILEDHEADER_H_
#define CXPRECOMPILEDHEADER_H_

#ifdef _WIN32

//Qt
#include <QObject>
#include <QString>
#include <QtGlobal>

//boost
#ifdef BOOST_BUILD_PCH_ENABLED
#include "pch.hpp"
#endif

//VTK
#include <vtkSmartPointer.h>

//cx
//#include "cxMathBase.h"
//#include "cxVector3D.h"

#endif

#endif //CXPRECOMPILEDHEADER_H_
