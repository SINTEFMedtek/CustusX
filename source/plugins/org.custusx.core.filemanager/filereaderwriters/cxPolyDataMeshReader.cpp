#include "cxPolyDataMeshReader.h"

#include <QFileInfo>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolyData.h>
#include <ctkPluginContext.h>
#include "cxMesh.h"
#include "cxErrorObserver.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"

namespace cx
{

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

vtkPolyDataPtr PolyDataMeshReader::loadVtkPolyData(QString fileName)
{
	vtkPolyDataReaderPtr reader = vtkPolyDataReaderPtr::New();
	reader->SetFileName(cstring_cast(fileName));

	if (!ErrorObserver::checkedRead(reader, fileName))
		return vtkPolyDataPtr();

	vtkPolyDataPtr polyData = reader->GetOutput();
	return polyData;
}

DataPtr PolyDataMeshReader::read(const QString& uid, const QString& filename)
{
	MeshPtr mesh(new Mesh(uid));
	this->readInto(mesh, filename);
	return mesh;
}

std::vector<DataPtr> PolyDataMeshReader::read(const QString &filename)
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

void PolyDataMeshReader::write(DataPtr data, const QString &filename)
{
	MeshPtr mesh = boost::dynamic_pointer_cast<Mesh>(data);
	if(!mesh)
		reportError("Could not cast data to mesh");
	vtkPolyDataWriterPtr writer = vtkPolyDataWriterPtr::New();
	writer->SetInputData(mesh->getVtkPolyData());
	writer->SetFileName(cstring_cast(filename));

	writer->Update();
	writer->Write();
}

PolyDataMeshReader::PolyDataMeshReader(ctkPluginContext *context) :
	FileReaderWriterImplService("PolyDataMeshReader", "mesh", "mesh", "vtk", context)
{
}

bool PolyDataMeshReader::canRead(const QString &type, const QString &filename)
{
	QString fileType = QFileInfo(filename).suffix();
	return ( fileType.compare("vtk", Qt::CaseInsensitive) == 0);
}


}


QString cx::PolyDataMeshReader::canWriteDataType() const
{
	return "mesh";
}

bool cx::PolyDataMeshReader::canWrite(const QString &type, const QString &filename) const
{
	return this->canWriteInternal(type, filename);
}