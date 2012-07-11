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

#include "sscVtkFileMesh.h"

#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>

/**
 * \brief Interfaces a vtk poly data file
 */
namespace ssc
{

VtkFileMesh::VtkFileMesh(const QString &uid, const QString &name, const QString& filePath)
	: Mesh(uid, name)
{
	setFilePath(filePath);
}

/**
 * Return a vtk poly data object.
 * Load from file if needed.
 */
vtkPolyDataPtr VtkFileMesh::getVtkPolyData()
{
	if (!mVtkPolyData)
	{
		loadData();
	}

	return mVtkPolyData;
}

/**
 * Load and assign a poly data object from file.
 * Assuming a file path is already provided.
 */
void VtkFileMesh::loadData()
{
	if (mFilePath.isEmpty())
		return;

	vtkPolyDataReaderPtr reader = vtkPolyDataReaderPtr::New();
	reader->SetFileName(mFilePath.toUtf8().constData());

	mVtkPolyData = reader->GetOutput();

	emit meshChanged();
}

}
