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

template<class T>
QAction* RegistrationHistoryWidget::createAction(QLayout* layout, QString iconName, QString text, QString tip, T slot)
{
  QAction* action = new QAction(QIcon(iconName), text, this);
  action->setStatusTip(tip);
  action->setToolTip(tip);
  connect(action, SIGNAL(triggered()), this, slot);
  QToolButton* button = new QToolButton();
  //button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  button->setDefaultAction(action);
  layout->addWidget(button);
  return action;
}

RegistrationHistoryWidget::RegistrationHistoryWidget(QWidget* parent) :
    QWidget(parent)
{
  //dock widget
  this->setObjectName("RegistrationHistoryWidget");
  this->setWindowTitle("Registration History");

  //layout
  QHBoxLayout* toptopLayout = new QHBoxLayout(this);
//  toptopLayout->setMargin(0);
  //mGroup = new QGroupBox;
  //group->setFlat(true);
  //mGroup->setTitle("Registration Time Control");
  QHBoxLayout* topLayout = new QHBoxLayout;
//  toptopLayout->setMargin(0);
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();


//  QString iconname = ":/icons/Tango/scalable/actions/go-first.svg";
//  QString iconname = ":/icons/Tango/32x32/actions/go-first.png";
//  QString iconname = ":/icons/arrow-left.png";
//  QString iconname = ":/icons/openx.png";
//  QString iconname = ":/icons/open_icon_library/png/64x64/actions/arrow-right-3.png";

  mRemoveAction = createAction(topLayout,
        ":/icons/open_icon_library/png/64x64/actions/dialog-close.png",
        "Remove",
        "Remove all registrations after the current",
        SLOT(removeSlot()));

  mBehindLabel = new QLabel(this);
  mBehindLabel->setToolTip("Number of registrations before current time");
  topLayout->addWidget(mBehindLabel);

  mRewindAction = createAction(topLayout,
      ":/icons/open_icon_library/png/64x64/actions/arrow-left-3.png",
      "Rewind",
      "One step back in registration history",
      SLOT(rewindSlot()));

  mForwardAction = createAction(topLayout,
      ":/icons/open_icon_library/png/64x64/actions/arrow-right-3.png",
      "Rewind",
      "One step forward in registration history",
      SLOT(forwardSlot()));

  mInFrontLabel = new QLabel(this);
  mInFrontLabel->setToolTip("Number of registrations after current time");
  topLayout->addWidget(mInFrontLabel);

  mFastForwardAction = createAction(topLayout,
      ":/icons/open_icon_library/png/64x64/actions/arrow-right-double-3.png",
      "Fast Forward",
      "Step to latest registration",
      SLOT(fastForwardSlot()));

  topLayout->addStretch();

//  mRewindAction = new QAction(QIcon(iconname), "Rewind", this);
//  mRewindAction->setStatusTip("One step back in registration history");
//  connect(mRewindAction, SIGNAL(triggered()), this, SLOT(rewindSlot()));
//  QToolButton* rewindButton = new QToolButton();
//  rewindButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
//  rewindButton->setDefaultAction(mRewindAction);
//  topLayout->addWidget(rewindButton);

//  mRewindButton = new QPushButton("Rewind");
//  mRewindButton->setToolTip("One step back in registration history");
//  connect(mRewindButton, SIGNAL(clicked()), this, SLOT(rewindSlot()));
//  topLayout->addWidget(mRewindButton);
//
//  mRemoveButton = new QPushButton("Remove");
//  mRemoveButton->setToolTip("Remove all registrations after the current");
//  connect(mRemoveButton, SIGNAL(clicked()), this, SLOT(removeSlot()));
//  topLayout->addWidget(mRemoveButton);
//
//  mForwardButton = new QPushButton("Forward");
//  mForwardButton->setToolTip("One step forward in registration history");
//  connect(mForwardButton, SIGNAL(clicked()), this, SLOT(forwardSlot()));
//  topLayout->addWidget(mForwardButton);
//
//  mFastForwardButton = new QPushButton("Fast Forward");
//  mFastForwardButton->setToolTip("Step to latest registration");
//  connect(mFastForwardButton, SIGNAL(clicked()), this, SLOT(fastForwardSlot()));
//  topLayout->addWidget(mFastForwardButton);
}


RegistrationHistoryWidget::~RegistrationHistoryWidget()
{
}

void RegistrationHistoryWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);

  this->reconnectSlot();
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(reconnectSlot()));
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(updateSlot()));

  updateSlot();
}

void RegistrationHistoryWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);

  for (unsigned i=0; i<mHistories.size(); ++i)
  {
    disconnect(mHistories[i].get(), SIGNAL(currentChanged()), this, SLOT(updateSlot()));
  }
  disconnect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(updateSlot()));
  disconnect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(reconnectSlot()));
}

void RegistrationHistoryWidget::reconnectSlot()
{
  for (unsigned i=0; i<mHistories.size(); ++i)
  {
    disconnect(mHistories[i].get(), SIGNAL(currentChanged()), this, SLOT(updateSlot()));
  }

  mHistories = this->getAllRegistrationHistories();

  for (unsigned i=0; i<mHistories.size(); ++i)
  {
    connect(mHistories[i].get(), SIGNAL(currentChanged()), this, SLOT(updateSlot()));
  }
}

/** get a map of all registration times and their corresponding descriptions.
 * Near-simultaneous times are filtered out, keeping only the newest in the group.
 */
std::map<QDateTime,QString> RegistrationHistoryWidget::getRegistrationTimes()
{
  TimeMap retval;

  std::vector<ssc::RegistrationHistoryPtr> allHistories = this->getAllRegistrationHistories();

  retval[QDateTime(QDate(2000,1,1))] = "initial";

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
//  ssc::messageManager()->sendInfo("setting active registration time " + active.toString(ssc::timestampSecondsFormatNice()) + ".");

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
  debugDump();

  QDateTime active = this->getActiveTime();
  if (!active.isValid()) // if invalid: we are already at head
    return;

  ssc::messageManager()->sendInfo("Removing all registration performed later than " + active.toString(ssc::timestampSecondsFormatNice()) + ".");

  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  for (unsigned i=0; i<raw.size(); ++i)
  {
    raw[i]->removeNewerThan(active);
  }

  debugDump();
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

  TimeMap times = this->getRegistrationTimes();

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
//  std::cout << "finished rewind" << std::endl;
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

  if (times.empty())
    return;

  // current points to the timestamp currently in use. end() is current time.
  TimeMap::iterator current = this->findCurrentActiveIter(times);

  if (current==times.end()) // already at end, ignore
    return;
  ++current;

  if (current==times.end() || times.rbegin()->first==current->first) // if at end or at the last position, interpret as end
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
  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  std::vector<ssc::RegistrationTransform> history = mergeHistory(raw);

  TimeMap times = this->getRegistrationTimes();
  std::map<QDateTime,QString>::iterator current = this->findCurrentActiveIter(times);
  int behind = std::min<int>(distance(times.begin(), current), times.size()-1);
  int infront = times.size() - 1 - behind;

  mRewindAction->setText("Rewind (" + qstring_cast(behind) + ")");
//  mRewindAction->setText("Rewind (" + qstring_cast(behind) + ")");
  mForwardAction->setText("Forward (" + qstring_cast(infront) + ")");

  mBehindLabel->setText("(" + qstring_cast(behind) + ")");
  mInFrontLabel->setText("(" + qstring_cast(infront) + ")");

  mRewindAction->setEnabled(behind>0);
  mRewindAction->setEnabled(behind>0);
  mRemoveAction->setEnabled(infront!=0);
  mForwardAction->setEnabled(infront!=0);
  mFastForwardAction->setEnabled(infront!=0);
//  mGroup->adjustSize();
//  this->adjustSize();
//  std::cout << "RegistrationHistoryWidget::updateSlot() " << behind << "/" << infront << std::endl;
}


}//end namespace cx
