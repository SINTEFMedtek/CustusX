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
  this->setWindowTitle("Registration History");

  //layout
  QHBoxLayout* toptopLayout = new QHBoxLayout(this);
  toptopLayout->setMargin(0);
  mGroup = new QGroupBox;
  //group->setFlat(true);
  toptopLayout->addWidget(mGroup);
  mGroup->setTitle("Registration Time Control");
  QHBoxLayout* topLayout = new QHBoxLayout(mGroup);

  mRewindButton = new QPushButton("Rewind");
  mRewindButton->setToolTip("One step back in registration history");
  connect(mRewindButton, SIGNAL(clicked()), this, SLOT(rewindSlot()));
  topLayout->addWidget(mRewindButton);

  mRemoveButton = new QPushButton("Remove");
  mRemoveButton->setToolTip("Remove all registrations after the current");
  connect(mRemoveButton, SIGNAL(clicked()), this, SLOT(removeSlot()));
  topLayout->addWidget(mRemoveButton);

  mForwardButton = new QPushButton("Forward");
  mForwardButton->setToolTip("One step forward in registration history");
  connect(mForwardButton, SIGNAL(clicked()), this, SLOT(forwardSlot()));
  topLayout->addWidget(mForwardButton);

  mFastForwardButton = new QPushButton("Fast Forward");
  mFastForwardButton->setToolTip("Step to latest registration");
  connect(mFastForwardButton, SIGNAL(clicked()), this, SLOT(fastForwardSlot()));
  topLayout->addWidget(mFastForwardButton);
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
    disconnect(raw[i].get(), SIGNAL(currentChanged()), this, SLOT(updateSlot()));
  }
}

/** get a map of all registration times and their corresponding descriptions.
 * Near-simultaneous times are filtered out, keeping only the newest in the group.
 */
std::map<QDateTime,QString> RegistrationHistoryWidget::getRegistrationTimes()
{
  TimeMap retval;

  std::vector<ssc::RegistrationHistoryPtr> allHistories = this->getAllRegistrationHistories();

  for (unsigned i=0; i<allHistories.size(); ++i)
  {
    std::vector<ssc::RegistrationTransform> current = allHistories[i]->getData();
    for (unsigned j=0; j<current.size(); ++j)
    {
      retval[current[j].mTimestamp] = current[j].mType;
    }
    std::vector<ssc::ParentFrame> frames = allHistories[i]->getParentFrames();
    for (unsigned j=0; j<frames.size(); ++j)
    {
      retval[frames[j].mTimestamp] = frames[j].mType;
    }
  }

  return retval;
}

RegistrationHistoryWidget::TimeMap::iterator RegistrationHistoryWidget::findCurrentActiveIter(TimeMap& times)
{
  QDateTime active = this->getActiveTime();

  if (!active.isValid())
    return times.end();

  for (TimeMap::iterator iter=times.begin(); iter!=times.end(); ++iter)
  {
    if (iter->first >= active)
      return iter;
  }
  return times.end();
}

/** return the current active time
 *
 */
QDateTime RegistrationHistoryWidget::getActiveTime()
{
  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  if (raw.empty())
    return QDateTime();
  return raw.back()->getActiveTime();
}

/**set a new active time
 *
 */
void RegistrationHistoryWidget::setActiveTime(QDateTime active)
{
  ssc::messageManager()->sendInfo("setting active registration time " + active.toString(ssc::timestampSecondsFormatNice()) + ".");

  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  for (unsigned i=0; i<raw.size(); ++i)
  {
    raw[i]->setActiveTime(active);
  }
}

/**collect registration histories from the tool manager (patient registration)
 * and images (image registration) and return.
 */
std::vector<ssc::RegistrationHistoryPtr> RegistrationHistoryWidget::getAllRegistrationHistories()
{
  std::vector<ssc::RegistrationHistoryPtr> retval;
  retval.push_back(ssc::ToolManager::getInstance()->get_rMpr_History());

  std::map<QString, ssc::DataPtr> data = ssc::DataManager::getInstance()->getData();
  for (std::map<QString, ssc::DataPtr>::iterator iter=data.begin(); iter!=data.end(); ++iter)
  {
    retval.push_back(iter->second->get_rMd_History());
  }

  return retval;
}

/**Remove all registrations later than current active time.
 */
void RegistrationHistoryWidget::removeSlot()
{
  QDateTime active = this->getActiveTime();
  if (!active.isValid()) // if invalid: we are already at head
    return;

  ssc::messageManager()->sendInfo("Removing all registration performed later than " + active.toString(ssc::timestampSecondsFormatNice()) + ".");

  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  for (unsigned i=0; i<raw.size(); ++i)
  {
    raw[i]->removeNewerThan(active);
  }
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
//  this->debugDump();

  TimeMap times = this->getRegistrationTimes();
//  QDateTime active = this->getActiveTime();

  if (times.size()<=1)
    return;

  // current points to the timestamp currently in use. end() is current time.
  std::map<QDateTime,QString>::iterator current = this->findCurrentActiveIter(times);

  if (current==times.begin())
    return;

  if (current==times.end())
    --current; // ignore the last entry

  --current;
  ssc::messageManager()->sendInfo("Rewind: Setting registration time to " + current->first.toString(ssc::timestampSecondsFormatNice()) + ", [" + current->second + "]");
  this->setActiveTime(current->first);
  std::cout << "finished rewind" << std::endl;

//  this->debugDump();
}

void RegistrationHistoryWidget::debugDump()
{
  TimeMap times = this->getRegistrationTimes();

  std::stringstream ss;
  if (!this->getActiveTime().isValid())
    ss << "active time: Current \n";
  else
    ss << "active time: " << this->getActiveTime().toString(ssc::timestampSecondsFormatNice()) << "\n";
  for (TimeMap::iterator iter=times.begin(); iter!=times.end(); ++iter)
  {
    ss << "\t" << iter->first.toString(ssc::timestampSecondsFormatNice()) << "\t" << iter->second << "\n";
  }

  std::cout << ss.str() << std::endl;
}

/** jump forward to one second ahead of the NEXT registration
 */
void RegistrationHistoryWidget::forwardSlot()
{
  std::map<QDateTime,QString> times = this->getRegistrationTimes();
//  QDateTime active = this->getActiveTime();

  if (times.empty())
    return;

  // current points to the timestamp currently in use. end() is current time.
  TimeMap::iterator current = this->findCurrentActiveIter(times);

  if (current==times.end()) // already at end, ignore
    return;
  ++current;

  if (current==times.end() || times.rbegin()->first==current->first) //
  {
    ssc::messageManager()->sendInfo("Forward: Setting registration time to current, [" + times.rbegin()->second + "]");
    this->setActiveTime(QDateTime());
  }
  else
  {
    ssc::messageManager()->sendInfo("Forward: Setting registration time to " + current->first.toString(ssc::timestampSecondsFormatNice()) + ", [" + current->second + "]");
    this->setActiveTime(current->first);
  }
}


/**Use the newest available registration.
 * Negates any call to usePreviousRegistration.
 */
void RegistrationHistoryWidget::fastForwardSlot()
{
  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  ssc::messageManager()->sendInfo("Fast Forward: Setting registration time to current.");

  for (unsigned i=0; i<raw.size(); ++i)
  {
    raw[i]->setActiveTime(QDateTime());
  }
}

void RegistrationHistoryWidget::updateSlot()
{
//  this->debugDump();

  //std::cout << "RegistrationHistoryWidget::updateSlot()" << std::endl;

  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  std::vector<ssc::RegistrationTransform> history = mergeHistory(raw);


  TimeMap times = this->getRegistrationTimes();
  std::map<QDateTime,QString>::iterator current = this->findCurrentActiveIter(times);
//  int behind = distance(times.begin(), current);
  int infront = distance(current, times.end()) - 1;
  if (infront<0)
    infront=0;
  int behind = times.size() - infront;

  mRewindButton->setText("Rewind (" + qstring_cast(behind) + ")");
  mForwardButton->setText("Forward (" + qstring_cast(infront) + ")");

//  bool newest = isUsingNewestRegistration();
//  mRewindButton->setEnabled(newest && !history.empty());
//  mRemoveButton->setEnabled(!history.empty());
//  mForwardButton->setEnabled(!newest && !history.empty());
  mRewindButton->setEnabled(behind>1);
  mRemoveButton->setEnabled(infront!=0);
  mForwardButton->setEnabled(infront!=0);
  mFastForwardButton->setEnabled(infront!=0);
//  mGroup->adjustSize();
//  this->adjustSize();
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
