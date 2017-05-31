#ifndef CXFILEMANAGERSERVICE_H
#define CXFILEMANAGERSERVICE_H

#include <QObject>
#include "cxResourceExport.h"
#include "boost/shared_ptr.hpp"
#include "cxData.h"

#define FileManagerService_iid "cx::FileManagerService"

namespace cx
{

typedef boost::shared_ptr<class FileManagerService> FileManagerServicePtr;


class cxResource_EXPORT FileManagerService : public QObject
{
	Q_OBJECT
public:
	virtual ~FileManagerService() {}

	virtual bool isNull() = 0;
	static FileManagerServicePtr getNullObject();

	virtual bool canLoad(const QString& type, const QString& filename) = 0;
	virtual DataPtr load(const QString& uid, const QString& filename) = 0;
	virtual vtkImageDataPtr loadVtkImageData(QString filename) = 0;
	virtual vtkPolyDataPtr loadVtkPolyData(QString filename) = 0;
	virtual QString canLoadDataType() const =0;
	virtual bool readInto(DataPtr data, QString path) = 0;
	virtual QString findDataTypeFromFile(QString filename) = 0;

	//TODO Convert from Image to Data?
	virtual void save(DataPtr data, const QString& filename) = 0;


	virtual void addFileReaderWriter(FileReaderWriterService *service) = 0;
	virtual void removeFileReaderWriter(FileReaderWriterService *service) = 0;

};

}

Q_DECLARE_INTERFACE(cx::FileManagerService, FileManagerService_iid)
#endif // CXFILEMANAGERSERVICE_H