#ifndef CXBRONCHOSCOPYIMAGE2IMAGEREGISTRATIONWIDGET_H
#define CXBRONCHOSCOPYIMAGE2IMAGEREGISTRATIONWIDGET_H

#include <QPushButton>
#include <QDomElement>
#include "cxRegistrationBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"


namespace cx
{
class WidgetObscuredListener;
typedef boost::shared_ptr<class StringPropertySelectMesh> StringPropertySelectMeshPtr;
typedef boost::shared_ptr<class BronchoscopyRegistration> BronchoscopyRegistrationPtr;

/**
 * BronchoscopyImage2ImageRegistrationWidget
 *
 * \brief Image to image registration for thorax data. Useing airway centerlines for multimodal
 * image registration in bronchoscopy (CT, PET, MR).
 *
 * \date Mars 15, 2017
 * \author Erlend Hofstad
 */
class BronchoscopyImage2ImageRegistrationWidget: public RegistrationBaseWidget
{
    Q_OBJECT

    BronchoscopyRegistrationPtr mBronchoscopyRegistration;

public:
    BronchoscopyImage2ImageRegistrationWidget(RegServicesPtr services, QWidget *parent);
    virtual ~BronchoscopyImage2ImageRegistrationWidget()
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

    QVBoxLayout* mVerticalLayout;
    XmlOptionFile mOptions;
    MeshPtr mMeshFixed;
    MeshPtr mMeshMoving;

    StringPropertySelectMeshPtr mSelectMeshFixedWidget;
    StringPropertySelectMeshPtr mSelectMeshMovingWidget;
    QPushButton* mRegisterButton;

    void initializeTrackingService();
};

} //namespace cx

#endif // BRONCHOSCOPYIMAGE2IMAGEREGISTRATIONWIDGET_H
