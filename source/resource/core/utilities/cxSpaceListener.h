// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXSPACELISTENER_H
#define CXSPACELISTENER_H

#include "sscCoordinateSystemHelpers.h"

namespace cx
{

/** \brief Class that listens to changes in a coordinate system,
 * and emits a signal if that system changes.
 *
 *
 * \date 21.02.2014, 2014
 * \author christiana
 */
class SpaceListener: public QObject
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
