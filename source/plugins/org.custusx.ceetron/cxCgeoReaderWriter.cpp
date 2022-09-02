/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxCgeoReaderWriter.h"

#include <QDir>
#include "vtkPolyData.h"
#include <vtkCellArray.h>
#include "vtkIdList.h"
#include "cxMesh.h"
#include "cxLogger.h"

namespace cx
{

CgeoReaderWriter::CgeoReaderWriter(PatientModelServicePtr patientModelService) :
	FileReaderWriterImplService("CgeoReaderWriter", "", Mesh::getTypeName(), "cgeo", patientModelService)
{
}

bool CgeoReaderWriter::isNull()
{
	return false;
}

bool CgeoReaderWriter::canRead(const QString &type, const QString &filename)
{
	return false;
}

std::vector<DataPtr> CgeoReaderWriter::read(const QString &filename)
{
	return std::vector<DataPtr>();
}

DataPtr CgeoReaderWriter::read(const QString &uid, const QString &filename)
{
	return DataPtr();
}

QString CgeoReaderWriter::canReadDataType() const
{
	return "";
}

bool CgeoReaderWriter::readInto(DataPtr data, QString path)
{
	return false;
}

void CgeoReaderWriter::write(DataPtr data, const QString &filename)
{
	QFile exportFile(filename);
	exportFile.open(QIODevice::WriteOnly);
	QDataStream out(&exportFile);
	this->writeToStream(data, out);
}

QByteArray CgeoReaderWriter::convertToQByteArray(DataPtr data)
{
	QByteArray retval;
	QDataStream out(&retval, QIODevice::WriteOnly);
	this->writeToStream(data, out);

	return retval;
}

void CgeoReaderWriter::writeToStream(DataPtr data, QDataStream &out)
{
	MeshPtr mesh = boost::dynamic_pointer_cast<Mesh>(data);
	if(!mesh)
	{
		CX_LOG_ERROR() << "Couldn't find mesh.";
		return;
	}
	vtkPolyDataPtr polyData = mesh->getTransformedPolyDataCopy(mesh->get_rMd());
	vtkCellArrayPtr polys = polyData->GetPolys();

	out.setByteOrder(QDataStream::LittleEndian);
	out.setFloatingPointPrecision(QDataStream::SinglePrecision);

	out << (qint32) 12072001;        // Magic (12072001)
	out << (qint32) 0;  // TextureCount
	out << (qint32) 1;  // PartCount

	out << (qint32) 0; // Component ID

	if(mesh->getColor().rgba())
	{
		int intRed = mesh->getColor().red();
		int intGreen = mesh->getColor().green();
		int intBlue = mesh->getColor().blue();
		int intAlpha = mesh->getColor().alpha();
		int intColor = intRed<<24 | intGreen<<16 | intBlue<<8 | intAlpha;
		out << (qint32) intColor; // Color
	}
	else
		out << (qint32) 0xFFFFFFFF;//53672537; // Color

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

QString CgeoReaderWriter::canWriteDataType() const
{
	return Mesh::getTypeName();
}

bool CgeoReaderWriter::canWrite(const QString &type, const QString &filename) const
{
	return this->canWriteInternal(type, filename);
}
}//cx
