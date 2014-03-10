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
#ifndef CXDATAFACTORY_H
#define CXDATAFACTORY_H

#include "cxForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;
class DataManager;


/** Factory for creating cx::Data objects
 *
 *
 * \ingroup sscData
 * \date 2014-02-22
 * \author christiana
 */
class DataFactory
{
public:
	explicit DataFactory(DataServicePtr dataManager, SpaceProviderPtr spaceProvider);

	DataPtr create(QString type, QString uid, QString name="");

	template<class T>
	boost::shared_ptr<T> createSpecific(QString uid, QString name="")
	{
		DataPtr retval = this->create(T::getTypeName(), uid, name);
		return boost::dynamic_pointer_cast<T>(retval);
	}
private:
	DataServicePtr mDataManager;
	SpaceProviderPtr mSpaceProvider;
};
typedef boost::shared_ptr<DataFactory> DataFactoryPtr;


} // namespace cx


#endif // CXDATAFACTORY_H
