/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTSPACELISTENERMOCK_H
#define CXTESTSPACELISTENERMOCK_H

#include "cxSpaceListener.h"

namespace cxtest
{
typedef boost::shared_ptr<class SpaceListenerMock> SpaceListenerMockPtr;

class SpaceListenerMock: public cx::SpaceListener
{
public:
	static SpaceListenerMockPtr create() { return SpaceListenerMockPtr(new SpaceListenerMock()); }
	SpaceListenerMock() {}
	virtual ~SpaceListenerMock() {}
	void setSpace(cx::CoordinateSystem space) { mSpace = space; }
	cx::CoordinateSystem getSpace() const { return mSpace; }
private:
	cx::CoordinateSystem mSpace;
};

} // namespace cxtest


#endif // CXTESTSPACELISTENERMOCK_H
