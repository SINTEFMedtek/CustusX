/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
