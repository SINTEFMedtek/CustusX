#ifndef CXPNGIMAGEREADER_H
#define CXPNGIMAGEREADER_H

#include "cxPortService.h"
#include "org_custusx_core_port_Export.h"

namespace cx {

/**\brief Reader for portable network graphics .png files.
 *
 */
class org_custusx_core_port_EXPORT PNGImageReader: public FileReaderWriterService
{
public:
	Q_INTERFACES(cx::FileReaderWriterService)

	PNGImageReader();
	virtual ~PNGImageReader() {}
	virtual bool canLoad(const QString& type, const QString& filename);
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(ImagePtr image, QString filename);
	virtual QString canLoadDataType() const { return "image"; }
	virtual DataPtr load(const QString& uid, const QString& filename);
	virtual vtkImageDataPtr loadVtkImageData(QString filename);

	bool isNull(){return false;}
};

}

#endif // CXPNGIMAGEREADER_H