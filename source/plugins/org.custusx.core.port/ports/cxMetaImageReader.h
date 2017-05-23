#ifndef CXMETAIMAGEREADER_H
#define CXMETAIMAGEREADER_H

#include "cxPortService.h"
#include <QFileInfo>

class ctkPluginContext;

namespace cx
{

class MetaImageReader : public PortService
{
public:
	Q_INTERFACES(cx::PortService)

	MetaImageReader(ctkPluginContext *context);
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