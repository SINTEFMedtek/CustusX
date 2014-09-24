/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXPOSITIONSTORAGEFILE_H_
#define CXPOSITIONSTORAGEFILE_H_

#include "cxResourceExport.h"

#include <QString>
#include <QFile>
#include <QDataStream>
#include <boost/cstdint.hpp>

#include "cxTransform3D.h"

namespace cx {

/**\brief Reader class for the position file.
 * 
 * Each call to read() gives the next position entry from the file.
 * When atEnd() returns true, all positions have been read. 
 *
 *
 *
 *
 * Binary file format description
   \verbatim
  Header:
    "SNWPOS"<version>

  Entries, can be one of:

   * Change tool. All position entries following this one belongs to the given tool
       <type=2><size><toolUid>

   * Position. integer-format tool id are appended.
      <type=1><size><timestamp><position><int toolid>

   * Position. Requires change tool to have been called.
      <type=3><size><timestamp><position>

   The position field is <position> = <thetaXY><thetaZ><phi><x><y><z>
   Where the parameters are found from a matrix using the class CGFrame.
   \endverbatim
 *
 * \sa PositionStorageWriter
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT PositionStorageReader
{
public:
	PositionStorageReader(QString filename);
	~PositionStorageReader();
	bool read(Transform3D* matrix, double* timestamp, int* toolIndex); // reads only tool data in integer format.
	bool read(Transform3D* matrix, double* timestamp, QString* toolUid);
	bool atEnd() const;
	static QString timestampToString(double timestamp);
	int version();
private:
	QString mCurrentToolUid; ///< the tool currently being written.
	QFile positions;
	QDataStream stream;
	quint8 mVersion;
	bool mError;
	class Frame3D frameFromStream();
};

typedef boost::shared_ptr<PositionStorageReader> PositionStorageReaderPtr;

/**\brief Writer class for the position file.
 * 
 * The generated file contains a compact representation
 * of tool position data along with timestamp and tool id.
 * Extract the info with class PositionStorageReader.
 *
 * For a description of the file format, see PositionStorageReader.
 *
 * \sa PositionStorageReader
 * \ingroup sscUtility
 */
class cxResource_EXPORT PositionStorageWriter
{
public:
	PositionStorageWriter(QString filename);
	~PositionStorageWriter();
	void write(Transform3D matrix, uint64_t timestamp, int toolIndex);
	void write(Transform3D matrix, uint64_t timestamp, QString toolUid);
private:
	QString mCurrentToolUid; ///< the tool currently being written.
	QFile positions;
	QDataStream stream;
};

} // namespace cx 


#endif /*CXPOSITIONSTORAGEFILE_H_*/
