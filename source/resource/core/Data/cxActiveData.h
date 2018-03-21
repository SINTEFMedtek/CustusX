/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXACTIVEDATA_H
#define CXACTIVEDATA_H

#include "cxResourceExport.h"
#include <QObject>
#include <QList>
#include "boost/shared_ptr.hpp"
#include "cxSessionStorageService.h"

namespace cx
{
typedef boost::shared_ptr<class ActiveData> ActiveDataPtr;
typedef boost::shared_ptr<class Data> DataPtr;
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class PatientStorage> PatientStoragePtr;

/** \brief Provides the last active data of warious types
 *
 * \ingroup cx_resource_core_data
 *
 * \date seb 28, 2015
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT ActiveData : public QObject
{
	Q_OBJECT
public:
	ActiveData(PatientModelServicePtr patientModelService, SessionStorageServicePtr sessionStorageService);
	virtual QList<DataPtr> getActiveDataHistory() const;
	virtual DataPtr getActive() const;
	virtual DataPtr getActiveUsingRegexp(QString typeRegexp) const;
	template <class DATA>
	boost::shared_ptr<DATA> getActive() const;
	virtual ImagePtr getDerivedActiveImage() const;///< In addition to returning Image this also provides derived (changing) images from TrackedStream
	virtual void setActive(DataPtr activeData);
	virtual void setActive(QString uid);
	virtual QString getActiveImageUid();
	virtual void remove(DataPtr dataToBeRemoved);

	static ActiveDataPtr getNullObject();
	virtual bool isNull() const { return false;}
signals:
	void activeImageChanged(const QString& uId);
	void activeDataChanged(const QString& uId);

private:
	QList<DataPtr> mActiveData;
	PatientStoragePtr mStorage;
	PatientModelServicePtr mPatientModelService;
	static ActiveDataPtr mNull;

	void emitSignals(DataPtr activeData);
	void emitActiveDataChanged();
	void emitActiveImageChanged();
	QString getChangedUid(DataPtr activeData) const;
	QList<DataPtr> getActiveDataHistory(QString typeRegexp) const;
	QString getStringToSave() const;
	QStringList getActiveDataStringList() const;
	void loadFromString(const QString activeDatas);
};

template <class DATA>
boost::shared_ptr<DATA> ActiveData::getActive() const
{
	boost::shared_ptr<DATA> retval;
	QList<DataPtr> activeDataList = this->getActiveDataHistory();
	for(int i = activeDataList.size() - 1; i >= 0; --i)
	{
		retval = boost::dynamic_pointer_cast<DATA>(activeDataList.at(i));
		if(retval)
			return retval;
	}
	return retval;
}


} //cx

#endif // CXACTIVEDATA_H
