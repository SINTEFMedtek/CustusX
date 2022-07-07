/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVBWIDGET_H_
#define CXVBWIDGET_H_

#include <QWidget>

#include "ctkPluginContext.h"

#include "cxSelectDataStringProperty.h"
#include "cxMesh.h"
#include "cxVBcameraPath.h"
#include "org_custusx_virtualbronchoscopy_Export.h"


class QVBoxLayout;
class QDial;
class QSlider;
class QPushButton;

namespace cx
{

typedef boost::shared_ptr<class StringPropertySelectMesh> StringPropertySelectMeshPtr;
typedef boost::shared_ptr<class PatientStorage> PatientStoragePtr;

/**
 * Widget for Virtual Bronchoscopy
 *
 * \ingroup org_custusx_virtualbronchoscopy
 *
 * \date Aug 27, 2015
 * \author Geir Arne Tangen, SINTEF
 */
class org_custusx_virtualbronchoscopy_EXPORT VBWidget : public QWidget
{
	Q_OBJECT
public:
	VBWidget(VisServicesPtr services, QWidget *parent = 0);
	virtual ~VBWidget();
	void setRouteToTarget(QString uid);
	void setRoutePositions(std::vector< Eigen::Vector3d > routePositions);
	void setCameraRotationAlongRoute(std::vector< double > cameraRotations);
	void setRecordVideoOption(bool recordVideo);

protected:
	QString defaultWhatsThis() const;
	VisServicesPtr		mServices;
	QVBoxLayout*			mVerticalLayout;
	QSlider*					mPlaybackSlider;
	QPushButton				*mPlayButton;
	QTimer*						mTimer;
	QDial*						mRotateDial;
	QDial*						mViewDial;
	QPushButton*			mResetEndoscopeButton;
	QPushButton*			mUseAutomaticRotationButton;
	QPalette					mAutomaticRotationButtonBackgroundColor;

	StringPropertySelectMeshPtr	mRouteToTarget;
	CXVBcameraPath*				mCameraPath;
	bool						mControlsEnabled;
	bool						mAutomaticRotation;
	bool						mRecordVideo;

	void						enableControls(bool enable);
	QFileInfo				startRecordFullscreen();
	void						stopRecordFullscreen();

	PatientStoragePtr mStorage;

signals:
	void						cameraPathChanged(MeshPtr pathMesh);
	void						cameraAtEndPosition();

private slots:
	void						inputChangedSlot();
	void						moveCameraSlot();
	void						resetEndoscopeSlot();
	void						automaticRotationSlot();
	void						updateRotationDialSlot(int value);

protected slots:
	virtual void				keyPressEvent(QKeyEvent* event);
	void								playButtonClickedSlot();
};

} /* namespace cx */

#endif /* CXVBWIDGET_H_ */
