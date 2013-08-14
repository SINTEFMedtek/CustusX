// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2012- SINTEF Medical Technology
// Copyright (C) 2012- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.


#ifndef SSCTESTVIDEOSOURCE_H_
#define SSCTESTVIDEOSOURCE_H_

#include "vtkSmartPointer.h"
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>

#include "sscVideoSource.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;
class QTimer;

namespace ssc
{

/**\brief test video source generator
 *
 * This class implements the ssc::VideoSource interface and generates
 * a moving test pattern. The color change front moves diagonally from
 * lower right to upper left corner. There are vertical black lines
 * every 10mm
 *
 *
 * \ingroup sscVideo
 */
class TestVideoSource : public VideoSource
{
	Q_OBJECT

public:
	TestVideoSource( QString uid, QString name, int width, int height);
	virtual ~TestVideoSource();

	virtual QString getUid() { return mUid; }
	virtual QString getName() { return mName; }
	virtual vtkImageDataPtr getVtkImageData();
	virtual double getTimestamp();

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

} // namespace ssc

#endif /* SSCTESTVIDEOSOURCE_H_ */
