#include "cxUr5USTrackerTab.h"


#include "cxUr5MessageEncoder.h"

#include "cxVisServices.h"
#include "cxTrackingService.h"
#include "cxPatientModelService.h"
#include "cxVideoService.h"
#include "cxVideoSource.h"
#include "cxAlgorithmHelpers.h"
#include "cxViewService.h"
#include "cxView.h"
#include <vtkRenderer.h>
#include <vtkImageCast.h>
#include "cxImage.h"
#include "cxProbe.h"
#include "cxTool.h"
#include "cxRep.h"
#include "cxRepContainer.h"
#include "cxRepImpl.h"
#include "cxStreamerService.h"

#include "vtkImageCanvasSource2D.h"
#include "cxActiveData.h"

#include "cxLogger.h"
#include "cxVolumeHelpers.h"

#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include <vtkImageActor.h>


namespace cx
{

Ur5USTrackerTab::Ur5USTrackerTab(Ur5RobotPtr Ur5Robot,VisServicesPtr services, QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot),
    mServices(services)
{
    setupUi(this);

    connect(testButton, &QPushButton::clicked, this, &Ur5USTrackerTab::testSlot);
}

Ur5USTrackerTab::~Ur5USTrackerTab()
{

}

void Ur5USTrackerTab::setupUi(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    setSettingsLayout(mainLayout);
    setMoveLayout(mainLayout);


}

void Ur5USTrackerTab::setSettingsLayout(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Setup US tracking");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *mainLayout = new QGridLayout();
    group->setLayout(mainLayout);

    testButton = new QPushButton(tr("Test"));

    int row = 0;
    mainLayout->addWidget(testButton, row, 0, 1, 1);
}

void Ur5USTrackerTab::setMoveLayout(QVBoxLayout *parent)
{

}

void Ur5USTrackerTab::testSlot()
{
    mVideoSource.reset(new BasicVideoSource(mServices->video()->getActiveVideoSource()->getUid()));

    vtkImageDataPtr inputImage = mVideoSource->getVtkImageData();
    itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromVTKImage(inputImage);
    vtkImageDataPtr outputImage = AlgorithmHelper::getVTKFromITK(itkImage);

    mVideoSource->start();
    mVideoSource->overrideTimeout(true);

    vtkImageDataPtr emptyImage = generateVtkImageData(Eigen::Array3i(3,3,1),
                                                           Vector3D(1,1,1),
                                                           0);

    vtkImageActorPtr imageActor = vtkImageActorPtr::New();
    imageActor->SetInputData(inputImage);

    mServices->view()->get3DView()->getRenderer()->AddActor(imageActor);
}

void Ur5USTrackerTab::newFrameSlot()
{
    vtkImageDataPtr inputImage = mVideoSource->getVtkImageData();
    itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromVTKImage(inputImage);
}

} // cx

