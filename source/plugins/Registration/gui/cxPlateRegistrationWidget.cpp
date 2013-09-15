#include "cxPlateRegistrationWidget.h"

#include <QPushButton>
#include <QLabel>
#include "sscTypeConversions.h"
#include "cxToolManager.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "cxRegistrationManager.h"
#include "cxViewManager.h"

namespace cx
{
PlateRegistrationWidget::PlateRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent) :
    RegistrationBaseWidget(regManager, parent, "PlateRegistrationWidget", "Plate Registration"),
    mPlateRegistrationButton(new QPushButton("Load registration points", this)),
    mReferenceToolInfoLabel(new QLabel("", this))
{
  connect(mPlateRegistrationButton, SIGNAL(clicked()), this, SLOT(plateRegistrationSlot()));
  connect(toolManager(), SIGNAL(configured()), this, SLOT(internalUpdate()));

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  toptopLayout->addWidget(mReferenceToolInfoLabel);
  toptopLayout->addWidget(mPlateRegistrationButton);
  toptopLayout->addStretch();

  this->internalUpdate();
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
  BaseWidget::showEvent(event);
  connect(toolManager(), SIGNAL(landmarkAdded(QString)),   this, SLOT(landmarkUpdatedSlot()));
  connect(toolManager(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

  viewManager()->setRegistrationMode(rsPATIENT_REGISTRATED);
}

void PlateRegistrationWidget::hideEvent(QHideEvent* event)
{
  BaseWidget::hideEvent(event);
  disconnect(toolManager(), SIGNAL(landmarkAdded(QString)),   this, SLOT(landmarkUpdatedSlot()));
  disconnect(toolManager(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

  viewManager()->setRegistrationMode(rsNOT_REGISTRATED);
}

void PlateRegistrationWidget::landmarkUpdatedSlot()
{
  mManager->doFastRegistration_Translation();
}

void PlateRegistrationWidget::plateRegistrationSlot()
{
  toolManager()->removeLandmarks();

  ToolPtr refTool = toolManager()->getReferenceTool();
  if(!refTool)//cannot register without a reference tool
  {
    messageManager()->sendDebug("No refTool");
    return;
  }
  std::map<int, Vector3D> referencePoints = refTool->getReferencePoints();
  if(referencePoints.empty()) //cannot register without at least 1 reference point
  {
    messageManager()->sendDebug("No referenceppoints in reftool "+refTool->getName());
    return;
  }

  std::map<int, Vector3D>::iterator it = referencePoints.begin();
  for(; it != referencePoints.end(); ++it)
  {
    QString uid = dataManager()->addLandmark();
    dataManager()->setLandmarkName(uid, qstring_cast(it->first));
    toolManager()->setLandmark(Landmark(uid, it->second));
  }

  // set all landmarks as not active as default
  LandmarkPropertyMap map = dataManager()->getLandmarkProperties();
  LandmarkPropertyMap::iterator landmarkIt = map.begin();
  for(; landmarkIt != map.end(); ++landmarkIt)
  {
    dataManager()->setLandmarkActive(landmarkIt->first, false);
  }

  //we don't want the user to load the landmarks twice, it will result in alot of global landmarks...
  mPlateRegistrationButton->setDisabled(true);
}

void PlateRegistrationWidget::internalUpdate()
{
  ToolPtr refTool = toolManager()->getReferenceTool();

  QString labelText = "";
  if(!refTool || refTool->getReferencePoints().size()<1)
  {
    mPlateRegistrationButton->setDisabled(true);

    labelText.append("Configure the tracker to have <br>a reference frame that has at least <br>one reference point.");
  }else
  {
    mPlateRegistrationButton->setEnabled(true);

    labelText = "<b>Reference tool selected:</b> <br>";
    labelText.append("Tool name: <i>"+refTool->getName()+"</i><br>");
    labelText.append("Number of defined reference points: <i>"+qstring_cast(refTool->getReferencePoints().size())+"</i>");
  }

  mReferenceToolInfoLabel->setText(labelText);
}

}//namespace cx
