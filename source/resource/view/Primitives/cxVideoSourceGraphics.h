/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
