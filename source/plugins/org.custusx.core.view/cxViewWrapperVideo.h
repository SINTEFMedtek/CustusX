/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWWRAPPERRTSTREAM_H_
#define CXVIEWWRAPPERRTSTREAM_H_

#include "org_custusx_core_view_Export.h"

#include <vector>
#include <QPointer>
#include "cxForwardDeclarations.h"
#include "cxDefinitions.h"

#include "cxViewWrapper.h"

namespace cx
{
/**
* \file
* \addtogroup org_custusx_core_view
* @{
*/

/** Wrapper for a View that displays a RealTimeStream.
 *  Handles the connections between specific reps and the view.
 *
 *  The view displays either a raw rt source or a us probe, depending on
 *  whats available.
 *
 */
class org_custusx_core_view_EXPORT ViewWrapperVideo: public ViewWrapper
{
Q_OBJECT
public:
	ViewWrapperVideo(ViewPtr view, VisServicesPtr services);
	virtual ~ViewWrapperVideo();
	virtual ViewPtr getView();
	virtual void setSlicePlanesProxy(SlicePlanesProxyPtr proxy) {}
	virtual void setViewGroup(ViewGroupDataPtr group);

private slots:
	void updateSlot();
	void showSectorActionSlot(bool checked);
	void connectStream();
	void streamActionSlot();
protected slots:
	virtual void dataViewPropertiesChangedSlot(QString uid) {}
	virtual void videoSourceChangedSlot(QString uid);

	virtual QString getDataDescription();
	virtual QString getViewDescription();
private:
	VideoSourcePtr getSourceFromService(QString uid);
	void addStreamAction(QString uid, QMenu* contextMenu);
	void loadStream();
	virtual void appendToContextMenu(QMenu& contextMenu);
	void setupRep(VideoSourcePtr source, ToolPtr tool);

	VideoFixedPlaneRepPtr mStreamRep;
	VideoSourcePtr mSource;
	ViewPtr mView;
	ToolPtr mTool;
};
typedef boost::shared_ptr<ViewWrapperVideo> ViewWrapperVideoPtr;

/**
* @}
*/
} // namespace cx

#endif /* CXVIEWWRAPPERRTSTREAM_H_ */
