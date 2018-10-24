/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILEREADERWRITERSERVICE_H
#define CXFILEREADERWRITERSERVICE_H

#include <QObject>
#include "cxResourceExport.h"
#include "boost/shared_ptr.hpp"
#include "cxData.h"

#define FileReaderWriterService_iid "cx::FileReaderWriterService"

class ctkPluginContext;

namespace cx
{

typedef boost::shared_ptr<class FileReaderWriterService> FileReaderWriterServicePtr;


class cxResource_EXPORT FileReaderWriterService : public QObject
{
	Q_OBJECT
public:
	static FileReaderWriterServicePtr getNullObject();

	virtual ~FileReaderWriterService() {}
	virtual bool isNull() = 0;

	virtual QString getName() const = 0;
	virtual QString getFileSuffix() const = 0;

	//reading
	virtual QString canReadDataType() const = 0;
	virtual bool canRead(const QString& type, const QString& filename) = 0;

	virtual std::vector<DataPtr> read(const QString& filename) = 0;

	//--- TODO REMOVE:
	virtual DataPtr read(const QString& uid, const QString& filename) = 0;
	virtual bool readInto(DataPtr data, QString path) = 0;
	//---

	//writing
	virtual QString canWriteDataType() const = 0;
	virtual bool canWrite(const QString& type, const QString& filename) const = 0;
	virtual void write(DataPtr data, const QString& filename) = 0;

	//utility
	virtual vtkImageDataPtr loadVtkImageData(QString filename) = 0;
	virtual vtkPolyDataPtr loadVtkPolyData(QString filename) = 0;

	//TODO add advanced options
};
}//cx

Q_DECLARE_INTERFACE(cx::FileReaderWriterService, FileReaderWriterService_iid)
//------------

namespace cx
{
class cxResource_EXPORT FileReaderWriterImplService : public FileReaderWriterService
{
public:
	Q_INTERFACES(cx::FileReaderWriterService)

	FileReaderWriterImplService(QString name, QString canReadDataType, QString canWriteDataType, QString fileSuffix, PatientModelServicePtr patientModelService);

	virtual QString getName() const;
	virtual QString getFileSuffix() const;

	virtual vtkImageDataPtr loadVtkImageData(QString filename) { return vtkImageDataPtr(); }
	virtual vtkPolyDataPtr loadVtkPolyData(QString filename) { return vtkPolyDataPtr(); }

protected:
	bool canReadInternal(const QString& type, const QString& filename) const;
	bool canWriteInternal(const QString& type, const QString& filename) const;

	DataPtr createData(QString type, QString filename, QString name = "") const;

private:
	PatientModelServicePtr mPatientModelService;

	QString mName;
	QString mCanReadDataType; //TODO: convert to enum?
	QString mCanWriteDataType; //TODO: convert to enum?
	QString mFileSuffix;


	//cxEnumConverter.h
	//cxDefinitions.h
	//cxDefinitionStrings.h
};

} //cx


#endif // CXFILEREADERWRITERSERVICE_H
