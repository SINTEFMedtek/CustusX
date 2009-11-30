#include "sscPositionStorageFile.h"

namespace ssc {

PositionStorageWriter::PositionStorageWriter(QString filename) : positions(filename)
{
	positions.open(QIODevice::Append);
	stream.setDevice(&positions);
	stream.setByteOrder(QDataStream::LittleEndian);
	if (positions.size() == 0)
	{
		stream.writeRawData("SNWPOS", 6);
		stream << (quint8)1;
	}
}

PositionStorageWriter::~PositionStorageWriter()
{
	positions.close();	
}


void PositionStorageWriter::write(Transform3D matrix, double timestamp, int toolIndex)
{
	ssc::Frame3D frame = ssc::Frame3D::create(matrix);
	
	stream << (quint8)1;	// Type - there is only one
	stream << (quint8)15;	// Size of data following this point
	stream << (quint64)timestamp;	// Microseconds since Epoch
	stream << (quint8)toolIndex;	// tool index
	stream << (double)frame.mThetaXY;
	stream << (double)frame.mThetaZ;
	stream << (double)frame.mPhi;
	stream << (double)frame.mPos[0];
	stream << (double)frame.mPos[1];
	stream << (double)frame.mPos[2];
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

PositionStorageReader::PositionStorageReader(QString filename) : positions(filename)
{
	positions.open(QIODevice::ReadOnly);
	stream.setDevice(&positions);
	stream.setByteOrder(QDataStream::LittleEndian);
	
	char header[6] = "     ";
	stream.readRawData(header, 6);
	quint8 version = 0;
	stream >> version;
	
	if (QString(header)!="SNWPOS" || version<1)
	{
		std::cout << "Error in header for file [" << filename.toStdString() << "]" << std::endl;
		positions.close();
	}
}

PositionStorageReader::~PositionStorageReader()
{
	
}

bool PositionStorageReader::read(Transform3D* matrix, double* timestamp, int* toolIndex)
{
	if (atEnd())
		return false;
	
	quint8 type;
	quint8 size;
	quint64 ts;
	quint8 tool;
	ssc::Frame3D frame;
	
	stream >> type;
	stream >> size;
	stream >> ts;
	stream >> tool;
	stream >> frame.mThetaXY >> frame.mThetaZ >> frame.mPhi;
	stream >> frame.mPos[0] >> frame.mPos[1] >> frame.mPos[2];
	
	*matrix = frame.transform();
	*timestamp = ts;
	*toolIndex = tool;
	
	return (type==1) && (size==15);
}

bool PositionStorageReader::atEnd() const
{
	return !positions.isReadable() || stream.atEnd();	
}

} // namespace ssc 
