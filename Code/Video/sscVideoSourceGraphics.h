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

#ifndef SSCVIDEOSOURCEGRAPHICS_H
#define SSCVIDEOSOURCEGRAPHICS_H

#include "sscForwardDeclarations.h"
#include "sscProbeSector.h"

namespace ssc
{
typedef boost::shared_ptr<class VideoGraphics> VideoGraphicsPtr;
typedef boost::shared_ptr<class VideoSourceGraphics> VideoSourceGraphicsPtr;

/** \brief Helper class for displaying a VideoSource.
 *
 * Used for Video display in VideoFixedPlaneRep and ToolRep3D.
 *
 *  Used by CustusX.
 *
 * \ingroup sscProxy
 *
 * \date Oct 31, 2010
 * \date april 26, 2013
 * \author christiana
 */
class VideoSourceGraphics : public QObject
{
	Q_OBJECT
public:
	VideoSourceGraphics(bool useMaskFilter=false);
	virtual ~VideoSourceGraphics();

	void setRealtimeStream(VideoSourcePtr data);
	void setTool(ToolPtr tool);
	ToolPtr getTool();
	ssc::ProbeSector getProbeData();
	/** Turn sector clipping on/off.
	 *  If on, only the area inside the probe sector is shown.
	 */
	void setClipToSector(bool on);
	void setShowInToolSpace(bool on);
	vtkActorPtr getActor();

signals:
	void newData();

private slots:
	void newDataSlot();
	void receiveTransforms(Transform3D matrix, double timestamp);
	void receiveVisible(bool visible);
	void probeSectorChanged();

private:
	bool mClipToSector;
	VideoGraphicsPtr mPipeline;
	bool mShowInToolSpace;
	ToolPtr mTool;
	ssc::ProbeSector mProbeData;
	VideoSourcePtr mData;
	ImagePtr mImage;//Can be used instead of mTexture. This allows visualization of rt 3D
};

} // namespace ssc

#endif // SSCVIDEOSOURCEGRAPHICS_H
