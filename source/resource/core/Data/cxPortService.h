#ifndef CXPORTSERVICE_H
#define CXPORTSERVICE_H

#include <QObject>
#include "cxResourceExport.h"
#include "boost/shared_ptr.hpp"
#include "cxData.h"

#define FileReaderWriterService_iid "cx::FileReaderWriterService"

namespace cx
{

typedef boost::shared_ptr<class FileReaderWriterService> FileReaderWriterServicePtr;

//Same as old DataReader class

class cxResource_EXPORT FileReaderWriterService : public QObject
{
	Q_OBJECT
public:
	virtual ~FileReaderWriterService() {}

	virtual bool isNull() = 0;
	static FileReaderWriterServicePtr getNullObject();

	virtual bool canLoad(const QString& type, const QString& filename) = 0;
	virtual DataPtr load(const QString& uid, const QString& filename) = 0;
	virtual vtkImageDataPtr loadVtkImageData(QString filename) { return vtkImageDataPtr(); }
	virtual vtkPolyDataPtr loadVtkPolyData(QString filename) { return vtkPolyDataPtr(); }
	virtual QString canLoadDataType() const =0;
	virtual bool readInto(DataPtr data, QString path) = 0;
	virtual void save(DataPtr data, const QString& filename) = 0;
};

}

Q_DECLARE_INTERFACE(cx::FileReaderWriterService, FileReaderWriterService_iid)

#endif // CXPORTSERVICE_H