#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <QtGui>

#include "sscTransform3D.h"
#include "sscPositionStorageFile.h"
#include "sscUtilHelpers.h"

/** 
 */
int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	
	if (argc<2)
	{
		std::cout << "usage: sscPositionFileReader [-v] <filename> " << std::endl;
		return 0;
	}

	QString posFile(argv[1]);
	bool verbose = argc>2 && QString(argv[1])=="-v";
	if (verbose)
	{
		posFile = QString(argv[2]);		
	}
	
	ssc::PositionStorageReader reader(posFile);
	
	ssc::Transform3D T;
	double timestamp;
	int toolIndex;
	
	std::cout << "reading file [" << posFile.toStdString() << "]" << std::endl;
	std::cout << "[index]\ttimestamp\ttoolIndex\tmatrix" << std::endl;
	int index = 0;
	while (!reader.atEnd())
	{
		reader.read(&T, &timestamp, &toolIndex);

		if (verbose)
		{
			std::cout 
			 	<< "index:\t"<< index << '\t'
				<< "tool id:\t" << toolIndex << '\t' 
				<< "timestamp:\t" << timestamp << '\n' 
				<< "matrix:\n" << T << '\n' 
				<< std::endl;			
		}
		else
		{		
			std::cout << "[" << index << "]\t" << timestamp << '\t' << toolIndex << '\t';
			boost::array<double, 16>  val = T.flatten();
			stream_range(std::cout, val.begin(), val.end(), ' ');
			std::cout << std::endl;
		}

		++index;
	}

	return 0;
}
