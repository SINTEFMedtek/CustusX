#include "cxPlateRegistrationWidget.h"

#include <QPushButton>
#include <QLabel>
#include "sscTypeConversions.h"
#include "cxToolManager.h"
#include "sscMessageManager.h"
#include "cxRegistrationManager.h"
#include "cxViewManager.h"

namespace cx
{
PlateRegistrationWidget::PlateRegistrationWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mPlateRegistrationButton(new QPushButton("Load registration points", this)),
    mReferenceToolInfoLabel(new QLabel("", this))
{
  this->setObjectName("PlateRegistrationWidget");
  this->setWindowTitle("Fast Image Registration");

  connect(mPlateRegistrationButton, SIGNAL(clicked()), this, SLOT(plateRegistrationSlot()));
  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(referenceToolInfoSlot()));

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  toptopLayout->addWidget(mReferenceToolInfoLabel);
  toptopLayout->addWidget(mPlateRegistrationButton);
  toptopLayout->addStretch();

  this->referenceToolInfoSlot();
}

PlateRegistrationWidget::~PlateRegistrationWidget()
{

}

QString PlateRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Plate registration.</h3>"
      "<p>Internally register the reference plates reference points as landmarks.</p>"
      "<p><i>Click the button to load landmarks.</i></p>"
      "</html>";
}

void PlateRegistrationWidget::showEvent(QShowEvent* event)
{
  WhatsThisWidget::showEvent(event);
  connect(ssc::toolManager(), SIGNAL(landmarkAdded(QString)),   this, SLOT(landmarkUpdatedSlot()));
  connect(ssc::toolManager(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

  viewManager()->setRegistrationMode(ssc::rsPATIENT_REGISTRATED);
}

void PlateRegistrationWidget::hideEvent(QHideEvent* event)
{
  WhatsThisWidget::hideEvent(event);
  disconnect(ssc::toolManager(), SIGNAL(landmarkAdded(QString)),   this, SLOT(landmarkUpdatedSlot()));
  disconnect(ssc::toolManager(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

  viewManager()->setRegistrationMode(ssc::rsNOT_REGISTRATED);
}

void PlateRegistrationWidget::landmarkUpdatedSlot()
{
  registrationManager()->doFastRegistration_Translation();
}

void PlateRegistrationWidget::plateRegistrationSlot()
{
  ssc::toolManager()->removeLandmarks();

  ssc::ToolPtr refTool = ssc::toolManager()->getReferenceTool();
  if(!refTool)//cannot register without a reference tool
  {
    ssc::messageManager()->sendDebug("No refTool");
    return;
  }
  std::map<int, ssc::Vector3D> referencePoints = refTool->getReferencePoints();
  if(referencePoints.empty()) //cannot register without at least 1 reference point
  {
    ssc::messageManager()->sendDebug("No referenceppoints in reftool "+refTool->getName());
    return;
  }

  std::map<int, ssc::Vector3D>::iterator it = referencePoints.begin();
  for(; it != referencePoints.end(); ++it)
  {
    ssc::toolManager()->setLandmark(ssc::Landmark(qstring_cast(it->first), it->second));
  }
}

void PlateRegistrationWidget::referenceToolInfoSlot()
{
  ssc::ToolPtr refTool = ssc::toolManager()->getReferenceTool();
  if(!refTool)
    return;

  QString labelText = "<b>Reference tool selected:</b> <br>";
  labelText.append("Tool name: <i>"+refTool->getName()+"</i><br>");
  labelText.append("Number of defined reference points: <i>"+qstring_cast(refTool->getReferencePoints().size())+"</i>");

  mReferenceToolInfoLabel->setText(labelText);
}

}//namespace cx
