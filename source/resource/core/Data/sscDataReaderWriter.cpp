// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscDataReaderWriter.h"

#include <sstream>
#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkSmartPointer.h>
#include <vtkMetaImageWriter.h>

#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkSTLReader.h>
#include <vtkImageChangeInformation.h>
#include "vtkMINCImageReader.h"
#include "vtkTransform.h"
#include "vtkCommand.h"
#include <vtkPNGReader.h>

#include <QtCore>
#include <QDomDocument>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include "sscTransform3D.h"
#include "sscRegistrationTransform.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscUtilHelpers.h"
#include "sscVideoSource.h"
#include "sscCustomMetaImage.h"

//#include "sscPointMetric.h"
//#include "sscDistanceMetric.h"
//#include "sscPlaneMetric.h"
//#include "sscAngleMetric.h"
//#include "cxShapedMetric.h"
//#include "cxSphereMetric.h"
//#include "cxFrameMetric.h"
//#include "cxToolMetric.h"

#include "sscImageLUT2D.h"
#include "sscImageTF3D.h"
#include "sscLogger.h"

typedef vtkSmartPointer<class vtkPNGReader> vtkPNGReaderPtr;

namespace cx
{

//---------------------------------------------------------
StaticMutexVtkLocker::StaticMutexVtkLocker()
{
/*	if (!mMutex)
		mMutex.reset(new QMutex(QMutex::Recursive));

	mMutex->lock();*/
}
StaticMutexVtkLocker::~StaticMutexVtkLocker()
{
//	mMutex->unlock();
}
boost::shared_ptr<QMutex> StaticMutexVtkLocker::mMutex;
//---------------------------------------------------------

bool MincImageReader::readInto(DataPtr data, QString filename)
{
	return this->readInto(boost::dynamic_pointer_cast<Image>(data), filename);
}

bool MincImageReader::readInto(ImagePtr image, QString filename)
{
	vtkImageDataPtr raw = this->loadVtkImageData(filename);
	if(!raw)
		return false;

	//Read data input file
	vtkMINCImageReaderPtr l_dataReader = vtkMINCImageReaderPtr::New();
	l_dataReader->SetFileName(cstring_cast(filename));
	l_dataReader->Update();

	double l_dataOrigin[3];
	l_dataReader->GetOutput()->GetOrigin(l_dataOrigin);
	int l_dimensions[3];
	l_dataReader->GetOutput()->GetDimensions(l_dimensions);

	//set the transform
	vtkTransformPtr l_dataTransform = vtkTransformPtr::New();
	l_dataTransform->SetMatrix(l_dataReader->GetDirectionCosines());
	l_dataTransform->Translate(l_dataReader->GetDataOrigin());
	//l_dataTransform->GetInverse()->TransformPoint(l_dataOrigin, l_dataOrigin);
	//l_dataTransform->Translate(l_dataOrigin);
	//l_dataTransform->Scale(l_dataReader->GetOutput()->GetSpacing());

	Transform3D rMd(l_dataTransform->GetMatrix());

	// TODO: ensure rMd is correct in CustusX terms

	vtkImageChangeInformationPtr zeroer = vtkImageChangeInformationPtr::New();
	zeroer->SetInput(l_dataReader->GetOutput());
	zeroer->SetOutputOrigin(0, 0, 0);
	zeroer->Update();
	//  vtkImageDataPtr imageData = zeroer->GetOutput();
	vtkImageDataPtr imageData = zeroer->GetOutput();

	QFile file(filename);
	QFileInfo info(file);
	//QString uid(info.completeBaseName()+"_minc_%1");
//	QString name = uid;

//	ImagePtr image(new Image(uid, imageData));
	image->setVtkImageData(imageData);
	//ImagePtr image = dataManager()->createImage(l_dataReader->GetOutput(),uid, name);
	image->get_rMd_History()->addRegistration(RegistrationTransform(rMd, info.lastModified(), "from Minc file"));
	image->getBaseVtkImageData()->Print(std::cout);

	return true;
}

//-----
DataPtr MincImageReader::load(const QString& uid, const QString& filename)
{
	ImagePtr image(new Image(uid, vtkImageDataPtr()));
	this->readInto(image, filename);
	return image;
}

/** Wrapper for vtkAlgorithm::Update(),
  * prints error message upon error,
  * also wraps the call inside a global mutex (see below for why).
  *
  * \ingroup sscData
  * \date jan 1, 2010
  * \date april 17, 2013
  * \author christiana
  */
class ErrorObserver: public vtkCommand
{
public:
	ErrorObserver()
	{
	}
	static ErrorObserver* New()
	{
		return new ErrorObserver;
	}
	virtual void Execute(vtkObject* caller, unsigned long, void* text)
	{
		mMessage = QString(reinterpret_cast<char*> (text));
	}
	QString mMessage;

	static bool checkedRead(vtkSmartPointer<vtkAlgorithm> reader, QString filename)
	{
		vtkSmartPointer<ErrorObserver> errorObserver = vtkSmartPointer<ErrorObserver>::New();
		reader->AddObserver("ErrorEvent", errorObserver);

		{
			StaticMutexVtkLocker lock;
			reader->Update();
		}
//		ErrorObserver::threadSafeUpdate(reader);

		if (!errorObserver->mMessage.isEmpty())
		{
			messageManager()->sendError("Load of data " + filename + " failed with message:\n"
				+ errorObserver->mMessage);
			return false;
		}
		return true;
	}
};

//-----
vtkImageDataPtr MetaImageReader::loadVtkImageData(QString filename)
{
	//load the image from file
	vtkMetaImageReaderPtr reader = vtkMetaImageReaderPtr::New();
	reader->SetFileName(cstring_cast(filename));
	reader->ReleaseDataFlagOn();

	if (!ErrorObserver::checkedRead(reader, filename))
		return vtkImageDataPtr();

	vtkImageChangeInformationPtr zeroer = vtkImageChangeInformationPtr::New();
	zeroer->SetInput(reader->GetOutput());
	zeroer->SetOutputOrigin(0, 0, 0);
	zeroer->Update();
	return zeroer->GetOutput();
}

bool MetaImageReader::readInto(DataPtr data, QString filename)
{
	return this->readInto(boost::dynamic_pointer_cast<Image>(data), filename);
}
bool MetaImageReader::readInto(ImagePtr image, QString filename)
{
	if (!image)
		return false;

	CustomMetaImagePtr customReader = CustomMetaImage::create(filename);
	Transform3D rMd = customReader->readTransform();

	vtkImageDataPtr raw = this->loadVtkImageData(filename);
	if(!raw)
		return false;

	image->setVtkImageData(raw);
//	ImagePtr image(new Image(uid, raw));

	//  RegistrationTransform regTrans(rMd, QFileInfo(filename).lastModified(), "From MHD file");
	//  image->get_rMd_History()->addRegistration(regTrans);
	image->get_rMd_History()->setRegistration(rMd);
	image->setModality(customReader->readModality());
	image->setImageType(customReader->readImageType());

	bool ok1 = true;
	bool ok2 = true;
	double level = customReader->readKey("WindowLevel").toDouble(&ok1);
	double window = customReader->readKey("WindowWidth").toDouble(&ok2);

	if (ok1 && ok2)
	{
		image->setInitialWindowLevel(window, level);
		image->resetTransferFunctions();
	}

	return true;
}

//-----
DataPtr MetaImageReader::load(const QString& uid, const QString& filename)
{
	ImagePtr image(new Image(uid, vtkImageDataPtr()));
	this->readInto(image, filename);
	return image;
}

void MetaImageReader::saveImage(ImagePtr image, const QString& filename)
{
	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
	writer->SetInput(image->getBaseVtkImageData());
	writer->SetFileDimensionality(3);
	writer->SetFileName(cstring_cast(filename));
	QDir().mkpath(QFileInfo(filename).path());
//	std::cout << "saving to " << filename << std::endl;

	writer->SetCompression(false);
	writer->Update();
	writer->Write();

	writer = 0;

	CustomMetaImagePtr customReader = CustomMetaImage::create(filename);
	customReader->setTransform(image->get_rMd());
	customReader->setModality(image->getModality());
	customReader->setImageType(image->getImageType());
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

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

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

bool PolyDataMeshReader::readInto(DataPtr data, QString filename)
{
	return this->readInto(boost::dynamic_pointer_cast<Mesh>(data), filename);
}

bool PolyDataMeshReader::readInto(MeshPtr mesh, QString filename)
{
	if (!mesh)
		return false;
	vtkPolyDataPtr raw = this->loadVtkPolyData(filename);
	if(!raw)
		return false;
	mesh->setVtkPolyData(raw);
	return true;
}
//-----
vtkPolyDataPtr PolyDataMeshReader::loadVtkPolyData(QString fileName)
{
	vtkPolyDataReaderPtr reader = vtkPolyDataReaderPtr::New();
	reader->SetFileName(cstring_cast(fileName));

	if (!ErrorObserver::checkedRead(reader, fileName))
		return vtkPolyDataPtr();

	vtkPolyDataPtr polyData = reader->GetOutput();
	return polyData;
}

DataPtr PolyDataMeshReader::load(const QString& uid, const QString& filename)
{
	MeshPtr mesh(new Mesh(uid));
	this->readInto(mesh, filename);
	return mesh;
}

bool StlMeshReader::readInto(DataPtr data, QString filename)
{
	return this->readInto(boost::dynamic_pointer_cast<Mesh>(data), filename);
}

bool StlMeshReader::readInto(MeshPtr mesh, QString filename)
{
	if (!mesh)
		return false;
	vtkPolyDataPtr raw = this->loadVtkPolyData(filename);
	if(!raw)
		return false;
	mesh->setVtkPolyData(raw);
	return true;
}


vtkPolyDataPtr StlMeshReader::loadVtkPolyData(QString fileName)
{
	vtkSTLReaderPtr reader = vtkSTLReaderPtr::New();
	reader->SetFileName(cstring_cast(fileName));

	if (!ErrorObserver::checkedRead(reader, fileName))
		return vtkPolyDataPtr();

	vtkPolyDataPtr polyData = reader->GetOutput();
	return polyData;
}

DataPtr StlMeshReader::load(const QString& uid, const QString& filename)
{
	MeshPtr mesh(new Mesh(uid));
	this->readInto(mesh, filename);
	return mesh;
}

DataReaderWriter::DataReaderWriter()
{
	mDataReaders.insert(DataReaderPtr(new MetaImageReader()));
	mDataReaders.insert(DataReaderPtr(new MincImageReader()));
	mDataReaders.insert(DataReaderPtr(new PolyDataMeshReader()));
	mDataReaders.insert(DataReaderPtr(new StlMeshReader()));
	mDataReaders.insert(DataReaderPtr(new PNGImageReader()));
}

DataReaderPtr DataReaderWriter::findReader(const QString& path, const QString& type)
{
	for (DataReadersType::iterator iter = mDataReaders.begin(); iter != mDataReaders.end(); ++iter)
	{
		if ((*iter)->canLoad(type, path))
			return *iter;
	}
	return DataReaderPtr();
}

vtkImageDataPtr DataReaderWriter::loadVtkImageData(QString filename)
{
	DataReaderPtr reader = this->findReader(filename);
	if (reader)
		return reader->loadVtkImageData(filename);
	return vtkImageDataPtr();
}

vtkPolyDataPtr DataReaderWriter::loadVtkPolyData(QString filename)
{
	DataReaderPtr reader = this->findReader(filename);
	if (reader)
		return reader->loadVtkPolyData(filename);
	return vtkPolyDataPtr();
}

QString DataReaderWriter::findDataTypeFromFile(QString filename)
{
	DataReaderPtr reader = this->findReader(filename);
	if (reader)
		return reader->canLoadDataType();
	return "";
}

void DataReaderWriter::readInto(DataPtr data, QString path)
{
	DataReaderPtr reader = this->findReader(path, data->getType());
	if (reader)
		reader->readInto(data, path);

	if(data)
	{
		QFileInfo fileInfo(qstring_cast(path));
		data->setName(changeExtension(fileInfo.fileName(), ""));
		data->setFilename(path); // need path even when not set explicitly: nice for testing
	}

}

} // namespace cx


