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
#ifndef SSCDATAREADERWRITER_H
#define SSCDATAREADERWRITER_H

#include <map>
#include <set>
#include <string>
#include <QMutex>
#include <vector>
#include "sscImage.h"
#include "sscMesh.h"
#include "sscDataManager.h"
#include <QFileInfo>
#include "boost/scoped_ptr.hpp"

namespace ssc
{

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
class StaticMutexVtkLocker
{
public:
	StaticMutexVtkLocker();
	~StaticMutexVtkLocker();
private:
	static boost::shared_ptr<QMutex> mMutex;
};


/**\brief Interface for Data file readers.
 *
 * \ingroup sscData
 */
class DataReader
{
public:
	virtual ~DataReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename) = 0;
	virtual DataPtr load(const QString& uid, const QString& filename) = 0;
	virtual vtkImageDataPtr loadVtkImageData(QString filename) { return vtkImageDataPtr(); }
	virtual vtkPolyDataPtr loadVtkPolyData(QString filename) { return vtkPolyDataPtr(); }
};
typedef boost::shared_ptr<DataReader> DataReaderPtr;

/**\brief Reader for metaheader .mhd files.
 *
 * \ingroup sscData
 */
class MetaImageReader: public DataReader
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
	virtual DataPtr load(const QString& uid, const QString& filename);
//	vtkImageDataPtr load(const QString& filename) { return this->loadVtkImageData(filename); }
	virtual vtkImageDataPtr loadVtkImageData(QString filename);
};

/**\brief Reader for portable network graphics .png files.
 *
 * \ingroup sscData
 */
class PNGImageReader: public DataReader
{
public:
	virtual ~PNGImageReader() {}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		QString fileType = QFileInfo(filename).suffix();
		return (fileType.compare("png", Qt::CaseInsensitive) == 0);
	}
	virtual DataPtr load(const QString& uid, const QString& filename);
	virtual vtkImageDataPtr loadVtkImageData(QString filename);
};

/**\brief Reader for minc files.
 *
 * \ingroup sscData
 */
class MincImageReader: public DataReader
{
public:
	virtual ~MincImageReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		QString fileType = QFileInfo(filename).suffix();
		return (fileType.compare("mnc", Qt::CaseInsensitive) == 0);
	}
	virtual DataPtr load(const QString& uid, const QString& filename);
};

//class MeshReader :
//{
//public:
//	virtual ~MeshReader() {}
//	virtual bool canLoad(const QString& filename) = 0;
//	virtual MeshPtr load(const QString& uid, const QString& filename) = 0;
//};
//typedef boost::shared_ptr<MeshReader> MeshReaderPtr;


/**\brief Reader for .vtk files.
 *
 * \ingroup sscData
 */
class PolyDataMeshReader: public DataReader
{
public:
	virtual ~PolyDataMeshReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		QString fileType = QFileInfo(filename).suffix();
		return (fileType.compare("vtk", Qt::CaseInsensitive) == 0);
	}
	virtual DataPtr load(const QString& uid, const QString& filename);
};

/**\brief Reader for STL files.
 *
 * \ingroup sscData
 */
class StlMeshReader: public DataReader
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
	virtual DataPtr load(const QString& uid, const QString& filename);
};

/** Read or write vtk or ssc data objects from/to file.
 *
 *
 * \ingroup sscData
 * \date May 2, 2013
 * \author christiana
 */
class DataReaderWriter
{
public:
	DataReaderWriter();

	vtkImageDataPtr loadVtkImageData(QString filename);
	vtkPolyDataPtr loadVtkPolyData(QString filename);
//	ssc::DataPtr loadData(QString uid, QString filename);
	DataPtr readData(const QString& uid, const QString& path, const QString& type);

private:
	DataReaderPtr findReader(const QString& path, const QString& type="unknown");
	typedef std::set<DataReaderPtr> DataReadersType;
	DataReadersType mDataReaders;
};

} // namespace ssc

#endif // SSCDATAREADERWRITER_H
