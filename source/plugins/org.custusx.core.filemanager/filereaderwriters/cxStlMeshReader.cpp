/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxStlMeshReader.h"

#include <QFileInfo>
#include <vtkSTLReader.h>
#include "cxTypeConversions.h"
#include <vtkPolyData.h>
#include "cxMesh.h"
#include "cxErrorObserver.h"
#include <ctkPluginContext.h>
#include <vtkSTLWriter.h>
#include "cxLogger.h"

namespace cx
{

StlMeshReader::StlMeshReader(PatientModelServicePtr patientModelService) :
	FileReaderWriterImplService("StlMeshReader", Mesh::getTypeName(), Mesh::getTypeName(), "stl", patientModelService)
{
}

bool StlMeshReader::canRead(const QString &type, const QString &filename)
{
	QString fileType = QFileInfo(filename).suffix();
	return (fileType.compare("stl", Qt::CaseInsensitive) == 0);
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

QString StlMeshReader::canReadDataType() const
{
	return Mesh::getTypeName();
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

DataPtr StlMeshReader::read(const QString& uid, const QString& filename)
{
	MeshPtr mesh(new Mesh(uid));
	this->readInto(mesh, filename);
	return mesh;
}

std::vector<DataPtr> StlMeshReader::read(const QString &filename)
{
	std::vector<DataPtr> retval;
	MeshPtr mesh = boost::dynamic_pointer_cast<Mesh>(this->createData(Mesh::getTypeName(), filename));

	vtkPolyDataPtr raw = this->loadVtkPolyData(filename);
	if(!raw)
		return retval;
	mesh->setVtkPolyData(raw);

	retval.push_back(mesh);
	return retval;
}

void StlMeshReader::write(DataPtr data, const QString &filename)
{
	MeshPtr mesh = boost::dynamic_pointer_cast<Mesh>(data);
	if(!mesh)
		reportError("Could not cast data to mesh");
	vtkSTLWriterPtr writer = vtkSTLWriterPtr::New();
	writer->SetInputData(mesh->getVtkPolyData());
	writer->SetFileName(cstring_cast(filename));

	writer->Update();
	writer->Write();
}

}

QString cx::StlMeshReader::canWriteDataType() const
{
	return Mesh::getTypeName();
}

bool cx::StlMeshReader::canWrite(const QString &type, const QString &filename) const
{
	return this->canWriteInternal(type, filename);
}
