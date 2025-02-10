/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXBRONCHOSCOPYREGISTRATIONWIDGET_H
#define CXBRONCHOSCOPYREGISTRATIONWIDGET_H

#include <QPushButton>
#include <QDomElement>
#include "cxVector3D.h"
#include "cxRegistrationBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"


namespace cx
{
class RecordTrackingWidget;
class WidgetObscuredListener;
typedef boost::shared_ptr<class Acquisition> AcquisitionPtr;
typedef boost::shared_ptr<class StringPropertySelectMesh> StringPropertySelectMeshPtr;
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef boost::shared_ptr<class RecordSessionWidget> RecordSessionWidgetPtr;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;
typedef boost::shared_ptr<class BronchoscopyRegistration> BronchoscopyRegistrationPtr;
typedef std::map<QString, ToolPtr> ToolMap;
typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;

/**
 * BronchoscopyRegistrationWidget
 *
 * \brief Register tracked bronchostopy tool path to lung centerline data (from CT)
 *
 * \date Oct 10, 2013
 * \author Ole Vegard Solberg
 * \author Erlend Hofstad
 */
class BronchoscopyRegistrationWidget: public RegistrationBaseWidget
{
	Q_OBJECT

	BronchoscopyRegistrationPtr mBronchoscopyRegistration;

public:
	BronchoscopyRegistrationWidget(RegServicesPtr services, QWidget *parent);
	virtual ~BronchoscopyRegistrationWidget()
	{
	}
	virtual QString defaultWhatsThis() const;

protected:
	virtual void prePaintEvent();
private slots:
	void processCenterlineSlot();
	void registerSlot();
	void generateTrackingDataToCTcenterlineInfoSlot();
	void clearDataOnNewPatient();
private:
	void setup();

	RegServicesPtr mServices;
	QVBoxLayout* mVerticalLayout;
	BoolPropertyPtr mUseLocalRegistration;
	BoolPropertyPtr mUseEBUSProbe;
	BoolPropertyPtr mUseSubsetOfGenerations;
	DoublePropertyPtr mMaxNumberOfGenerations;
	DoublePropertyPtr mMaxLocalRegistrationDistance;
	XmlOptionFile mOptions;
	MeshPtr mMesh;

	StringPropertySelectMeshPtr mSelectMeshWidget;
	QPushButton* mProcessCenterlineButton;
	QPushButton* mRegisterButton;
	QPushButton* mInfoFileButton;
	ToolPtr mTool;

	RecordTrackingWidget* mRecordTrackingWidget;

	void initializeTrackingService();

	void createMaxNumberOfGenerations(QDomElement root);
	void selectSubsetOfBranches(QDomElement root);
	void useEBUSprobe(QDomElement root);
	void useLocalRegistration(QDomElement root);
	void createMaxLocalRegistrationDistance(QDomElement root);
	void writeTrackingDataInfoToFile(std::vector<double> timestamps, std::vector<Eigen::MatrixXd::Index> closestCTPositionIndex, Eigen::VectorXd distanceToClosestCTPosition, std::vector<std::vector<int>> branchPositionInfo, std::vector<std::vector<int> > branchCodes);
};

} //namespace cx

#endif // CXBRONCHOSCOPYREGISTRATIONWIDGET_H
