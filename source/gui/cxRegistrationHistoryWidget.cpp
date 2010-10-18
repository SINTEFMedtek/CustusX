#include "cxRegistrationHistoryWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include "sscImage.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "sscTime.h"

namespace cx
{

RegistrationHistoryWidget::RegistrationHistoryWidget(QWidget* parent) :
    QWidget(parent)
{
  //dock widget
  this->setObjectName("RegistrationHistoryWidget");
  this->setWindowTitle("RegistrationHistory");

  //layout
  QHBoxLayout* toptopLayout = new QHBoxLayout(this);
  toptopLayout->setMargin(0);
  QGroupBox* group = new QGroupBox;
  //group->setFlat(true);
  toptopLayout->addWidget(group);
  group->setTitle("Registration Time Control");
  QHBoxLayout* topLayout = new QHBoxLayout(group);

  mRewindButton = new QPushButton("Rewind");
  mRewindButton->setToolTip("Use previous registration");
  connect(mRewindButton, SIGNAL(clicked()), this, SLOT(rewindSlot()));
  topLayout->addWidget(mRewindButton);

  mRemoveButton = new QPushButton("Remove");
  mRemoveButton->setToolTip("Remove the latest registration");
  connect(mRemoveButton, SIGNAL(clicked()), this, SLOT(removeSlot()));
  topLayout->addWidget(mRemoveButton);

  mForwardButton = new QPushButton("Forward");
  mForwardButton->setToolTip("Use latest registration");
  connect(mForwardButton, SIGNAL(clicked()), this, SLOT(forwardSlot()));
  topLayout->addWidget(mForwardButton);
}

RegistrationHistoryWidget::~RegistrationHistoryWidget()
{
}

void RegistrationHistoryWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);

  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  for (unsigned i=0; i<raw.size(); ++i)
  {
    connect(raw[i].get(), SIGNAL(currentChanged()), this, SLOT(updateSlot()));
  }

  updateSlot();
}

void RegistrationHistoryWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);

  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  for (unsigned i=0; i<raw.size(); ++i)
  {
    connect(raw[i].get(), SIGNAL(currentChanged()), this, SLOT(updateSlot()));
  }
}


/**collect registration histories from the tool manager (patient registration)
 * and images (image registration) and return.
 */
std::vector<ssc::RegistrationHistoryPtr> RegistrationHistoryWidget::getAllRegistrationHistories()
{
  std::vector<ssc::RegistrationHistoryPtr> retval;
  retval.push_back(ssc::ToolManager::getInstance()->get_rMpr_History());

  std::map<QString, ssc::ImagePtr> image = ssc::DataManager::getInstance()->getImages();
  for (std::map<QString, ssc::ImagePtr>::iterator iter=image.begin(); iter!=image.end(); ++iter)
  {
    retval.push_back(iter->second->get_rMd_History());
  }

  return retval;
}

/**Remove the latest registration event (image or patient)
 * inserted into the system.
 */
void RegistrationHistoryWidget::removeSlot()
{
  //search among all images and patient reg, find latest, remove that one.
  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  std::vector<ssc::RegistrationTransform> history = mergeHistory(raw);

  if (history.empty())
    return;

  QDateTime lastTime = history.back().mTimestamp;

  lastTime = lastTime.addSecs(-1);
  ssc::messageManager()->sendInfo("Removing all registration performed later than " + lastTime.toString(ssc::timestampSecondsFormatNice()) + ".");

  for (unsigned i=0; i<raw.size(); ++i)
  {
    raw[i]->removeNewerThan(lastTime);
  }

  //updateSlot();
}

std::vector<ssc::RegistrationTransform> RegistrationHistoryWidget::mergeHistory(const std::vector<ssc::RegistrationHistoryPtr>& allHistories)
{
  std::vector<ssc::RegistrationTransform> history;
  for (unsigned i=0; i<allHistories.size(); ++i)
  {
    std::vector<ssc::RegistrationTransform> current = allHistories[i]->getData();
    std::copy(current.begin(), current.end(), std::back_inserter(history));
  }
  std::sort(history.begin(), history.end());

  return history;
}

/**Take one step back in registration time and use the previous
 * registration event instead of the current.
 */
void RegistrationHistoryWidget::rewindSlot()
{
  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  std::vector<ssc::RegistrationTransform> history = mergeHistory(raw);

  if (history.empty())
    return;

  QDateTime lastTime = history.back().mTimestamp;
  lastTime = lastTime.addSecs(-1);

  ssc::messageManager()->sendInfo("Setting registration time to " + lastTime.toString(ssc::timestampSecondsFormatNice()) + ".");

  for (unsigned i=0; i<raw.size(); ++i)
  {
    raw[i]->setActiveTime(lastTime);
  }
}

/**Use the newest available registration.
 * Negates any call to usePreviousRegistration.
 */
void RegistrationHistoryWidget::forwardSlot()
{
  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  ssc::messageManager()->sendInfo("Setting registration time to current.");

  for (unsigned i=0; i<raw.size(); ++i)
  {
    raw[i]->setActiveTime(QDateTime());
  }
}

void RegistrationHistoryWidget::updateSlot()
{
  //std::cout << "RegistrationHistoryWidget::updateSlot()" << std::endl;

  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  std::vector<ssc::RegistrationTransform> history = mergeHistory(raw);

  bool newest = isUsingNewestRegistration();
  mRewindButton->setEnabled(newest && !history.empty());
  mRemoveButton->setEnabled(!history.empty());
  mForwardButton->setEnabled(!newest && !history.empty());
}

/**Return true if the system is currently using the newest
 * available registration.
 */
bool RegistrationHistoryWidget::isUsingNewestRegistration()
{
  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();

  for (unsigned i=0; i<raw.size(); ++i)
  {
    if (raw[i]->getActiveTime()!=QDateTime())
      return false;
  }

  return true;
}


}//end namespace cx
