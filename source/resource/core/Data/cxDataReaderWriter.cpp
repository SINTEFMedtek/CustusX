/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#include "cxDataReaderWriter.h"

#include <sstream>
#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkSmartPointer.h>
#include <vtkMetaImageWriter.h>

#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkSTLReader.h>
#include <vtkImageChangeInformation.h>
#include "vtkMINCImageReader.h"
#include "vtkTransform.h"
#include "vtkCommand.h"
#include <vtkPNGReader.h>
#include <vtkNIFTIImageReader.h>

#include <QtCore>
#include <QDomDocument>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include "cxTransform3D.h"
#include "cxRegistrationTransform.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxUtilHelpers.h"
#include "cxVideoSource.h"
#include "cxCustomMetaImage.h"
#include "cxConfig.h"

#include "cxImageLUT2D.h"
#include "cxImageTF3D.h"


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
			reportError("Load of data [" + filename + "] failed with message:\n"
				+ errorObserver->mMessage);
			return false;
		}
		return true;
	}
};

//-----
vtkImageDataPtr MetaImageReader::loadVtkImageData(QString filename)
{
	vtkMetaImageReaderPtr reader = vtkMetaImageReaderPtr::New();
	reader->SetFileName(cstring_cast(filename));
	reader->ReleaseDataFlagOn();

	if (!ErrorObserver::checkedRead(reader, filename))
		return vtkImageDataPtr();

	vtkImageChangeInformationPtr zeroer = vtkImageChangeInformationPtr::New();
	zeroer->SetInputConnection(reader->GetOutputPort());
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
	writer->SetInputData(image->getBaseVtkImageData());
	writer->SetFileDimensionality(3);
	writer->SetFileName(cstring_cast(filename));
	QDir().mkpath(QFileInfo(filename).path());

//	std::cout << "SAVING MHD COMPRESSED " << filename << std::endl;
//	writer->SetCompression(true);
	writer->SetCompression(false);
//	writer->Update(); // caused writing of (null).0 files - not necessary
	writer->Write();

	writer = 0;

	CustomMetaImagePtr customReader = CustomMetaImage::create(filename);
	customReader->setTransform(image->get_rMd());
	customReader->setModality(image->getModality());
	customReader->setImageType(image->getImageType());
	customReader->setKey("WindowLevel", qstring_cast(image->getInitialWindowLevel()));
	customReader->setKey("WindowWidth", qstring_cast(image->getInitialWindowWidth()));
	customReader->setKey("Creator", QString("CustusX_%1").arg(CustusX_VERSION_STRING));
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

NIfTIReader::NIfTIReader() :
	DataReader()
{
	sform_matrix = vtkMatrix4x4Ptr::New();
}

bool NIfTIReader::readInto(ImagePtr image, QString path)
{
	if (!image)
		return false;

	vtkImageDataPtr raw = this->loadVtkImageData(path);
	if(!raw)
		return false;

	Transform3D rMd(sform_matrix);
	image->setVtkImageData(raw);
	image->get_rMd_History()->setRegistration(rMd);

	return true;
}

bool NIfTIReader::readInto(DataPtr data, QString filename)
{
	return this->readInto(boost::dynamic_pointer_cast<Image>(data), filename);
}

DataPtr NIfTIReader::load(const QString &uid, const QString &filename)
{
	ImagePtr image(new Image(uid, vtkImageDataPtr()));
	this->readInto(image, filename);
	return image;
}

vtkImageDataPtr NIfTIReader::loadVtkImageData(QString filename)
{
	vtkNIFTIImageReaderPtr reader = vtkNIFTIImageReaderPtr::New();
	reader->SetFileName(cstring_cast(filename));
	reader->ReleaseDataFlagOn();
	reader->Update();

	/** https://nifti.nimh.nih.gov/nifti-1/documentation/faq#Q19
	Q19. Why does NIfTI-1 allow for two coordinate systems (the qform and sform)? (Mark Jenkinson)
	The basic idea behind having two coordinate systems is to allow the image to store information about (1) the scanner coordinate system used in the acquisition of the volume (in the qform) and (2) the relationship to a standard coordinate system - e.g. MNI coordinates (in the sform).

	The qform allows orientation information to be kept for alignment purposes without losing volumetric information, since the qform only stores a rigid-body transformation which preserves volume. On the other hand, the sform stores a general affine transformation which can map the image coordinates into a standard coordinate system, like Talairach or MNI, without the need to resample the image.

	By having both coordinate systems, it is possible to keep the original data (without resampling), along with information on how it was acquired (qform) and how it relates to other images via a standard space (sform). This ability is advantageous for many analysis pipelines, and has previously required storing additional files along with the image files. By using NIfTI-1 this extra information can be kept in the image files themselves.

	Note: the qform and sform also store information on whether the coordinate system is left-handed or right-handed (see Q15) and so when both are set they must be consistent, otherwise the handedness of the coordinate system (often used to distinguish left-right order) is unknown and the results of applying operations to such an image are unspecified.

	*/
	sform_matrix = reader->GetSFormMatrix();

	if (!ErrorObserver::checkedRead(reader, filename))
		return vtkImageDataPtr();

	return reader->GetOutput();
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

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


bool XMLPolyDataMeshReader::readInto(DataPtr data, QString filename)
{
	return this->readInto(boost::dynamic_pointer_cast<Mesh>(data), filename);
}

bool XMLPolyDataMeshReader::readInto(MeshPtr mesh, QString filename)
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
vtkPolyDataPtr XMLPolyDataMeshReader::loadVtkPolyData(QString fileName)
{
	vtkXMLPolyDataReaderPtr reader = vtkXMLPolyDataReaderPtr::New();
	reader->SetFileName(cstring_cast(fileName));

	if (!ErrorObserver::checkedRead(reader, fileName))
		return vtkPolyDataPtr();

	vtkPolyDataPtr polyData = reader->GetOutput();
	return polyData;
}

DataPtr XMLPolyDataMeshReader::load(const QString& uid, const QString& filename)
{
	MeshPtr mesh(new Mesh(uid));
	this->readInto(mesh, filename);
	return mesh;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


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

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


DataReaderWriter::DataReaderWriter()
{
	mDataReaders.insert(DataReaderPtr(new MetaImageReader()));
	mDataReaders.insert(DataReaderPtr(new NIfTIReader()));
	mDataReaders.insert(DataReaderPtr(new PolyDataMeshReader()));
	mDataReaders.insert(DataReaderPtr(new XMLPolyDataMeshReader()));
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


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


} // namespace cx


