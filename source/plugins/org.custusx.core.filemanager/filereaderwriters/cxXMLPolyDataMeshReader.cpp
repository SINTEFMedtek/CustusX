/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxXMLPolyDataMeshReader.h"

#include "cxMesh.h"
#include <ctkPluginContext.h>
#include "cxErrorObserver.h"
#include <vtkXMLPolyDataReader.h>
#include "cxTypeConversions.h"
#include <vtkPolyData.h>

namespace cx {

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

std::vector<DataPtr> XMLPolyDataMeshReader::read(const QString &filename)
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

vtkPolyDataPtr XMLPolyDataMeshReader::loadVtkPolyData(QString fileName)
{
	vtkXMLPolyDataReaderPtr reader = vtkXMLPolyDataReaderPtr::New();
	reader->SetFileName(cstring_cast(fileName));

	if (!ErrorObserver::checkedRead(reader, fileName))
		return vtkPolyDataPtr();

	vtkPolyDataPtr polyData = reader->GetOutput();
	return polyData;
}

DataPtr XMLPolyDataMeshReader::read(const QString& uid, const QString& filename)
{
	MeshPtr mesh(new Mesh(uid));
	this->readInto(mesh, filename);
	return mesh;
}

QString XMLPolyDataMeshReader::canReadDataType() const
{
	return Mesh::getTypeName();
}

XMLPolyDataMeshReader::XMLPolyDataMeshReader(PatientModelServicePtr patientModelService) :
	FileReaderWriterImplService("XMLPolyDataMeshReader", Mesh::getTypeName(), "", "vtp", patientModelService)
{
}

bool XMLPolyDataMeshReader::canRead(const QString &type, const QString &filename)
{
	QString fileType = QFileInfo(filename).suffix();
	return ( fileType.compare("vtp", Qt::CaseInsensitive) == 0);
}

}

QString cx::XMLPolyDataMeshReader::canWriteDataType() const
{
	return "";
}

bool cx::XMLPolyDataMeshReader::canWrite(const QString &type, const QString &filename) const
{
	return false;
}
