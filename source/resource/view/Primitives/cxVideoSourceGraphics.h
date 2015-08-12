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


#ifndef CXVIDEOSOURCEGRAPHICS_H_
#define CXVIDEOSOURCEGRAPHICS_H_

#include "cxResourceVisualizationExport.h"

#include "cxForwardDeclarations.h"
#include "cxProbeSector.h"

namespace cx
{
typedef boost::shared_ptr<class VideoGraphics> VideoGraphicsPtr;
typedef boost::shared_ptr<class VideoSourceGraphics> VideoSourceGraphicsPtr;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;

/** \brief Helper class for displaying a VideoSource.
 *
 * Used for Video display in VideoFixedPlaneRep and ToolRep3D.
 *
 * \ingroup cx_resource_view
 *
 * \date Oct 31, 2010
 * \date april 26, 2013
 * \author Christian Askeland, SINTEF
 */
class cxResourceVisualization_EXPORT VideoSourceGraphics : public QObject
{
	Q_OBJECT
public:
	explicit VideoSourceGraphics(SpaceProviderPtr spaceProvider, bool useMaskFilter=false);
	virtual ~VideoSourceGraphics();

	void setRealtimeStream(VideoSourcePtr data);
	void setTool(ToolPtr tool);
	ToolPtr getTool();
	ProbeSector getProbeDefinition();
	void setClipToSector(bool on); ///< Turn sector clipping on/off. If on, only the area inside the probe sector is shown.
	void setShowInToolSpace(bool on);
	vtkActorPtr getActor();

signals:
	void newData();

private slots:
	void newDataSlot();
	void receiveTransforms(Transform3D matrix, double timestamp);
	void probeSectorChanged();

private:
	bool mClipToSector;
	VideoGraphicsPtr mPipeline;
	bool mShowInToolSpace;
	ToolPtr mTool;
	ProbeSector mProbeDefinition;
	VideoSourcePtr mData;
	SpaceProviderPtr mSpaceProvider;
};

} // namespace cx

#endif // CXVIDEOSOURCEGRAPHICS_H_
