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

/*
 * cxProbeAdapterRTSource.h
 *
 *  Created on: Feb 11, 2011
 *      Author: christiana
 */

#ifndef CXPROBEADAPTERRTSOURCE_H_
#define CXPROBEADAPTERRTSOURCE_H_

class QString;
#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "sscVideoSource.h"
#include "sscTool.h"

namespace ssc
{

/**\brief VideoSource that applies the parameters from a Probe to the VideoSource.
 *
 * Some metadata such as pixel spacing might not be correct when read directly
 * from the video source. The ProbeAdapterRTSource overrides some methods of
 * a VideoSource and inserts info from a Probe instead.
 *
 * \ingroup sscTool
 */
class ProbeAdapterRTSource: public ssc::VideoSource
{
Q_OBJECT
public:
	ProbeAdapterRTSource(QString uid, ProbePtr probe, VideoSourcePtr source);
	virtual ~ProbeAdapterRTSource()
	{
	}

	virtual QString getUid();
	virtual QString getName()
	{
		return mBase->getName();
	}
	virtual vtkImageDataPtr getVtkImageData();
	virtual double getTimestamp();

	virtual QString getInfoString() const
	{
		return mBase->getInfoString();
	}
	virtual QString getStatusString() const
	{
		return mBase->getStatusString();
	}

	virtual void start()
	{
		mBase->start();
	}
	virtual void stop()
	{
		mBase->stop();
	}

	virtual bool validData() const
	{
		return mBase->validData();
	}
	virtual bool isConnected() const
	{
		return mBase->isConnected();
	}
	virtual bool isStreaming() const
	{
		return mBase->isStreaming();
	}

private slots:
	void probeChangedSlot();
	void newFrameSlot();

private:
	QString mUid;
	ssc::VideoSourcePtr mBase;
	ssc::ProbeWeakPtr mProbe;
	vtkImageChangeInformationPtr mRedirecter;
};

}

#endif /* CXPROBEADAPTERRTSOURCE_H_ */
