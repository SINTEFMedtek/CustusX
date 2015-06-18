/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
	 this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
	 this list of conditions and the following disclaimer in the documentation
	 and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
	 may be used to endorse or promote products derived from this software
	 without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXSELECTDATASTRINGPROPERTYBASE_H_
#define CXSELECTDATASTRINGPROPERTYBASE_H_

#include "cxResourceExport.h"
#include "cxStringPropertyBase.h"
#include "cxForwardDeclarations.h"

namespace cx
{

typedef boost::shared_ptr<class SelectDataStringPropertyBase> SelectDataStringPropertyBasePtr;
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;

/** Base class for all Properties that selects a Data or descendants.
 *
 *  \ingroup cx_resource_core_properties
 *  \date 2014-09-11
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT SelectDataStringPropertyBase : public StringPropertyBase
{
	Q_OBJECT
public:
	virtual ~SelectDataStringPropertyBase() {}

public: // basic methods
	virtual QString getDisplayName() const;

public: // optional methods
	virtual QStringList getValueRange() const;
	virtual QString convertInternal2Display(QString internal);
	virtual QString getHelp() const;

public: // interface extension
	virtual DataPtr getData() const;
	virtual void setValueName(const QString name);
	virtual void setHelp(QString text);
	virtual void setUidRegexp(QString regexp);

signals:
	void dataChanged(QString);
protected:
	/** Construct base with a filter that determined allowed Data types based
		* on their getType() return value. The default of ".*" means any type.
		*/
	explicit SelectDataStringPropertyBase(PatientModelServicePtr patientModelService, QString typeRegexp = ".*");
	std::map<QString, DataPtr> filterOnType(std::map<QString, DataPtr> input, QString regexp) const;
	std::map<QString, DataPtr> filterOnUid(std::map<QString, DataPtr> input, QString regexp) const;
	QString mTypeRegexp;
	QString mUidRegexp;
	QString mValueName;
	QString mHelp;

	PatientModelServicePtr mPatientModelService;
};

} // cx
#endif // CXSELECTDATASTRINGPROPERTYBASE_H_
