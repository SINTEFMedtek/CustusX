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

#ifndef CXDATAREADERWRITER_H_
#define CXDATAREADERWRITER_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include <map>
#include <set>
#include <string>
#include <QMutex>
#include <vector>
#include "cxImage.h"
#include "cxMesh.h"
#include <QFileInfo>
#include "boost/scoped_ptr.hpp"
#include "cxLogger.h"

namespace cx
{
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;

/**
 * \file
 * \addtogroup cx_resource_core_data
 * @{
 */

/** Locks a static mutex in the constructor and unlocks it in the desctructor,
  * similar to a QMutexLocker.
  *
  * Use this as a global access restriction for thread-unsafe VTK objects.
  *
  * Testing has shown that the following methods need to be mutexed:
  *   - vtkMetaImageReader::Update()
  *   - vtkMetaImageWrite::Write()
  * There are probably some global stuff inside vtkmetaio.
  *
  * Note: Googling indicates that VTK in general is threadUNsafe.
  */
class cxResource_EXPORT StaticMutexVtkLocker
{
public:
	StaticMutexVtkLocker();
	~StaticMutexVtkLocker();
private:
	static boost::shared_ptr<QMutex> mMutex;
};


/**\brief Interface for Data file readers.
 *
 */
class cxResource_EXPORT DataReader
{
public:
	virtual ~DataReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename) = 0;
	virtual DataPtr load(const QString& uid, const QString& filename) = 0;
	virtual vtkImageDataPtr loadVtkImageData(QString filename) { return vtkImageDataPtr(); }
	virtual vtkPolyDataPtr loadVtkPolyData(QString filename) { return vtkPolyDataPtr(); }
	virtual QString canLoadDataType() const =0;
	virtual bool readInto(DataPtr data, QString path) = 0;
	virtual void saveImage(ImagePtr image, const QString& filename){}

};
typedef boost::shared_ptr<DataReader> DataReaderPtr;

/**\brief Reader for metaheader .mhd files.
 *
 */
class cxResource_EXPORT MetaImageReader: public DataReader
{
public:
	virtual ~MetaImageReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		QString fileType = QFileInfo(filename).suffix();
		return (fileType.compare("mhd", Qt::CaseInsensitive) == 0 || fileType.compare("mha", Qt::CaseInsensitive) == 0);
	}
	virtual QString canLoadDataType() const { return "image"; }
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(ImagePtr image, QString filename);
	virtual DataPtr load(const QString& uid, const QString& filename);
	virtual vtkImageDataPtr loadVtkImageData(QString filename);
	void saveImage(ImagePtr image, const QString& filename);
};

/**\brief Reader for portable network graphics .png files.
 *
 */
class cxResource_EXPORT PNGImageReader: public DataReader
{
public:
	virtual ~PNGImageReader() {}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		QString fileType = QFileInfo(filename).suffix();
		return (fileType.compare("png", Qt::CaseInsensitive) == 0);
	}
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(ImagePtr image, QString filename);
	virtual QString canLoadDataType() const { return "image"; }
	virtual DataPtr load(const QString& uid, const QString& filename);
	virtual vtkImageDataPtr loadVtkImageData(QString filename);
};


/**\brief Reader for .vtk files.
 *
 */
class cxResource_EXPORT PolyDataMeshReader: public DataReader
{
public:
	virtual ~PolyDataMeshReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		QString fileType = QFileInfo(filename).suffix();
		return ( fileType.compare("vtk", Qt::CaseInsensitive) == 0);
	}
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(MeshPtr mesh, QString filename);

	virtual vtkPolyDataPtr loadVtkPolyData(QString filename);
	virtual QString canLoadDataType() const { return "mesh"; }
	virtual DataPtr load(const QString& uid, const QString& filename);
};

/**\brief Reader for .vtp files.
 *
 */
class cxResource_EXPORT XMLPolyDataMeshReader: public DataReader
{
public:
	virtual ~XMLPolyDataMeshReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		QString fileType = QFileInfo(filename).suffix();
		return ( fileType.compare("vtp", Qt::CaseInsensitive) == 0);
	}
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(MeshPtr mesh, QString filename);

	virtual vtkPolyDataPtr loadVtkPolyData(QString filename);
	virtual QString canLoadDataType() const { return "mesh"; }
	virtual DataPtr load(const QString& uid, const QString& filename);
};

/**\brief Reader for STL files.
 *
 */
class cxResource_EXPORT StlMeshReader: public DataReader
{
public:
	virtual ~StlMeshReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		QString fileType = QFileInfo(filename).suffix();
		return (fileType.compare("stl", Qt::CaseInsensitive) == 0);
	}
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(MeshPtr mesh, QString filename);
	virtual vtkPolyDataPtr loadVtkPolyData(QString filename);
	virtual QString canLoadDataType() const { return "mesh"; }
	virtual DataPtr load(const QString& uid, const QString& filename);
};

/** Read or write vtk or ssc data objects from/to file.
 *
 * \date May 2, 2013
 * \author christiana
 */
class cxResource_EXPORT DataReaderWriter
{
public:
	explicit DataReaderWriter();

	bool canLoad(const QString& type, const QString& filename);
	DataPtr load(const QString& uid, const QString& filename);
	vtkImageDataPtr loadVtkImageData(QString filename);
	vtkPolyDataPtr loadVtkPolyData(QString filename);
	QString findDataTypeFromFile(QString filename);
	void readInto(DataPtr data, QString path);
	void saveImage(ImagePtr image, const QString& filename);

private:
	DataReaderPtr findReader(const QString& path, const QString& type="unknown");
	typedef std::set<DataReaderPtr> DataReadersType;
	DataReadersType mDataReaders;


};

/**
 * @}
 */


} // namespace cx

#endif // CXDATAREADERWRITER_H_
