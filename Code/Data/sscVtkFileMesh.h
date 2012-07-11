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

#ifndef SSCVTKFILEMESH_
#define SSCVTKFILEMESH_

#include <set>
#include <boost/shared_ptr.hpp>

#include "vtkForwardDeclarations.h"

#include "sscMesh.h"

namespace ssc
{

/**
  * A mesh data set capable of reading a vtk polydata file.
  *
  * \ingroup sscData
  */
class VtkFileMesh: public Mesh
{
	Q_OBJECT

public:
	VtkFileMesh(const QString& uid, const QString& name = "", const QString& filePath = "");
	virtual ~VtkFileMesh() {}

	virtual vtkPolyDataPtr getVtkPolyData();
	virtual QString getType() const
	{
		return "vtkfilemesh";
	}

private:
	vtkPolyDataPtr mVtkPolyData;

	void loadData();
};

typedef boost::shared_ptr<VtkFileMesh> VtkFileMeshPtr;

} // namespace ssc

#endif /*SSCVTKFILEMESH_*/
