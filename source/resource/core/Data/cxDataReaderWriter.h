/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	virtual bool readInto(DataPtr data, QString filename);
	bool readInto(ImagePtr image, QString path);
	virtual DataPtr load(const QString& uid, const QString& filename);
	virtual vtkImageDataPtr loadVtkImageData(QString filename);
	void saveImage(ImagePtr image, const QString& filename);
};

/**\brief Reader for NIfTI files.
 *
 */
class cxResource_EXPORT NIfTIReader: public DataReader
{
public:
	NIfTIReader();
	virtual ~NIfTIReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		QString fileType = QFileInfo(filename).suffix();
		return (fileType.compare("nii", Qt::CaseInsensitive) == 0);
	}
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(ImagePtr image, QString filename);
	virtual QString canLoadDataType() const { return "image"; }
	virtual DataPtr load(const QString& uid, const QString& filename);
	virtual vtkImageDataPtr loadVtkImageData(QString filename);

private:
	vtkMatrix4x4Ptr sform_matrix; ///< the sform stores a general affine transformation which can map the image coordinates into a standard coordinate system, like Talairach or MNI
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


/**\brief Read or write vtk or ssc data objects from/to file.
 *
 */
class cxResource_EXPORT DataReaderWriter
{
public:
	explicit DataReaderWriter();

	vtkImageDataPtr loadVtkImageData(QString filename);
	vtkPolyDataPtr loadVtkPolyData(QString filename);
	QString findDataTypeFromFile(QString filename);
	void readInto(DataPtr data, QString path);

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
