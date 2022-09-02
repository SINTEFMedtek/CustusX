/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXDATAFACTORY_H
#define CXDATAFACTORY_H

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;


/** Factory for creating cx::Data objects
 *
 *
 * \ingroup cx_resource_core_data
 * \date 2014-02-22
 * \author christiana
 */
class cxResource_EXPORT DataFactory
{
public:
	explicit DataFactory(PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);

	DataPtr create(QString type, QString uid, QString name="");

	template<class T>
	boost::shared_ptr<T> createSpecific(QString uid, QString name="")
	{
		DataPtr retval = this->create(T::getTypeName(), uid, name);
		return boost::dynamic_pointer_cast<T>(retval);
	}
private:
	DataPtr createRaw(QString type, QString uid);

	PatientModelServicePtr mDataManager;
	SpaceProviderPtr mSpaceProvider;

};
typedef boost::shared_ptr<DataFactory> DataFactoryPtr;


} // namespace cx


#endif // CXDATAFACTORY_H
