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
#ifndef CXTESTVIDEOSOURCE_H_
#define CXTESTVIDEOSOURCE_H_

#include "cxResourceExport.h"

#include "vtkSmartPointer.h"
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>

#include "cxVideoSource.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;
class QTimer;

namespace cx
{

/** \brief test video source generator
 *
 * This class implements the VideoSource interface and generates
 * a moving test pattern. The color change front moves diagonally from
 * lower right to upper left corner. There are vertical black lines
 * every 10mm
 *
 *
 * \ingroup cx_resource_core_video
 */
class cxResource_EXPORT TestVideoSource : public VideoSource
{
	Q_OBJECT

public:
	TestVideoSource( QString uid, QString name, int width, int height);
	virtual ~TestVideoSource();

	virtual QString getUid() { return mUid; }
	virtual QString getName() { return mName; }
	virtual vtkImageDataPtr getVtkImageData();
	virtual double getTimestamp();
	virtual TimeInfo getAdvancedTimeInfo();

	virtual QString getInfoString() const { return QString("TestVideo Info"); }
	virtual QString getStatusString() const { return QString("TestVideo Status"); }

	virtual void start();
	virtual void stop();

	virtual bool validData() const { return isStreaming() && mInitialized && mValidData; }
	virtual bool isConnected() const { return mConnected; }
	virtual bool isStreaming() const { return mConnected && mStreaming; }

	virtual void setConnected(bool state) { mConnected = state; }
	virtual void setValidData(bool valid) { mValidData = valid; }

	virtual void setResolution(double resolution);
private slots:
	void processBuffer();
private:
	bool mConnected;
	bool mValidData;
	QString mUid;
	QString mName;
	int mFrames;
	double mResolution;
	vtkImageImportPtr mImageImport;
	int mWidth;
	int mHeight;
	bool mStreaming;
	QTimer *mImageTimer;
	uint8_t *mBuffer;
	bool mInitialized;
};
typedef boost::shared_ptr<TestVideoSource> TestVideoSourcePtr;

} // namespace cx

#endif /* CXTESTVIDEOSOURCE_H_ */
