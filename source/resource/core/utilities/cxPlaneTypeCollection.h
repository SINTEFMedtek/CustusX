/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXPLANETYPECOLLECTION_H
#define CXPLANETYPECOLLECTION_H

#include "cxResourceExport.h"

#include "cxDefinitions.h"
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
class cxResource_EXPORT PlaneTypeCollection
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
