/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXVIEWGROUPDATA_H
#define CXVIEWGROUPDATA_H

#include "cxResourceVisualizationExport.h"

#include <vector>
#include <QVariant>
#include <QObject>
#include "cxDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxPlaneTypeCollection.h"
#include "cxViewService.h"
class QDomNode;

namespace cx
{

typedef boost::shared_ptr<class CameraData> CameraDataPtr;
typedef boost::shared_ptr<class CoreServices> CoreServicesPtr;
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class StringListProperty> StringListPropertyPtr;


/**
 * \file
 * \addtogroup cx_resource_view
 * @{
 */
struct cxResourceVisualization_EXPORT CameraStyleData
{
	CameraStyleData();
	explicit CameraStyleData(CAMERA_STYLE_TYPE style);
	void setCameraStyle(CAMERA_STYLE_TYPE style);
	CAMERA_STYLE_TYPE getStyle();
	void clear();
	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode dataNode);

	double mCameraViewAngle;
	bool mCameraFollowTool;
	bool mFocusFollowTool;
	bool mCameraLockToTooltip;
	double mCameraTooltipOffset;
	bool mTableLock;
	double mElevation;
	bool mUniCam;
	QString mCameraNotBehindROI; // never move camera behind this roi
	QString mFocusROI; // name of ROI to set focus in.
	QString mAutoZoomROI; // name of ROI to zoom to.
};
cxResourceVisualization_EXPORT bool operator==(const CameraStyleData& lhs, const CameraStyleData& rhs);

/**Define a priority for the input data.
 * High means display on top, low means in the back.
 */
cxResourceVisualization_EXPORT  int getPriority(DataPtr data);

/**Sorts DataPtr in default display ordering, using getPriority().
 */
cxResourceVisualization_EXPORT bool dataTypeSort(const DataPtr data1, const DataPtr data2);

typedef boost::shared_ptr<class SyncedValue> SyncedValuePtr;
typedef boost::shared_ptr<class ViewGroupData> ViewGroupDataPtr;

/** Stores how a Data should be visualized in 2D and 3D views.
  */
class cxResourceVisualization_EXPORT DataViewProperties
{
public:
	DataViewProperties() : mVolume3D(false), mSlice3D(false), mSlice2D(false) {}
	static DataViewProperties createDefault();
	static DataViewProperties createFull();
	static DataViewProperties createVolume3D();
	static DataViewProperties createSlice3D();
	static DataViewProperties createSlice2D();
	static DataViewProperties create3D();
	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode dataNode);
	bool empty() const;
	DataViewProperties addFlagsIn(DataViewProperties rhs) const;
	DataViewProperties removeFlagsIn(DataViewProperties rhs) const;
	bool containsAnyFlagsIn(DataViewProperties required) const;

	bool hasVolume3D() const { return mVolume3D; }
	bool hasSlice3D() const { return mSlice3D; }
	bool hasSlice2D() const { return mSlice2D; }

private:
	bool mVolume3D;
	bool mSlice3D;
	bool mSlice2D;
};

/** \brief Container for data shared between all members of a view group
 */
class cxResourceVisualization_EXPORT ViewGroupData: public QObject
{
Q_OBJECT

public:
	explicit ViewGroupData(CoreServicesPtr services, QString uid);
	void requestInitialize();
	QString getUid() const { return mUid; }
	std::vector<DataPtr> getData(DataViewProperties properties=DataViewProperties::createFull()) const;
	QString getVideoSource() const;
	void addData(QString uid);
	void addDataSorted(QString uid); ///< add data in a predefined ordering: CT/MR/SC/US/USA/Mesh/Metrics
	void setVideoSource(QString uid);
	bool removeData(QString uid);
	void clearData();
	std::vector<ImagePtr> getImages(DataViewProperties properties) const;
	std::vector<MeshPtr> getMeshes(DataViewProperties properties) const;
	std::vector<TrackedStreamPtr> getTrackedStreams(DataViewProperties properties) const;
	std::vector<ImagePtr> getImagesAndChangingImagesFromTrackedStreams(DataViewProperties properties, bool include2D = false) const;
	DataViewProperties getProperties(QString uid);
	void setProperties(QString uid, DataViewProperties properties);

	void initializeGlobal2DZoom(SyncedValuePtr val);
	SyncedValuePtr getGroup2DZoom();
	SyncedValuePtr getGlobal2DZoom();

	CameraDataPtr getCamera3D() { return mCamera3D; }
    void zoomCamera3D(int zoomFactor);

	PlaneTypeCollection getSliceDefinitions();
	void setSliceDefinitions(PlaneTypeCollection val);
	StringListPropertyPtr getSliceDefinitionProperty();

	// view options for this group.
	struct Options
	{
		Options();
		bool mShowLandmarks;
		bool mShowPointPickerProbe;
		MeshPtr mPickerGlyph;
		CameraStyleData mCameraStyle;
	};

	Options getOptions() const;
	void setOptions(Options options);
	void setRegistrationMode(REGISTRATION_STATUS mode);

	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode dataNode);

private slots:
	void purgeDataNotExistingInPatientModelService();

signals:
	void dataViewPropertiesChanged(QString uid);
	void videoSourceChanged(QString uid);
	void initialized();
	void optionsChanged();

private:
	typedef std::pair<QString, DataViewProperties> DataAndViewPropertiesPair;

	struct data_equals
	{
		data_equals(QString uid) : mData(uid) {}
		bool operator()(const DataAndViewPropertiesPair& right)
		{
			return mData == right.first;
		}
		QString mData;
	};

	template<class DATA_TYPE>
	std::vector<boost::shared_ptr<DATA_TYPE> > getDataOfType(DataViewProperties requiredProperties) const;
	bool contains(QString uid) const;
	void createSliceDefinitionProperty();
	void insertData(std::vector<DataAndViewPropertiesPair>::iterator iter, DataAndViewPropertiesPair &item);

	QString mUid;
	CoreServicesPtr mServices;
	QString mVideoSource;
	std::vector<DataAndViewPropertiesPair> mData;
	CameraDataPtr mCamera3D;
	Options mOptions;
	SyncedValuePtr mGroup2DZoom;
	SyncedValuePtr mGlobal2DZoom;
	StringListPropertyPtr mSliceDefinitionProperty;
	DataPtr getData(QString uid) const;
	//SharedOpenGLContextPtr mSharedOpenGLContext;

};


/**
 * @}
 */
} //namespace cx

#endif // CXVIEWGROUPDATA_H
