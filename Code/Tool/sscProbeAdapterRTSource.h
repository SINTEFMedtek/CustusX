// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
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

#ifndef CXPROBEADAPTERRTSOURCE_H_
#define CXPROBEADAPTERRTSOURCE_H_

#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "sscVideoSource.h"
#include "sscTool.h"

class QString;

namespace ssc
{

/**
 * \brief VideoSource that applies the parameters from a Probe to the VideoSource.
 *
 * Some metadata such as pixel spacing might not be correct when read directly
 * from the video source. The ProbeAdapterRTSource overrides some methods of
 * a VideoSource and inserts info from a Probe instead.
 *
 * \ingroup sscTool
*
 * \date Feb 11, 2011
 * \author: Christian Askeland, SINTEF
 */
class ProbeAdapterRTSource: public ssc::VideoSource
{
	Q_OBJECT

public:
	ProbeAdapterRTSource(QString uid, ProbePtr probe, VideoSourcePtr source);
	virtual ~ProbeAdapterRTSource();

	virtual QString getUid();
	virtual QString getName();
	virtual vtkImageDataPtr getVtkImageData();
	virtual double getTimestamp();

	virtual QString getInfoString() const;
	virtual QString getStatusString() const;

	virtual void start();
	virtual void stop();
	virtual bool validData() const;
	virtual bool isConnected() const;
	virtual bool isStreaming() const;
	ssc::VideoSourcePtr getBaseSource();

private slots:
	void probeChangedSlot();
	void newFrameSlot(); ///< Check the image sizes of the probe definition and the image. If different: resample the probe def to match the image

private:
	QString mUid;
	ssc::VideoSourcePtr mBase;
	ssc::ProbeWeakPtr mProbe;
	vtkImageChangeInformationPtr mRedirecter;
};

}

#endif /* CXPROBEADAPTERRTSOURCE_H_ */
