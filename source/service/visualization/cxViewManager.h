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

#ifndef CXVIEWMANAGER_H_
#define CXVIEWMANAGER_H_

#include <map>
#include <vector>
#include <QObject>

#include "sscData.h"
#include "sscDefinitions.h"
#include "cxForwardDeclarations.h"

class QActionGroup;
class QAction;
class QGridLayout;
class QWidget;
class QTimer;
class QTime;

namespace cx
{
class LayoutData;
struct LayoutRegion;
class LayoutWidget;
class ViewWrapper;
typedef boost::shared_ptr<class SyncedValue> SyncedValuePtr;
typedef boost::shared_ptr<class InteractiveCropper> InteractiveCropperPtr;
typedef boost::shared_ptr<class InteractiveClipper> InteractiveClipperPtr;
typedef boost::shared_ptr<class CyclicActionLogger> CyclicActionLoggerPtr;
typedef boost::shared_ptr<class CameraStyle> CameraStylePtr;
typedef boost::shared_ptr<class RenderLoop> RenderLoopPtr;

/**
 * \file
 * \addtogroup cxServiceVisualization
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
class ViewManager: public QObject
{
Q_OBJECT
public:

	ViewWidgetQPtr get3DView(int group = 0, int index = 0);
	std::vector<ViewGroupPtr> getViewGroups() { return mViewGroups; }

	LayoutData getLayoutData(const QString uid) const; ///< get data for given layout
	std::vector<QString> getAvailableLayouts() const; ///< get uids of all defined layouts
	void setLayoutData(const LayoutData& data); ///< add or edit a layout
	QString generateLayoutUid() const; ///< return an uid not used in present layouts.
	void deleteLayoutData(const QString uid);
	QActionGroup* createInteractorStyleActionGroup();
	bool isCustomLayout(const QString& uid) const;

	static ViewManager* createInstance(); ///< create the instance
	static ViewManager* getInstance(); ///< returns the only instance of this class, NULL unless createInstance has been called.
	static void destroyInstance(); ///< destroys the only instance of this class

	/** Initialize the widget and fill with the default view layout.
	  * Return the top widget, it should be added to the calling gui.
	  */
	void initialize();
	QWidget* getLayoutWidget(int index=0);

	void setRegistrationMode(REGISTRATION_STATUS mode);

	QString getActiveLayout(int widgetIndex=0) const; ///< returns the active layout
	void setActiveLayout(const QString& uid, int widgetIndex=0); ///< change the layout

	ViewWrapperPtr getActiveView() const; ///< returns the active view
	int getActiveViewGroup() const;
	void setActiveView(QString viewUid); ///< convenient function for setting the active view
	void storeLayoutData(const LayoutData& data);

	void setGlobal2DZoom(bool global); ///< enable/disable global 2d zooming
	bool getGlobal2DZoom(); ///< find out if global 2D zooming is enable

	InteractiveClipperPtr getClipper();
	InteractiveCropperPtr getCropper();

	CyclicActionLoggerPtr getRenderTimer();// { return mRenderTimer; }
	CameraStylePtr getCameraStyle() { return mCameraStyle; }

	void deactivateCurrentLayout();///< deactivate the current layout, leaving an empty layout
	void autoShowData(DataPtr data);

	/**
	 * Return a list of all images used in viewGroups
	 * Uses a map to remove duplicates
	 */
	std::map<QString, ImagePtr> getVisibleImages();

signals:
	void fps(int number); ///< Emits number of frames per second
	void activeLayoutChanged(); ///< emitted when the active layout changes
	void activeViewChanged(); ///< emitted when the active view changes

protected slots:
	void settingsChangedSlot(QString key);

	void clearSlot();
	void duringSavePatientSlot();
	void duringLoadPatientSlot();
	void updateViews();

protected:
	ViewManager(); ///< create all needed views
	virtual ~ViewManager();

	//Interface for saving/loading
	void addXml(QDomNode& parentNode); ///< adds xml information about the viewmanager and its variables
	void parseXml(QDomNode viewmanagerNode); ///< Use a XML node to load data. \param viewmanagerNode A XML data representation of the ViewManager

	ViewWidget* getView(const QString& uid); ///< returns the view with the given uid, use getType to determine if it's a 2D or 3D view

	void syncOrientationMode(SyncedValuePtr val);

	void activateView(LayoutWidget* widget, ViewWrapperPtr wrapper, int group, LayoutRegion region);
	void activate2DView(LayoutWidget *widget, int group, PLANE_TYPE plane, LayoutRegion region);
	void activate3DView(LayoutWidget *widget, int group, LayoutRegion region);
	void activateRTStreamView(LayoutWidget *widget, int group, LayoutRegion region);
	void addDefaultLayouts();
	unsigned findLayoutData(const QString uid) const;
	void addDefaultLayout(LayoutData data);
	void setRenderingInterval(int interval);

	void loadGlobalSettings();
	void saveGlobalSettings();
	void activateViews(LayoutWidget *widget, LayoutData next);
	void rebuildLayouts();

	static ViewManager* mTheInstance; ///< the only instance of this class

	typedef std::vector<LayoutData> LayoutDataVector;
	LayoutDataVector mLayouts;
	std::vector<QString> mDefaultLayouts;
	std::vector<QPointer<LayoutWidget> > mLayoutWidgets;
	QStringList mActiveLayout; ///< the active layout (type)
	QString mActiveView; ///< the active view
	RenderLoopPtr mRenderLoop;
	std::vector<ViewGroupPtr> mViewGroups;

	bool mGlobal2DZoom; ///< controlling whether or not 2D zooming is global
	bool mGlobalObliqueOrientation; ///< controlling whether or not all 2d views should be oblique or orthogonal
	SyncedValuePtr mGlobalZoom2DVal;

	InteractiveClipperPtr mInteractiveClipper;
	InteractiveCropperPtr mInteractiveCropper;
	SlicePlanesProxyPtr mSlicePlanesProxy;

	CameraStylePtr mCameraStyle;

private:
	ViewManager(ViewManager const&);
	ViewManager& operator=(ViewManager const&);
};
/**Shortcut for accessing the viewmanager instance.
 */
ViewManager* viewManager();

/**
 * @}
 */
} //namespace

#endif /* CXVIEWMANAGER_H_ */
