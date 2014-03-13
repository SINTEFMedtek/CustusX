#include "cxFastImageRegistrationWidget.h"

#include "cxMessageManager.h"
#include "cxDataManager.h"
#include "cxRegistrationManager.h"

namespace cx
{

FastImageRegistrationWidget::FastImageRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent, QString objectName, QString windowTitle) :
    LandmarkImageRegistrationWidget(regManager, parent, objectName, windowTitle)
{
}

FastImageRegistrationWidget::~FastImageRegistrationWidget()
{

}

QString FastImageRegistrationWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Fast image registration.</h3>"
			"<p>Select landmarks in the data set that you want to use for performing fast registration.</p>"
			"<p><i>Click in the dataset and push the add or resample button.</i></p>"
			"</html>";
}

void FastImageRegistrationWidget::performRegistration()
{
  //make sure the masterImage is set
  DataPtr fixedData = mManager->getFixedData();
  if(!fixedData)
    mManager->setFixedData(dataManager()->getActiveImage());

  this->updateAvarageAccuracyLabel();
}
//------------------------------------------------------------------------------
PlateImageRegistrationWidget::PlateImageRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent) :
    FastImageRegistrationWidget(regManager, parent, "PlateImageRegistrationWidget", "Plate Image Registration")
{
}

PlateImageRegistrationWidget::~PlateImageRegistrationWidget()
{}

QString PlateImageRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Image registration.</h3>"
      "<p>Select landmarks in the data set that you want to use for performing plate registration.</p>"
      "<p><i>Click in the dataset and push the add or resample button.</i></p>"
      "</html>";
}

void PlateImageRegistrationWidget::editLandmarkButtonClickedSlot()
{
  dataManager()->setLandmarkActive(mActiveLandmark, true);
  LandmarkImageRegistrationWidget::editLandmarkButtonClickedSlot();
}

void PlateImageRegistrationWidget::performRegistration()
{
  FastImageRegistrationWidget::performRegistration();
  mManager->doFastRegistration_Translation();
}
//------------------------------------------------------------------------------

}//namespace cx
