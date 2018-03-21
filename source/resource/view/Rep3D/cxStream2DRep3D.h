/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CX2DSTREAMREP3D_H
#define CX2DSTREAMREP3D_H

#include "cxResourceVisualizationExport.h"
#include "cxRepImpl.h"
#include "cxForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class VideoSourceGraphics> VideoSourceGraphicsPtr;

/** \brief Display a stream as 2D in 3D
 *
 * The stream can be either 2D or 3D
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 *
 * \date Sep 10, 2015
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResourceVisualization_EXPORT Stream2DRep3D : public RepImpl
{
    Q_OBJECT
public:
    static Stream2DRep3DPtr New(SpaceProviderPtr spaceProvider, const QString& uid = "");
    virtual QString getType() const;
	void setTrackedStream(TrackedStreamPtr trackedStream);
	bool isReady();
protected:
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);
private slots:
	void trackedStreamChanged();
private:
    Stream2DRep3D(SpaceProviderPtr spaceProvider);

    SpaceProviderPtr mSpaceProvider;
    VideoSourceGraphicsPtr mRTStream;
    TrackedStreamPtr mTrackedStream;
};

} //cx

#endif // CX2DSTREAMREP3D_H
