/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	virtual DataPtr getData(QString uid) const;
	virtual void setValueName(const QString name);
	virtual void setHelp(QString text);
	virtual void setUidRegexp(QString regexp);
	static std::map<QString, DataPtr> filterOnType(std::map<QString, DataPtr> input, QString regexp);
	virtual void setTypeRegexp(QString regexp);

	void setOnly2DImagesFilter(bool only2D);
signals:
	void dataChanged(QString);
protected:
	/** Construct base with a filter that determined allowed Data types based
		* on their getType() return value. The default of ".*" means any type.
		*/
	explicit SelectDataStringPropertyBase(PatientModelServicePtr patientModelService, QString typeRegexp = ".*");
	std::map<QString, DataPtr> filterOnUid(std::map<QString, DataPtr> input, QString regexp) const;
	std::map<QString, DataPtr> filterImagesOn2D(std::map<QString, DataPtr> input, bool only2D) const;
	QString mTypeRegexp;
	QString mUidRegexp;
	bool mOnly2D;
	QString mValueName;
	QString mHelp;

	PatientModelServicePtr mPatientModelService;
};

} // cx
#endif // CXSELECTDATASTRINGPROPERTYBASE_H_
