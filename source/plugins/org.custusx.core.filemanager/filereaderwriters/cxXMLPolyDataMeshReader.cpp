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

XMLPolyDataMeshReader::XMLPolyDataMeshReader() :
	FileReaderWriterImplService("XMLPolyDataMeshReader", "mesh", "", "vtp")
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