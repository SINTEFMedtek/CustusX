#include "cxCgeoReaderWriter.h"

#include <QDir>
#include "vtkPolyData.h"
#include <vtkCellArray.h>
#include "vtkIdList.h"
#include "cxMesh.h"

namespace cx
{

CgeoReaderWriter::CgeoReaderWriter()
{
	this->setObjectName("CgeoReaderWriter");
}

bool CgeoReaderWriter::isNull()
{
	return false;
}

bool CgeoReaderWriter::canLoad(const QString &type, const QString &filename)
{
	return false;
}

DataPtr CgeoReaderWriter::load(const QString &uid, const QString &filename)
{
	return DataPtr();
}

QString CgeoReaderWriter::canLoadDataType() const
{
	return "";
}

bool CgeoReaderWriter::readInto(DataPtr data, QString path)
{
	return false;
}

void CgeoReaderWriter::save(DataPtr data, const QString &filename)
{
	MeshPtr mesh = boost::dynamic_pointer_cast<Mesh>(data);
	vtkPolyDataPtr polyData = mesh->getTransformedPolyData(mesh->get_rMd());
	vtkCellArrayPtr polys = polyData->GetPolys();

	QFile exportFile(filename);
	exportFile.open(QIODevice::WriteOnly);
	QDataStream out(&exportFile);
	out.setByteOrder(QDataStream::LittleEndian);
	out.setFloatingPointPrecision(QDataStream::SinglePrecision);

	out << (qint32) 12072001;        // Magic (12072001)
	out << (qint32) 0;  // TextureCount
	out << (qint32) 1;  // PartCount

	out << (qint32) 0; // Component ID
	out << (qint32) 53672537; // Color

	out << (qint32) polyData->GetNumberOfPoints(); // # vertices
	out << (qint32) 0; // always 0
	out << (qint32) -1; // always -1
	out << (qint32) polys->GetNumberOfCells(); // # primitives
	out << (qint32) 3; // 3 nodes per primitive (triangle)

	for (int i=0; i<polyData->GetNumberOfPoints(); i++)
	{
		double p[3];
		polyData->GetPoint(i,p);
		out << p[0]; // x
		out << p[1]; // y
		out << p[2]; // z
	}

	vtkIdType n_ids;
	vtkSmartPointer<vtkIdList> idlist = vtkSmartPointer<vtkIdList>::New();
	polys->InitTraversal();

	while(polys->GetNextCell(idlist))
	{
		n_ids = idlist->GetNumberOfIds();
		if (n_ids == 3)
		{
		   out << (qint32)  idlist->GetId(0); // First vertex of triangle
		   out << (qint32)  idlist->GetId(1); // Second vertex of triangle
		   out << (qint32)  idlist->GetId(2); // Third vertex of triangle
		}
		else
			std::cout << "Warning in .cgeo export: Skipped polygon not containing exactly 3 points." << std::endl;
	}
}
}
