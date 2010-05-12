#ifndef SSCPOSITIONSTORAGEFILE_H_
#define SSCPOSITIONSTORAGEFILE_H_

#include <QString>
#include <QFile>
#include <QDataStream>

#include "sscTransform3D.h"

namespace ssc {

/**Reader class for the position file.
 * 
 * Each call to read() gives the next position entry from the file.
 * When atEnd() returns true, all positions have been read. 
 */
class PositionStorageReader
{
public:
	PositionStorageReader(QString filename);
	~PositionStorageReader();
	bool read(Transform3D* matrix, double* timestamp, int* toolIndex);
	bool atEnd() const;
	static QString timestampToString(double timestamp);
private:
	QFile positions;
	QDataStream stream;	
};

/** Writer class for the position file.
 * 
 * The generated file contains a compact representation
 * of tool position data along with timestamp and tool id.
 * Extract the info with class PositionStorageReader.
 */
class PositionStorageWriter
{
public:
	PositionStorageWriter(QString filename);
	~PositionStorageWriter();
	void write(Transform3D matrix, double timestamp, int toolIndex);
private:
	QFile positions;
	QDataStream stream;
};

} // namespace ssc 


#endif /*SSCPOSITIONSTORAGEFILE_H_*/
