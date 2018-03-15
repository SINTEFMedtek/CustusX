/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXPROBEADAPTERRTSOURCE_H_
#define CXPROBEADAPTERRTSOURCE_H_

#include "cxResourceExport.h"

#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "cxVideoSource.h"
#include "cxTool.h"

class QString;

namespace cx
{

/**
 * \brief VideoSource that applies the parameters from a Probe to the VideoSource.
 *
 * Some metadata such as pixel spacing might not be correct when read directly
 * from the video source. The ProbeAdapterRTSource overrides some methods of
 * a VideoSource and inserts info from a Probe instead.
 *
 * \ingroup cx_resource_core_tool
*
 * \date Feb 11, 2011
 * \author: Christian Askeland, SINTEF
 */
class cxResource_EXPORT ProbeAdapterRTSource: public VideoSource
{
	Q_OBJECT

public:
	ProbeAdapterRTSource(QString uid, ProbePtr probe, VideoSourcePtr source);
	virtual ~ProbeAdapterRTSource();

	virtual QString getUid();
	virtual QString getName();
	virtual vtkImageDataPtr getVtkImageData();
	virtual double getTimestamp();
	virtual TimeInfo getAdvancedTimeInfo();

	virtual QString getInfoString() const;
	virtual QString getStatusString() const;

	virtual void start();
	virtual void stop();
	virtual bool validData() const;
	virtual bool isConnected() const;
	virtual bool isStreaming() const;
	VideoSourcePtr getBaseSource();

private slots:
	void probeChangedSlot();
	void newFrameSlot(); ///< Check the image sizes of the probe definition and the image. If different: resample the probe def to match the image

private:
	QString mUid;
	VideoSourcePtr mBase;
	ProbeWeakPtr mProbe;
	vtkImageChangeInformationPtr mRedirecter;
};

typedef boost::shared_ptr<ProbeAdapterRTSource> ProbeAdapterRTSourcePtr;

} // cx

#endif /* CXPROBEADAPTERRTSOURCE_H_ */
