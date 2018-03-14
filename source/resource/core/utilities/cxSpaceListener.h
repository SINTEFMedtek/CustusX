/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSPACELISTENER_H
#define CXSPACELISTENER_H

#include "cxResourceExport.h"

#include "cxCoordinateSystemHelpers.h"
#include <QObject>

namespace cx
{

/** \brief Class that listens to changes in a coordinate system,
 * and emits a signal if that system changes.
 *
 *
 * \ingroup cx_resource_core_utilities
 * \date 21.02.2014, 2014
 * \author christiana
 */
class cxResource_EXPORT SpaceListener: public QObject
{
Q_OBJECT

public:
	virtual ~SpaceListener() {}
	virtual void setSpace(CoordinateSystem space) = 0;
	virtual CoordinateSystem getSpace() const = 0;
signals:
	void changed();
};
typedef boost::shared_ptr<SpaceListener> SpaceListenerPtr;

} // namespace cx

#endif // CXSPACELISTENER_H
