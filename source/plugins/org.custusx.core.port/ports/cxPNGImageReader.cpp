#include "cxPNGImageReader.h"

#include <QFileInfo>
#include <vtkPNGReader.h>
#include <vtkImageData.h>
#include <ctkPluginContext.h>
#include "cxImage.h"

typedef vtkSmartPointer<class vtkPNGReader> vtkPNGReaderPtr;
namespace cx
{

PNGImageReader::PNGImageReader(ctkPluginContext *context)
{
	this->setObjectName("PNGImageReader");
}

bool PNGImageReader::canLoad(const QString &type, const QString &filename)
{
	QString fileType = QFileInfo(filename).suffix();
	return (fileType.compare("png", Qt::CaseInsensitive) == 0);
}

bool PNGImageReader::readInto(DataPtr data, QString filename)
{
	return this->readInto(boost::dynamic_pointer_cast<Image>(data), filename);
}

bool PNGImageReader::readInto(ImagePtr image, QString filename)
{
	if (!image)
		return false;
	vtkImageDataPtr raw = this->loadVtkImageData(filename);
	if(!raw)
		return false;
	image->setVtkImageData(raw);
	return true;
}

DataPtr PNGImageReader::load(const QString& uid, const QString& filename)
{
	ImagePtr image(new Image(uid, vtkImageDataPtr()));
	this->readInto(image, filename);
	return image;
}

vtkImageDataPtr PNGImageReader::loadVtkImageData(QString filename)
{
	vtkPNGReaderPtr pngReader = vtkPNGReaderPtr::New();
	pngReader->SetFileName(filename.toStdString().c_str());
	pngReader->Update();
	return pngReader->GetOutput();
}

}
