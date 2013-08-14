#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <QtGui>
#include <boost/cstdint.hpp>
#include <limits>

#include "sscTransform3D.h"
#include "sscPositionStorageFile.h"
#include "sscUtilHelpers.h"
#include "sscTime.h"
#include "sscTypeConversions.h"

#define EVENT_DATE_FORMAT "yyyy:MM:dd-HH:mm:ss.zzz000"

/** 
 */
int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	int arg = 1;
	
	if (argc<2)
	{
		std::cout << "Usage: sscPositionFileReader [-v] <filename> <timestamp>\nTimestamp format " << EVENT_DATE_FORMAT << std::endl;
		return 0;
	}


	bool verbose = (QString(argv[1]) == "-v");
	if (verbose) arg++;
	QString posFile(argv[arg++]);
	ssc::PositionStorageReader reader(posFile);
	QString startTS;
	if (argc == arg + 1)
	{
		startTS = QString(argv[arg++]);
	}
	else if (reader.version() == 1)
	{
		std::cout << "This is a version 1 of the record format, and requires a timestamp parameter.";
		return 0;
	}

  boost::uint64_t tsModifier = 0;
  if (reader.version() == 1)
  {
    QDateTime startTime = QDateTime::fromString(startTS, EVENT_DATE_FORMAT);
    boost::uint64_t ret64 = startTime.toTime_t();
    ret64 *= 1000;

    // workaround for compilling 32 bit version:
    tsModifier = ret64 & (std::numeric_limits<boost::uint64_t>::max() ^ 0xffffffff); // ffffffffffffffff XOR 00000000ffffffff = ffffffff00000000
    //boost::uint64_t tsModifier = ret64 & 0xffffffff00000000;
    std::cout << "Start time: " << startTS << ", converted to " << startTime.toString(EVENT_DATE_FORMAT) << std::endl;
  }

	
	ssc::Transform3D T = ssc::Transform3D::Identity();
	double timestamp;
	QString toolIndex;
	
//  QDateTime now = QDateTime::currentDateTime();
//  boost::uint64_t now_t64 = now.toTime_t();
//  now_t64 *= 1000;
//  now_t64 += now.time().msec();
//
//  std::cout << "now_t64 " << now_t64 << std::endl;
//  uint now_t = now.toTime_t()*1000 + now.time().msec(); //milliseconds
//  std::cout << "now.toTime_t() " << now.toTime_t() << std::endl;
//  std::cout << "now.toTime_t()*1000 + now.time().msec() " << now_t << std::endl;
//  std::cout << "test dir: " << ssc::PositionStorageReader::timestampToString(now_t64) << std::endl;
//	std::cout << "test now: " << ssc::PositionStorageReader::timestampToString(ssc::getMilliSecondsSinceEpoch()) << std::endl;
//  std::cout << "test now: " << ssc::PositionStorageReader::timestampToString(ssc::getMicroSecondsSinceEpoch()/1000) << std::endl;


	std::cout << "reading file [" << posFile.toStdString() << "]" << std::endl;
	if (!verbose)
		std::cout << "[index]\ttimestamp\tmatrix\ttoolIndex" << std::endl;
	int index = 0;
	while (!reader.atEnd())
	{
    if (reader.version() == 1)
    {
      int index = 0;
      if (!reader.read(&T, &timestamp, &index))
        break;
      toolIndex = QString(index);
    }
    else
    {
      if (!reader.read(&T, &timestamp, &toolIndex))
        break;
    }

		boost::uint64_t ts64 = (boost::uint64_t)timestamp;
		if (reader.version() == 1)
		{
			ts64 |= tsModifier;
		}

		if (verbose)
		{
			std::cout 
			 	<< "index:\t"<< index << '\t'
				<< "tool id:\t" << toolIndex.toStdString() << '\t'
//				<< "timestamp:\t" << timestamp << '\n'
				<< "timestamp:\t" << ssc::PositionStorageReader::timestampToString((double)ts64).toStdString() << '\n'
				<< "matrix:\n" << T << '\n' 
				<< std::endl;			
		}
		else
		{		
      std::cout << "[" << index << "]" << '\t';
      std::cout << ssc::PositionStorageReader::timestampToString((double)ts64).toStdString() << '\t';
			boost::array<double, 16>  val = T.flatten();
			ssc::stream_range(std::cout, val.begin(), val.end(), ' ');
      std::cout << '\t' << toolIndex.toStdString();
			std::cout << std::endl;
		}

		++index;
	}

	return 0;
}
