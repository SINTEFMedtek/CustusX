#ifndef CXMETAIMAGEREADER_H
#define CXMETAIMAGEREADER_H

#include "cxPortService.h"
#include "org_custusx_core_port_Export.h"
#include <QFileInfo>

class ctkPluginContext;

namespace cx
{

class org_custusx_core_port_EXPORT MetaImageReader : public FileReaderWriterService
{
public:
	Q_INTERFACES(cx::FileReaderWriterService)

	MetaImageReader();
	virtual ~MetaImageReader(){}
	bool isNull();

	virtual bool canLoad(const QString& type, const QString& filename);
	virtual QString canLoadDataType() const { return "image"; }
	virtual bool readInto(DataPtr data, QString path);
	virtual bool readInto(ImagePtr image, QString filename);
	virtual DataPtr load(const QString& uid, const QString& filename);
	virtual vtkImageDataPtr loadVtkImageData(QString filename);
	virtual void saveImage(ImagePtr image, const QString& filename);

};

}
#endif // CXMETAIMAGEREADER_H