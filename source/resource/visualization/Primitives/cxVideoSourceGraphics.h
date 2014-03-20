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

#ifndef CXVIDEOSOURCEGRAPHICS_H_
#define CXVIDEOSOURCEGRAPHICS_H_

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
 * \ingroup cx_resource_visualization
 *
 * \date Oct 31, 2010
 * \date april 26, 2013
 * \author Christian Askeland, SINTEF
 */
class VideoSourceGraphics : public QObject
{
	Q_OBJECT
public:
	explicit VideoSourceGraphics(SpaceProviderPtr spaceProvider, bool useMaskFilter=false);
	virtual ~VideoSourceGraphics();

	void setRealtimeStream(VideoSourcePtr data);
	void setTool(ToolPtr tool);
	ToolPtr getTool();
	ProbeSector getProbeData();
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
	ProbeSector mProbeData;
	VideoSourcePtr mData;
	SpaceProviderPtr mSpaceProvider;
//	ImagePtr mImage;//Can be used instead of mTexture. This allows visualization of rt 3D
};

} // namespace cx

#endif // CXVIDEOSOURCEGRAPHICS_H_
