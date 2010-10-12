#include "cxSegmentationWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>

#include "sscDataManager.h"
#include "cxDataInterface.h"
#include  "cxVolumePropertiesWidget.h"
#include "cxSegmentation.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"

namespace cx
{


SegmentationWidget::SegmentationWidget(QWidget* parent) :
        WhatsThisWidget(parent)
{
  this->setObjectName("SegmentationWidget");
  this->setWindowTitle("Segmentation");

  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);

  toptopLayout->addWidget(new ActiveVolumeWidget(this));

  QPushButton* segmentButton = new QPushButton("Segment", this);
  toptopLayout->addWidget(segmentButton);
  connect(segmentButton, SIGNAL(clicked()), this, SLOT(segmentSlot()));

  QPushButton* contourButton = new QPushButton("Contour", this);
  toptopLayout->addWidget(contourButton);
  connect(contourButton, SIGNAL(clicked()), this, SLOT(contourSlot()));

  toptopLayout->addStretch();
}

SegmentationWidget::~SegmentationWidget()
{
}

void SegmentationWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void SegmentationWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}

void SegmentationWidget::segmentSlot()
{
  int threshold = 100;
  bool useSmothing = true;
  double smoothSigma = 0.5;

  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();

  Segmentation().segment(ssc::dataManager()->getActiveImage(), outputBasePath, threshold, useSmothing, smoothSigma);
}

void SegmentationWidget::contourSlot()
{
  int threshold = 100;

  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();

  Segmentation().contour(ssc::dataManager()->getActiveImage(), outputBasePath, threshold);
}


QString SegmentationWidget::defaultWhatsThis() const
{
  return "<html>"
        "<h3>Segmentation.</h3>"
        "<p><i>Segment out blood vessels(?) from the selected image.</i></p>"
        "</html>";
}


//------------------------------------------------------------------------------


CenterlineWidget::CenterlineWidget(QWidget* parent) :
    WhatsThisWidget(parent)
{
  this->setObjectName("CenterlineWidget");
  this->setWindowTitle("Centerline");
}

CenterlineWidget::~CenterlineWidget()
{}

QString CenterlineWidget::defaultWhatsThis() const
{
  return "<html>"
        "<h3>Centerline extraction.</h3>"
        "<p><i>Extract the centerline from a segment.</i></p>"
        "<p><b>Tip:</b> The centerline extraction can take a <b>long</b> time.</p>"
        "</html>";
}

}//namespace cx
