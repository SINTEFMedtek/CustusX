#include "sscPositionStorageFile.h"
#include <QDateTime>
#include <boost/cstdint.hpp>
#include "sscFrame3D.h"


namespace ssc
{


PositionStorageReader::PositionStorageReader(QString filename) : positions(filename)
{
  positions.open(QIODevice::ReadOnly);
  stream.setDevice(&positions);
  stream.setByteOrder(QDataStream::LittleEndian);

  char header[6] = "     ";
  stream.readRawData(header, 6);
  mVersion = 0;
  stream >> mVersion;

  if (QString(header)!="SNWPOS" || mVersion<1)
  {
    std::cout << "Error in header for file [" << filename.toStdString() << "]" << std::endl;
    positions.close();
  }
}

PositionStorageReader::~PositionStorageReader()
{

}

int PositionStorageReader::version()
{
  return mVersion;
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

bool PositionStorageReader::read(Transform3D* matrix, double* timestamp, QString* toolUid)
{
  if (this->atEnd())
    return false;

  quint8 type;
  quint8 size;

  if (type==2) // change tool format
  {
    stream >> size;
    char* data = NULL;
    uint isize = 0;
    stream.readBytes(data, isize);
    mCurrentToolUid = QString(QByteArray(data, size));
    delete[] data;

    stream >> type; // read type and make ready for a new read below
  }

  if (type==1) // tool-on-line format
  {
    quint64 ts;
    quint8 tool;
    ssc::Frame3D frame;

    stream >> size;
    stream >> ts;
    stream >> tool;
    stream >> frame.mThetaXY >> frame.mThetaZ >> frame.mPhi;
    stream >> frame.mPos[0] >> frame.mPos[1] >> frame.mPos[2];

    *matrix = frame.transform();
    *timestamp = ts;
    *toolUid = QString::number(tool);
    return (size==15);
  }
  else if (type==3) // position only format
  {
    quint64 ts;
    ssc::Frame3D frame;

    stream >> size;
    stream >> ts;
    stream >> frame.mThetaXY >> frame.mThetaZ >> frame.mPhi;
    stream >> frame.mPos[0] >> frame.mPos[1] >> frame.mPos[2];

    *matrix = frame.transform();
    *timestamp = ts;
    *toolUid = mCurrentToolUid;
    return true;
  }
  else
  {
    return false;
  }

  return false;
}

bool PositionStorageReader::atEnd() const
{
  return !positions.isReadable() || stream.atEnd();
}

QString PositionStorageReader::timestampToString(double timestamp)
{
  QDateTime retval;
  boost::uint64_t ts = static_cast<boost::uint64_t>(timestamp);
  retval.setTime_t(ts/1000);
  retval = retval.addMSecs(ts%1000);
  return retval.toString("yyyyMMddhhmmss.zzz");
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


PositionStorageWriter::PositionStorageWriter(QString filename) : positions(filename)
{
	positions.open(QIODevice::Append);
	stream.setDevice(&positions);
	stream.setByteOrder(QDataStream::LittleEndian);
	if (positions.size() == 0)
	{
		stream.writeRawData("SNWPOS", 6);
		stream << (quint8)2; // version 1 had only 32 bit timestamps
	}
}

PositionStorageWriter::~PositionStorageWriter()
{
	positions.close();	
}


void PositionStorageWriter::write(Transform3D matrix, uint64_t timestamp, int toolIndex)
{
	ssc::Frame3D frame = ssc::Frame3D::create(matrix);
	
	stream << (quint8)1;	// Type - there is only one
	stream << (quint8)(8+1+6*10);	// Size of data following this point
	stream << (quint64)timestamp;	// Microseconds since Epoch
	stream << (quint8)toolIndex;	// tool index
	stream << (double)frame.mThetaXY;
	stream << (double)frame.mThetaZ;
	stream << (double)frame.mPhi;
	stream << (double)frame.mPos[0];
	stream << (double)frame.mPos[1];
	stream << (double)frame.mPos[2];
}

void PositionStorageWriter::write(Transform3D matrix, uint64_t timestamp, QString toolUid)
{
  if (toolUid!=mCurrentToolUid)
  {
    QByteArray name = toolUid.toAscii();

    stream << (quint8)2;  // Type - tool change
    stream << (quint8)(name.size()+4); // Size of data following this point
    stream.writeBytes(name.data(), name.size());
    mCurrentToolUid = toolUid;
  }
  else
  {
    ssc::Frame3D frame = ssc::Frame3D::create(matrix);

    stream << (quint8)3;  // Type -
    stream << (quint8)(8+6*10); // Size of data following this point
    stream << (quint64)timestamp; // Microseconds since Epoch
    stream << (double)frame.mThetaXY;
    stream << (double)frame.mThetaZ;
    stream << (double)frame.mPhi;
    stream << (double)frame.mPos[0];
    stream << (double)frame.mPos[1];
    stream << (double)frame.mPos[2];
  }

}


} // namespace ssc 
