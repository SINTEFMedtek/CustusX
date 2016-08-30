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

#ifndef CXVIEWMANAGER_H_
#define CXVIEWMANAGER_H_

#include "org_custusx_core_view_Export.h"

#include <map>
#include <vector>
#include <QObject>

#include "cxData.h"
#include "cxDefinitions.h"
#include "cxForwardDeclarations.h"
//#include "cxLayoutData.h"
#include "cxViewService.h"

class QActionGroup;
class QAction;
class QGridLayout;
class QWidget;
class QTime;

namespace cx
{
class LayoutData;
struct LayoutViewData;
struct LayoutRegion;
class ViewCollectionWidget;
class ViewWrapper;
typedef boost::shared_ptr<class SyncedValue> SyncedValuePtr;
typedef boost::shared_ptr<class InteractiveCropper> InteractiveCropperPtr;
typedef boost::shared_ptr<class CyclicActionLogger> CyclicActionLoggerPtr;
typedef boost::shared_ptr<class CameraStyleInteractor> CameraStyleInteractorPtr;
typedef boost::shared_ptr<class RenderLoop> RenderLoopPtr;
typedef boost::shared_ptr<class LayoutRepository> LayoutRepositoryPtr;
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class Navigation> NavigationPtr;
typedef boost::shared_ptr<class CameraControl> CameraControlPtr;

typedef boost::shared_ptr<class ViewManager> ViewManagerPtr;

/**
 * \file
 * \addtogroup org_custusx_core_view
 * @{
 */



/**
 * \class ViewManager
 *
 * \brief Creates a pool of views and offers an interface to them, also handles
 * layouts on a centralwidget.
 *
 * \image html cxArchitecture_visualization.png "Overview of ViewManager and Views"
 *
 * The primitive element is the View. Various derivations of ViewWrapper
 * controls each view and fills them with Reps. ViewWrapper2D controls a 2D
 * slice, ViewWrapper3D controls a full 3D scene, ViewWrapperRTSource shows a
 * video stream.
 *
 * ViewManager handles the visualization of the 3D data. It composes the Views
 * onto the QWidget using a QGridLayout. This is called a Layout in the GUI.
 * Available layouts can be found inside ViewManager.  The QWidget is the
 * centralWidget in the MainWindow.
 *
 * The views are divided into several groups. Each group, represented by a
 * ViewGroup, has some common characteristics, such as which data to display.
 * These data are stored in a ViewGroupData. The View/ViewWrappers connected
 * to a group, uses the ViewGroupData to know what they should visualize. Each
 * ViewWrapper formats the data in their own way (as slices or 3D renderings).
 *
 * The right-click menu in the Views are created inside the ViewWrappers. The
 * data they manipulate are (mostly) stored within ViewGroupData.
 *
 * \image html cxArchitecture_view_layout.png "Pipeline from Data to rendering in Views, organization in groups and layout"
 *
 *
 * \section ViewManager_section_layout_example Layout Example
 *
 * The Layout uses QGridLayout to organize the views. Each view belong to a
 * group that show the same data in different ways (for example: 3D+ACS).
 * The layout and grouping can be reconfigured in the OR.
 *
 *  - Group 0 displays A MR volume with an US overlay and segmented vessels.
 *  - Group 1 displays only the MR volume.
 *
 * \image html LayoutExample.png "Layout example, schematic view."
 * \image html metastase_mr_us_small.png "Layout Example, rendered view."
 *
 *
 * \date Dec 9, 2008
 * \date Jan 19, 2012
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_view_EXPORT ViewManager: public QObject
{
Q_OBJECT
public:
	static ViewManagerPtr create(VisServicesPtr backend);
	virtual ~ViewManager();

	ViewPtr get3DView(int group = 0, int index = 0);

	virtual ViewGroupDataPtr getViewGroup(int groupIdx) const;
	unsigned viewGroupCount() const
	{
		int count = 0;
		while(this->getViewGroup(count))
			++count;
		return count;
	}
	virtual int getActiveViewGroup() const;

	LayoutRepositoryPtr getLayoutRepository();

	QActionGroup* getInteractorStyleActionGroup();
	NavigationPtr getNavigation(int group = 0);

	/** Initialize the widget and fill with the default view layout.
	  * Return the top widget, it should be added to the calling gui.
	  */
//	void initialize();
	QWidget* createLayoutWidget(QWidget* parent, int index);
    QWidget* getLayoutWidget(int index);

	void enableRender(bool val);
	bool renderingIsEnabled() const;

	QString getActiveLayout(int widgetIndex=0) const; ///< returns the active layout
	void setActiveLayout(const QString& uid, int widgetIndex=0); ///< change the layout

	InteractiveCropperPtr getCropper();

	CyclicActionLoggerPtr getRenderTimer();

	void deactivateCurrentLayout();///< deactivate the current layout, leaving an empty layout
	void autoShowData(DataPtr data);
	CameraControlPtr getCameraControl() { return mCameraControl; }
	void clear();
	//Interface for saving/loading
	void addXml(QDomNode& parentNode);
	void parseXml(QDomNode viewmanagerNode);

	void centerToImageCenterInViewGroup(unsigned groupNr);
	void setCameraStyle(CAMERA_STYLE_TYPE style, int groupIdx);
	void addDefaultLayout(LayoutData layoutData);

	void enableContextMenuForViews(bool enable=true);
signals:
	void fps(int number); ///< Emits number of frames per second
	void activeLayoutChanged(); ///< emitted when the active layout changes
	void activeViewChanged(); ///< emitted when the active view changes
	void renderingEnabledChanged(); ///< emitted then rendering is enabled/disabled
	void pointSampled(Vector3D p_r);
	void renderFinished();


protected slots:
	void settingsChangedSlot(QString key);
	void onLayoutRepositoryChanged(QString uid);

	void updateViews();
	void updateCameraStyleActions();
	void setActiveView(QString viewUid);
	void layoutWidgetDestroyed(QObject *object);

protected:
	ViewManager(VisServicesPtr backend);
	std::vector<ViewGroupPtr> getViewGroups() { return mViewGroups; }

	void activateView(ViewCollectionWidget* widget, LayoutViewData viewData);
	ViewWrapperPtr createViewWrapper(ViewPtr view, LayoutViewData viewData);
	int findGroupContaining3DViewGivenGuess(int preferredGroup);
	QString getActiveView() const; ///< returns the active view

	void setRenderingInterval(int interval);
	void setSlicePlanesProxyInViewsUpTo2DViewgroup();

	void loadGlobalSettings();
	void saveGlobalSettings();
	void activateViews(ViewCollectionWidget *widget, LayoutData next);
	void rebuildLayouts();
	void initializeGlobal2DZoom();
	void initializeActiveView();

	LayoutRepositoryPtr mLayoutRepository;
	std::vector<QPointer<ViewCollectionWidget> > mLayoutWidgets;
	QStringList mActiveLayout; ///< the active layout (type)
	SyncedValuePtr mActiveView;
	RenderLoopPtr mRenderLoop;
	std::vector<ViewGroupPtr> mViewGroups;
	CameraControlPtr mCameraControl;

	bool mGlobalObliqueOrientation; ///< controlling whether or not all 2d views should be oblique or orthogonal
	SyncedValuePtr mGlobal2DZoomVal;

	InteractiveCropperPtr mInteractiveCropper;
	SlicePlanesProxyPtr mSlicePlanesProxy;

	CameraStyleInteractorPtr mCameraStyleInteractor;
	VisServicesPtr mBackend;

private:
	ViewManager(ViewManager const&);
	ViewManager& operator=(ViewManager const&);
protected:
	void autoShowInViewGroups(DataPtr data);
	void autoResetCameraToSuperiorView();
	void autoCenterToImageCenter();
	QList<unsigned> getViewGroupsToAutoShowIn();
};

/**
 * @}
 */
} //namespace

#endif /* CXVIEWMANAGER_H_ */
