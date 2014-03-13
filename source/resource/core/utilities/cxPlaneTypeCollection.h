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
#ifndef CXPLANETYPECOLLECTION_H
#define CXPLANETYPECOLLECTION_H

#include "sscDefinitions.h"
#include <vector>
#include <QStringList>

namespace cx
{

/** A list of PLANE_TYPE, along with utility methods
 *
 * \ingroup cx_resource_core_utilities
 * \date 2014-03-04
 * \author Christian Askeland, SINTEF
 */
class PlaneTypeCollection
{
public:

	explicit PlaneTypeCollection(std::vector<PLANE_TYPE> planes);
	PlaneTypeCollection(PLANE_TYPE a=ptCOUNT, PLANE_TYPE b=ptCOUNT, PLANE_TYPE c=ptCOUNT);
	static PlaneTypeCollection fromString(QString input, PlaneTypeCollection defVal = PlaneTypeCollection());

	void add(PLANE_TYPE plane);
	std::vector<PLANE_TYPE> get() const;
	QString toString() const;

private:
	std::vector<PLANE_TYPE> mPlanes;
};



} // namespace cx

#endif // CXPLANETYPECOLLECTION_H
