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

	bool mCameraFollowTool;
	bool mFocusFollowTool;
	bool mTableLock;
	double mElevation;
	bool mUniCam;
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
	explicit ViewGroupData(CoreServicesPtr services);
	void requestInitialize();
//	std::vector<DataPtr> getData() const;
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
	std::vector<TrackedStreamPtr> getTracked2DStreams(DataViewProperties properties) const;
	std::vector<ImagePtr> getImagesAndChangingImagesFromTrackedStreams(DataViewProperties properties, bool include2D = false) const;
	DataViewProperties getProperties(QString uid);
	void setProperties(QString uid, DataViewProperties properties);

	void initializeGlobal2DZoom(SyncedValuePtr val);
	SyncedValuePtr getGroup2DZoom();
	SyncedValuePtr getGlobal2DZoom();

	CameraDataPtr getCamera3D() { return mCamera3D; }

	PlaneTypeCollection getSliceDefinitions();
	void setSliceDefinitions(PlaneTypeCollection val);
	StringListPropertyPtr getSliceDefinitionProperty();

	// view options for this group.
	struct Options
	{
		Options();
		bool mShowLandmarks;
		bool mShowPointPickerProbe;
//		bool mLockToTable; ///< lock the 3D view orientation to have Table down.
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
	CoreServicesPtr mServices;
	QString mVideoSource;
	typedef std::pair<QString, DataViewProperties> DataAndViewProperties;
	std::vector<DataAndViewProperties> mData;
	CameraDataPtr mCamera3D;
	Options mOptions;
	SyncedValuePtr mGroup2DZoom;
	SyncedValuePtr mGlobal2DZoom;
	StringListPropertyPtr mSliceDefinitionProperty;
	DataPtr getData(QString uid) const;

	struct data_equals
	{
		data_equals(QString uid) : mData(uid) {}
		bool operator()(const DataAndViewProperties& right)
		{
			return mData == right.first;
		}
		QString mData;
	};

	template<class DATA_TYPE>
	std::vector<boost::shared_ptr<DATA_TYPE> > getDataOfType(DataViewProperties requiredProperties) const;
	bool contains(QString uid) const;
	void createSliceDefinitionProperty();
};


/**
 * @}
 */
} //namespace cx

#endif // CXVIEWGROUPDATA_H
