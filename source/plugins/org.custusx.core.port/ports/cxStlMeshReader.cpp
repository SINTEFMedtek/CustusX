#include "cxStlMeshReader.h"

#include <QFileInfo>
#include <vtkSTLReader.h>
#include "cxTypeConversions.h"
#include <vtkPolyData.h>
#include "cxMesh.h"
#include "cxErrorObserver.h"
#include <ctkPluginContext.h>

namespace cx
{

StlMeshReader::StlMeshReader()
{
	this->setObjectName("StlMeshReader");
}

bool StlMeshReader::canLoad(const QString &type, const QString &filename)
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

}