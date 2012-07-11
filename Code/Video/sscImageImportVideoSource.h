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
#ifndef SSCIMAGEIMPORTVIDEOSOURCE_H_
#define SSCIMAGEIMPORTVIDEOSOURCE_H_

#include "vtkSmartPointer.h"
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>
#include <boost/array.hpp>

#include "sscVideoSource.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;
class QTimer;

namespace ssc
{

/**\brief VideoSource controlled by a vtkImageImport
 *
 * Primitive implementation of the VideoSource interface.
 * Use the interface extensions to control the input:
 * Set a vtkImageImport, and call refresh() to emit a
 * signal.
 *
 * \ingroup sscVideo
 * \date April 11, 2012
 * \author Christian Askeland, SINTEF
 */
class ImageImportVideoSource : public VideoSource
{
	Q_OBJECT

public:
	ImageImportVideoSource( QString uid, QString name="");
	virtual ~ImageImportVideoSource();

	virtual QString getUid() { return mUid; }
	virtual QString getName() { return mName; }
	virtual vtkImageDataPtr getVtkImageData();
	virtual double getTimestamp();

	virtual QString getInfoString() const { return mInfo; }
	virtual QString getStatusString() const { return mStatus; }

	virtual void start();
	virtual void stop();

	virtual bool validData() const { return isStreaming() && mValidData; }
	virtual bool isConnected() const { return mConnected; }
	virtual bool isStreaming() const { return mConnected && mStreaming; }

	virtual void setConnected(bool state) { mConnected = state; }
	virtual void setValidData(bool valid) { mValidData = valid; }

	virtual void setResolution(double resolution);

	// extensions:
	vtkImageImportPtr getImageImport();
	void refresh(double timestamp);
	void setInfoString(QString text) { mInfo = text; }
	void setStatusString(QString text) { mStatus = text; }

private:
	void setEmptyImage();

	bool mConnected;
	bool mValidData;
	QString mUid;
	QString mName;
	double mResolution;
	vtkImageImportPtr mImageImport;
	bool mStreaming;
	double mTimestamp;
	boost::array<unsigned char, 100> mZero;
	QString mInfo;
	QString mStatus;
};

typedef boost::shared_ptr<ImageImportVideoSource> ImageImportVideoSourcePtr;

} // namespace ssc


#endif /* SSCIMAGEIMPORTVIDEOSOURCE_H_ */
