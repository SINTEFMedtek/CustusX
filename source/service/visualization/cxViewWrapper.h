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

#ifndef CXVIEWWRAPPER_H_
#define CXVIEWWRAPPER_H_

#include <vector>
#include <QVariant>
#include <QObject>
#include <vtkPolyDataAlgorithm.h>
#include "sscDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include <vtkSphereSource.h>
#include "sscMesh.h"

typedef vtkSmartPointer<class vtkPolyDataAlgorithm> vtkPolyDataAlgorithmPtr;
class QMenu;
class QActionGroup;

namespace cx
{

typedef boost::shared_ptr<class CameraData> CameraDataPtr;

/**
 * \file
 * \addtogroup cxServiceVisualization
 * @{
 */

typedef boost::shared_ptr<class SyncedValue> SyncedValuePtr;

class SyncedValue: public QObject
{
Q_OBJECT
public:
	SyncedValue(QVariant val = QVariant());
	static SyncedValuePtr create(QVariant val = QVariant());
	void set(QVariant val);
	QVariant get() const;
private:
	QVariant mValue;signals:
	void changed();
};

typedef boost::shared_ptr<class ViewGroupData> ViewGroupDataPtr;

/** \brief Container for data shared between all members of a view group
 */
class ViewGroupData: public QObject
{
Q_OBJECT
public:
	ViewGroupData();
	void requestInitialize();
	std::vector<ssc::DataPtr> getData() const;
	void addData(ssc::DataPtr data);
	bool removeData(ssc::DataPtr data);
	void clearData();
	std::vector<ssc::ImagePtr> getImages() const;
	std::vector<ssc::MeshPtr> getMeshes() const;

	CameraDataPtr getCamera3D() { return mCamera3D; }

	// view options for this group.
	struct Options
	{
		Options() : mShowLandmarks(false), mShowPointPickerProbe(false),
				mPickerGlyph(new ssc::Mesh("PickerGlyph")) {}
		bool mShowLandmarks;
		bool mShowPointPickerProbe;
		ssc::MeshPtr mPickerGlyph;
//		vtkPolyDataAlgorithmPtr mPickerGlyph; ///< a glyph visually representing the picker.
//		vtkSphereSourcePtr getSpherePickerGlyph() { return vtkSphereSource::SafeDownCast(mPickerGlyph); }
	};

	Options getOptions() const;
	void setOptions(Options options);

private slots:
	void removeDataSlot(QString uid);signals:
	void dataAdded(QString uid);
	void dataRemoved(QString uid);
	void initialized();
	void optionsChanged();

private:
	std::vector<ssc::DataPtr> mData;
	CameraDataPtr mCamera3D;
	Options mOptions;
};

/**
 * \brief Superclass for ViewWrappers.
 *
 *
 * \date 6. apr. 2010
 * \\author jbake
 */
class ViewWrapper: public QObject
{
Q_OBJECT
public:
	virtual ~ViewWrapper() {}
	virtual void initializePlane(ssc::PLANE_TYPE plane) {}
	virtual ssc::ViewWidget* getView() = 0;
	virtual void setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy) = 0;
	virtual void setViewGroup(ViewGroupDataPtr group);

	virtual void setZoom2D(SyncedValuePtr value) {}
	virtual void setOrientationMode(SyncedValuePtr value) {}

signals:
	void orientationChanged(ssc::ORIENTATION_TYPE type);

protected slots:
	void contextMenuSlot(const QPoint& point);
	void dataActionSlot();

	void dataAddedSlot(QString uid);
	void dataRemovedSlot(QString uid);

protected:
	virtual void dataAdded(ssc::DataPtr data) = 0;
	virtual void dataRemoved(const QString& uid) = 0;

	void connectContextMenu(ssc::ViewWidget* view);
	virtual void appendToContextMenu(QMenu& contextMenu) = 0;
	void addDataAction(QString uid, QMenu* contextMenu);
	QStringList getAllDataNames() const;

	ViewGroupDataPtr mViewGroup;

private:
	QString mLastDataActionUid;

};

/**
 * @}
 */
} //namespace cx

#endif /* CXVIEWWRAPPER_H_ */
