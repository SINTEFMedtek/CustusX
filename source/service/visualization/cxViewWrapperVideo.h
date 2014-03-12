// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXVIEWWRAPPERRTSTREAM_H_
#define CXVIEWWRAPPERRTSTREAM_H_

#include <vector>
#include <QPointer>
#include "cxForwardDeclarations.h"
#include "sscDefinitions.h"

#include "cxViewWrapper.h"

namespace cx
{
/**
* \file
* \addtogroup cx_service_visualization
* @{
*/

/** Wrapper for a View that displays a RealTimeStream.
 *  Handles the connections between specific reps and the view.
 *
 *  The view displays either a raw rt source or a us probe, depending on
 *  whats available.
 *
 */
class ViewWrapperVideo: public ViewWrapper
{
Q_OBJECT
public:
	ViewWrapperVideo(ViewWidget* view, VisualizationServiceBackendPtr backend);
	virtual ~ViewWrapperVideo();
	virtual ViewWidget* getView();
	virtual void setSlicePlanesProxy(SlicePlanesProxyPtr proxy) {}
	virtual void updateView() {}
	virtual void setViewGroup(ViewGroupDataPtr group);

private slots:
	void updateSlot();
	void showSectorActionSlot(bool checked);
	void connectStream();
	void streamActionSlot();
protected slots:
	virtual void dataViewPropertiesChangedSlot(QString uid) {}

protected:
//	virtual void dataAdded(DataPtr data) {}
//	virtual void dataRemoved(const QString& uid) {}
	virtual void videoSourceChangedSlot(QString uid);

private:
	VideoSourcePtr getSourceFromService(QString uid);
	void addStreamAction(QString uid, QMenu* contextMenu);
	void loadStream();
	virtual void appendToContextMenu(QMenu& contextMenu);
	void addReps();
	void setupRep(VideoSourcePtr source, ToolPtr tool);

	VideoFixedPlaneRepPtr mStreamRep;
	VideoSourcePtr mSource;
	DisplayTextRepPtr mPlaneTypeText;
	DisplayTextRepPtr mDataNameText;
	QPointer<ViewWidget> mView;
	ToolPtr mTool;
};
typedef boost::shared_ptr<ViewWrapperVideo> ViewWrapperVideoPtr;

/**
* @}
*/
} // namespace cx

#endif /* CXVIEWWRAPPERRTSTREAM_H_ */
