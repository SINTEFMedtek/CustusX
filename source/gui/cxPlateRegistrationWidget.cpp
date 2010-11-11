#include "cxPlateRegistrationWidget.h"

#include <QPushButton>
#include "sscTypeConversions.h"
#include "sscToolManager.h"
#include "sscMessageManager.h"

namespace cx
{
PlateRegistrationWidget::PlateRegistrationWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mPlateRegistrationButton(new QPushButton("Load registration points", this))
{
  this->setObjectName("PlateRegistrationWidget");
  this->setWindowTitle("Fast Image Registration");

  connect(mPlateRegistrationButton, SIGNAL(clicked()), this, SLOT(plateRegistrationSlot()));

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  //QGridLayout* topLayout = new QGridLayout();

//  toptopLayout->addLayout(topLayout);
  toptopLayout->addWidget(mPlateRegistrationButton);
  toptopLayout->addStretch();
//  topLayout->addWidget(mPlateRegistrationButton, 1, 0);
//  topLayout->addWidget(sampleGroupBox, 2, 0);
}

PlateRegistrationWidget::~PlateRegistrationWidget()
{

}

QString PlateRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Plate registration.</h3>"
      "<p>Internally register the reference plates reference points as landmarks.</p>"
      "<p><i>Click register to add landmarks.</i></p>"
      "</html>";
}

void PlateRegistrationWidget::plateRegistrationSlot()
{
  //TODO clear toolmanagers landmarks

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

}//namespace cx
