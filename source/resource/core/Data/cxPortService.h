#ifndef CXPORTSERVICE_H
#define CXPORTSERVICE_H

#include <QObject>
#include "cxResourceExport.h"
#include "boost/shared_ptr.hpp"
#include "cxData.h"

#define PortService_iid "cx::PortService"

namespace cx
{

typedef boost::shared_ptr<class PortService> PortServicePtr;

//Same as old DataReader class

class cxResource_EXPORT PortService : public QObject
{
	Q_OBJECT
public:
	virtual ~PortService() {}

	virtual bool isNull() = 0;
	static PortServicePtr getNullObject();

	virtual bool canLoad(const QString& type, const QString& filename) = 0;
	virtual DataPtr load(const QString& uid, const QString& filename) = 0;
	virtual vtkImageDataPtr loadVtkImageData(QString filename) { return vtkImageDataPtr(); }
	virtual vtkPolyDataPtr loadVtkPolyData(QString filename) { return vtkPolyDataPtr(); }
	virtual QString canLoadDataType() const =0;
	virtual bool readInto(DataPtr data, QString path) = 0;

	//TODO Convert from Image to Data?
	virtual void saveImage(ImagePtr image, const QString& filename){}


	virtual void addPort(PortService *service) {}
	virtual void removePort(PortService *service) {}

};

}

Q_DECLARE_INTERFACE(cx::PortService, PortService_iid)

#endif // CXPORTSERVICE_H