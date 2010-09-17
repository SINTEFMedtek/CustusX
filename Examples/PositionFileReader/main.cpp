#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <QtGui>
#include <boost/cstdint.hpp>

#include "sscTransform3D.h"
#include "sscPositionStorageFile.h"
#include "sscUtilHelpers.h"

#define EVENT_DATE_FORMAT "yyyyMMddHHmmss.zzz000"

/** 
 */
int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	
	if (argc<3)
	{
		std::cout << "usage: sscPositionFileReader [-v] <filename> <timestamp>" << std::endl;
		return 0;
	}

	QString posFile(argv[1]);
	QString startTS(argv[2]);
	bool verbose = argc>2 && QString(argv[1])=="-v";
	if (verbose)
	{
		posFile = QString(argv[2]);		
		startTS = QString(argv[3]);
	}
	
	QDateTime startTime = QDateTime::fromString(startTS, EVENT_DATE_FORMAT);
	boost::uint64_t ret64 = startTime.toTime_t();
	ret64 *= 1000;

	// workaround for compilling 32 bit version:
	boost::uint64_t tsModifier = ret64 & std::numeric_limits<boost::uint64_t>::max() & 0x00000000;
	//boost::uint64_t tsModifier = ret64 & 0xffffffff00000000;

	ssc::PositionStorageReader reader(posFile);
	
	ssc::Transform3D T;
	double timestamp;
	int toolIndex;
	
	std::cout << "reading file [" << posFile.toStdString() << "]" << std::endl;
	if (!verbose)
		std::cout << "[index]\ttoolIndex\ttimestamp\tmatrix" << std::endl;
	int index = 0;
	while (!reader.atEnd())
	{
		reader.read(&T, &timestamp, &toolIndex);
		boost::uint64_t ts64 = (boost::uint64_t)timestamp;
		ts64 |= tsModifier;

		if (verbose)
		{
			std::cout 
			 	<< "index:\t"<< index << '\t'
				<< "tool id:\t" << toolIndex << '\t' 
//				<< "timestamp:\t" << timestamp << '\n'
				<< "timestamp:\t" << ssc::PositionStorageReader::timestampToString((double)ts64).toStdString() << '\n'
				<< "matrix:\n" << T << '\n' 
				<< std::endl;			
		}
		else
		{		
			std::cout << "[" << index << "]\t" << toolIndex << '\t' << ssc::PositionStorageReader::timestampToString((double)ts64).toStdString() << '\t';
			boost::array<double, 16>  val = T.flatten();
			ssc::stream_range(std::cout, val.begin(), val.end(), ' ');
			std::cout << std::endl;
		}

		++index;
	}

	return 0;
}
