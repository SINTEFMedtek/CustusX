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
#include "vtkImageBlend.h"
#include "cxActiveData.h"

#include "cxLogger.h"
#include "cxVolumeHelpers.h"

#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include <vtkImageActor.h>

#include "itkVTKImageToImageFilter.h"
#include "itkHoughTransform2DCirclesImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkThresholdImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include <itkGradientMagnitudeImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkExtractImageFilter.h>

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
    mVideoSource = mServices->video()->getActiveVideoSource();

    vtkImageDataPtr inputImage = mVideoSource->getVtkImageData();
    itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromVTKImage(inputImage);
    vtkImageDataPtr outputImage = AlgorithmHelper::getVTKFromITK(itkImage);

    vtkImageActorPtr imageActor = vtkImageActorPtr::New();
    imageActor->SetInputData(outputImage);

    mServices->view()->get3DView()->getRenderer()->AddActor(imageActor);

    connect(mVideoSource.get(),&VideoSource::newFrame, this, &Ur5USTrackerTab::newFrameSlot);
}

void Ur5USTrackerTab::newFrameSlot()
{

}

} // cx

