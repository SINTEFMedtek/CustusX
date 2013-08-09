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

#include "sscPointMetric.h"
#include "sscDistanceMetric.h"
#include "sscPlaneMetric.h"
#include "sscAngleMetric.h"

#include "sscImageLUT2D.h"
#include "sscImageTF3D.h"
#include "sscLogger.h"

typedef vtkSmartPointer<class vtkPNGReader> vtkPNGReaderPtr;

namespace ssc
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


//-----
DataPtr MincImageReader::load(const QString& uid, const QString& filename)
{
	std::cout << "Reading " << filename << std::endl;

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

	ssc::Transform3D rMd(l_dataTransform->GetMatrix());

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
	QString name = uid;

	ImagePtr image(new Image(uid, imageData));
	//ssc::ImagePtr image = ssc::dataManager()->createImage(l_dataReader->GetOutput(),uid, name);
	image->get_rMd_History()->addRegistration(ssc::RegistrationTransform(rMd, info.lastModified(), "from Minc file"));
	image->getBaseVtkImageData()->Print(std::cout);

	return image;
	//////////////////////////////
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
			ssc::messageManager()->sendError("Load of data " + filename + " failed with message:\n"
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

//-----
DataPtr MetaImageReader::load(const QString& uid, const QString& filename)
{
	CustomMetaImagePtr customReader = CustomMetaImage::create(filename);
	Transform3D rMd = customReader->readTransform();

	vtkImageDataPtr raw = this->loadVtkImageData(filename);
	if(!raw)
		return DataPtr();

	ImagePtr image(new Image(uid, raw));

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
//		image->getTransferFunctions3D()->setLevel(level);
//		image->getTransferFunctions3D()->setWindow(window);

		// set TF 3D using the color points and alpha points based on windowlevel settings.
		ImageTF3DPtr tf3D = image->getTransferFunctions3D();
//		SSC_LOG("level: %f, win: %f, first: %f, second: %f", level, window, level-window/2, level+window/2);
		tf3D->addColorPoint(level-window/2, QColor("black"));
		tf3D->addColorPoint(level+window/2, QColor("white"));
		tf3D->removeInitAlphaPoint();
		tf3D->addAlphaPoint(level-0.7*window/2, 0);
		tf3D->addAlphaPoint(level+window/2, 255);

		image->getLookupTable2D()->setLevel(level);
		image->getLookupTable2D()->setWindow(window);
	}

	// add shading for known preoperative modalities
	if (image->getModality().contains("CT") || image->getModality().contains("MR"))
		image->setShadingOn(true);

	//std::cout << "ImagePtr MetaImageReader::load" << std::endl << std::endl;
	return image;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

DataPtr PNGImageReader::load(const QString& uid, const QString& filename)
{
	vtkImageDataPtr raw = this->loadVtkImageData(filename);
	if(!raw)
		return DataPtr();
	ImagePtr image(new Image(uid, raw));
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

//-----
DataPtr PolyDataMeshReader::load(const QString& uid, const QString& fileName)
{
	vtkPolyDataReaderPtr reader = vtkPolyDataReaderPtr::New();
	reader->SetFileName(cstring_cast(fileName));

	if (!ErrorObserver::checkedRead(reader, fileName))
		return DataPtr();

	vtkPolyDataPtr polyData = reader->GetOutput();

	//return MeshPtr(new Mesh(fileName, fileName, polyData));
	MeshPtr tempMesh(new Mesh(uid, "PolyData", polyData));
	return tempMesh;

}

DataPtr StlMeshReader::load(const QString& uid, const QString& fileName)
{
	vtkSTLReaderPtr reader = vtkSTLReaderPtr::New();
	reader->SetFileName(cstring_cast(fileName));

	if (!ErrorObserver::checkedRead(reader, fileName))
		return DataPtr();

	vtkPolyDataPtr polyData = reader->GetOutput();

	//return MeshPtr(new Mesh(fileName, fileName, polyData));
	MeshPtr tempMesh(new Mesh(uid, "PolyData", polyData));
	return tempMesh;

}



DataReaderWriter::DataReaderWriter()
{
	mDataReaders.insert(DataReaderPtr(new MetaImageReader()));
	mDataReaders.insert(DataReaderPtr(new MincImageReader()));
	mDataReaders.insert(DataReaderPtr(new PolyDataMeshReader()));
	mDataReaders.insert(DataReaderPtr(new StlMeshReader()));
	mDataReaders.insert(DataReaderPtr(new PNGImageReader()));

	// extra cx data types
	mDataReaders.insert(ssc::DataReaderPtr(new ssc::PointMetricReader()));
	mDataReaders.insert(ssc::DataReaderPtr(new ssc::DistanceMetricReader()));
	mDataReaders.insert(ssc::DataReaderPtr(new ssc::PlaneMetricReader()));
	mDataReaders.insert(ssc::DataReaderPtr(new ssc::AngleMetricReader()));
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

/** Read a data set and return it. Do NOT add it to the datamanager.
 *  Internal method: used by loadData family.
 */
DataPtr DataReaderWriter::readData(const QString& uid, const QString& path, const QString& type)
{
	DataReaderPtr reader = this->findReader(path, type);
	if (!reader)
	{
		std::cout << "failed to create data object: " << path << ", " << uid << ", " << type << std::endl;
		return DataPtr();
	}

	DataPtr current = reader->load(uid, path);
	if(!current)
		std::cout << "Failed to read data object: " << path << ", " << uid << ", " << type << std::endl;
	else
	{
		QFileInfo fileInfo(qstring_cast(path));
		current->setName(changeExtension(fileInfo.fileName(), ""));
		current->setFilePath(path); // need path even when not set explicitly: nice for testing
	}
	return current;
}


} // namespace ssc


