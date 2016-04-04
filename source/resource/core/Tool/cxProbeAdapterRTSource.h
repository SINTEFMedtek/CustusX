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
