// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include <sscDataMetric.h>
#include "sscRegistrationTransform.h"

namespace ssc
{

DataMetric::DataMetric(const QString& uid, const QString& name) :
				Data(uid, name)
{
	m_rMd_History = ssc::RegistrationHistory::getNullObject();
}

DataMetric::~DataMetric()
{
}

QString DataMetric::getSpace()
{
	return "";
}

}
