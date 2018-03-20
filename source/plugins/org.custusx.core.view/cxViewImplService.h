/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWIMPLSERVICE_H_
#define CXVIEWIMPLSERVICE_H_

#include "cxViewService.h"
#include "org_custusx_core_view_Export.h"
class ctkPluginContext;
class QDomElement;

namespace cx
{
class ViewCollectionWidget;
typedef boost::shared_ptr<class SessionStorageService> SessionStorageServicePtr;
typedef boost::shared_ptr<class SyncedValue> SyncedValuePtr;
typedef boost::shared_ptr<class RenderLoop> RenderLoopPtr;
typedef boost::shared_ptr<class CameraStyleInteractor> CameraStyleInteractorPtr;


/**
 * Implementation of ViewService.
 *
 * \ingroup org_custusx_core_view
 *
 * \brief Creates a pool of views and offers an interface to them, also handles
 * layouts on a centralwidget.
 *
 * \image html cxArchitecture_visualization.png "Overview of ViewManager(=ViewService) and Views"
 *
 * The primitive element is the View. Various derivations of ViewWrapper
 * controls each view and fills them with Reps. ViewWrapper2D controls a 2D
 * slice, ViewWrapper3D controls a full 3D scene, ViewWrapperRTSource shows a
 * video stream.
 *
 * ViewService handles the visualization of the 3D data. It composes the Views
 * onto the QWidget using a QGridLayout. This is called a Layout in the GUI.
 * Available layouts can be found inside ViewService.  The QWidget is the
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
 * \section ViewService_section_layout_example Layout Example
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
 * \date 2014-09-19
 * \date 2016-12-02
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */

class org_custusx_core_view_EXPORT ViewImplService : public ViewService
{
	Q_OBJECT
	Q_INTERFACES(cx::ViewService)
public:
	ViewImplService(ctkPluginContext* context);
	virtual ~ViewImplService();

	virtual ViewPtr get3DView(int group = 0, int index = 0);

	virtual int getActiveGroupId() const;
	virtual ViewGroupDataPtr getGroup(int groupIdx) const;
	virtual void setRegistrationMode(REGISTRATION_STATUS mode);

	virtual void autoShowData(DataPtr data);
	virtual void enableRender(bool val);
	virtual bool renderingIsEnabled() const;

    virtual QWidget* createLayoutWidget(QWidget* parent, int index);
    virtual QWidget* getLayoutWidget(int index);
	virtual QString getActiveLayout(int widgetIndex = 0) const;
	virtual void setActiveLayout(const QString& uid, int widgetIndex);
	virtual ClippersPtr getClippers();
	virtual InteractiveCropperPtr getCropper();
	virtual CyclicActionLoggerPtr getRenderTimer();
	virtual NavigationPtr getNavigation(int group = 0);
	virtual LayoutRepositoryPtr getLayoutRepository();
	virtual CameraControlPtr getCameraControl();
	virtual QActionGroup* getInteractorStyleActionGroup();
	virtual void centerToImageCenterInActiveViewGroup();
	virtual void addDefaultLayout(LayoutData layoutData);
	virtual void enableContextMenuForViews(bool enable=true);

	virtual bool isNull();

	virtual void setCameraStyle(CAMERA_STYLE_TYPE style, int groupIdx);
	virtual void zoomCamera3D(int viewGroup3DNumber, int zoomFactor);

public slots:
    virtual void aboutToStop();

protected slots:
	void layoutWidgetDestroyed(QObject *object);

private slots:
	void onSessionChanged();
	void onSessionCleared();
	void onSessionLoad(QDomElement& node);
	void onSessionSave(QDomElement& node);

	void updateViews();
	void updateCameraStyleActions();
	void onLayoutRepositoryChanged(QString uid);
	void setActiveView(QString viewUid);
	void settingsChangedSlot(QString key);

protected:
	void rebuildLayouts();
	QList<unsigned> getViewGroupsToAutoShowIn();

	VisServicesPtr mServices;
	std::vector<QPointer<ViewCollectionWidget> > mLayoutWidgets;
	RenderWindowFactoryPtr mRenderWindowFactory;
	RenderLoopPtr mRenderLoop;

private:
	void init();
	void loadGlobalSettings();
	void saveGlobalSettings();
	void initializeGlobal2DZoom();
	void initializeActiveView();
	std::vector<ViewGroupPtr> getViewGroups();
	QString getActiveView() const; ///< returns the active view

	//Interface for saving/loading
	void addXml(QDomNode& parentNode);
	void parseXml(QDomNode viewmanagerNode);
	void clear();
	void deactivateCurrentLayout();///< deactivate the current layout, leaving an empty layout
	unsigned viewGroupCount() const;
	void activateViews(ViewCollectionWidget *widget, LayoutData next);
	void activateView(ViewCollectionWidget* widget, LayoutViewData viewData);
	void setSlicePlanesProxyInViewsUpTo2DViewgroup();
	void setRenderingInterval(int interval);
	ViewWrapperPtr createViewWrapper(ViewPtr view, LayoutViewData viewData);
	int findGroupContaining3DViewGivenGuess(int preferredGroup);
	void autoShowInViewGroups(DataPtr data);
	void autoResetCameraToSuperiorView();
	void autoCenterToImageCenter();
	void centerToImageCenterInViewGroup(unsigned groupNr);

	ctkPluginContext *mContext;
	SessionStorageServicePtr mSession;
	ClippersPtr mClippers;

	//From old ViewManager
	LayoutRepositoryPtr mLayoutRepository;
	QStringList mActiveLayout; ///< the active layout (type)
	SyncedValuePtr mActiveView;
	std::vector<ViewGroupPtr> mViewGroups;
	CameraControlPtr mCameraControl;

	bool mGlobalObliqueOrientation; ///< controlling whether or not all 2d views should be oblique or orthogonal
	SyncedValuePtr mGlobal2DZoomVal;

	InteractiveCropperPtr mInteractiveCropper;
	SlicePlanesProxyPtr mSlicePlanesProxy;

	CameraStyleInteractorPtr mCameraStyleInteractor;

};
typedef boost::shared_ptr<ViewImplService> ViewImplServicePtr;

} /* namespace cx */

#endif /* CXVIEWIMPLSERVICE_H_ */

