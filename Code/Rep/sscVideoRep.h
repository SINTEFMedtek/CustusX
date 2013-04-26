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

#ifndef SSCRTSTREAMREP_H_
#define SSCRTSTREAMREP_H_

#include "sscRepImpl.h"
#include "sscVtkHelperClasses.h"
#include "sscForwardDeclarations.h"
#include "sscProbeSector.h"

namespace ssc
{
typedef boost::shared_ptr<class ViewportListener> ViewportListenerPtr;
typedef boost::shared_ptr<class VideoSourceGraphics> VideoSourceGraphicsPtr;

typedef boost::shared_ptr<class VideoFixedPlaneRep> VideoFixedPlaneRepPtr;

/** \brief Display a VideoSource in a View.
 *
 * A rep visualizing a VideoSource directly into the view plane.
 * It does not follow the tool, but controls the camera in order to
 * fill the entire View.
 *
 * Used by CustusX.
 *
 * \ingroup sscRep
 * \ingroup sscRepVideo
 *
 * \date Oct 31, 2010
 * \author christiana
 */
class VideoFixedPlaneRep : public ssc::RepImpl
{
	Q_OBJECT
public:
	VideoFixedPlaneRep(const QString& uid, const QString& name="");
	virtual ~VideoFixedPlaneRep();
	virtual QString getType() const { return "ssc::RealTimeStreamFixedPlaneRep"; }
	void setRealtimeStream(VideoSourcePtr data);
	void setTool(ToolPtr tool);

	void setShowSector(bool on);
	bool getShowSector() const;

protected:
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);
	private slots:
	void newDataSlot();

private:
	void setCamera();
	void updateSector();

	VideoSourceGraphicsPtr mRTGraphics;
	bool mShowSector;

	ToolPtr mTool;
	ssc::ProbeSector mProbeData;
	VideoSourcePtr mData;

	ssc::TextDisplayPtr mStatusText;
	ssc::TextDisplayPtr mInfoText;

	GraphicalPolyData3DPtr mProbeSector;
	GraphicalPolyData3DPtr mProbeOrigin;
	GraphicalPolyData3DPtr mProbeClipRect;

	vtkRendererPtr mRenderer;
	View* mView;
	ViewportListenerPtr mViewportListener;
};


} // namespace ssc

#endif /* SSCRTSTREAMREP_H_ */
