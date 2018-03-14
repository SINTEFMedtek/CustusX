/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCENTERLINEREGISTRATIONWIDGET_H
#define Centerline

#include <QPushButton>
#include <QDomElement>
#include "cxRegistrationBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"
#include "org_custusx_registration_method_centerline_Export.h"


namespace cx
{
class RecordTrackingWidget;
class WidgetObscuredListener;
typedef boost::shared_ptr<class Acquisition> AcquisitionPtr;
typedef boost::shared_ptr<class StringPropertySelectMesh> StringPropertySelectMeshPtr;
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef boost::shared_ptr<class RecordSessionWidget> RecordSessionWidgetPtr;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;
typedef boost::shared_ptr<class CenterlineRegistration> CenterlineRegistrationPtr;
typedef std::map<QString, ToolPtr> ToolMap;
typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;

/**
 * CenterlineRegistrationWidget
 *
 * \brief Register tracked tool path to centerline data (from CT)
 *
 * \date 2017-02-17
* \author Erlend Fagertun Hofstad, SINTEF
 */
class org_custusx_registration_method_centerline_EXPORT CenterlineRegistrationWidget: public RegistrationBaseWidget
{
	Q_OBJECT

public:
    CenterlineRegistrationWidget(RegServicesPtr services, QWidget *parent);
    virtual ~CenterlineRegistrationWidget()
	{
	}
    virtual QString defaultWhatsThis() const;

protected:
    virtual void prePaintEvent();
private slots:
	void registerSlot();
    void clearDataOnNewPatient();
private:
    void setup();

    CenterlineRegistrationPtr mCenterlineRegistration;

	RegServicesPtr mServices;
	QVBoxLayout* mVerticalLayout;
    BoolPropertyPtr mUseXtranslation;
    BoolPropertyPtr mUseYtranslation;
    BoolPropertyPtr mUseZtranslation;
    BoolPropertyPtr mUseXrotation;
    BoolPropertyPtr mUseYrotation;
    BoolPropertyPtr mUseZrotation;

	XmlOptionFile mOptions;
	MeshPtr mMesh;

    StringPropertySelectMeshPtr mSelectMeshWidget;
	QPushButton* mRegisterButton;
    ToolPtr mTool;

	RecordTrackingWidget* mRecordTrackingWidget;

	void initializeTrackingService();

    void selectXtranslation(QDomElement root);
    void selectYtranslation(QDomElement root);
    void selectZtranslation(QDomElement root);
    void selectXrotation(QDomElement root);
    void selectYrotation(QDomElement root);
    void selectZrotation(QDomElement root);

};

} //namespace cx

#endif // CXCENTERLINEREGISTRATIONWIDGET_H
