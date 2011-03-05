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
  QString whatsThis = "<html>"
      "<h3>Registration history.</h3>"
      "<p>"
      "Use the registration history to rewind the system to previous time. When history is rewinded, "
      "all registrations performed after the active time is ignored by the system."
      "</p>"
      "<p>"
      "<b><h4>Note!</h4> While a previous time is active, <em>no new registrations or adding of data</em> should be performed. "
      "This will lead to undefined behaviour!</b>"
      "</p>"
      "</html>";
  this->setWhatsThis(whatsThis);
  this->setStatusTip(whatsThis);

  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
//  toptopLayout->setMargin(0);
  mGroup = new QFrame;
  //mGroup->setFlat(true);
//  mGroup->setTitle("Registration Time Control");
  QHBoxLayout* topLayout = new QHBoxLayout;
  toptopLayout->addWidget(mGroup);
  mGroup->setLayout(topLayout);
//  topLayout->setMargin(20);
//  toptopLayout->setMargin(0);
//  toptopLayout->addLayout(topLayout);
  mTextEdit = new QTextEdit;
  mTextEdit->setVisible(false);
  mTextEdit->setLineWrapMode(QTextEdit::NoWrap);
  toptopLayout->addWidget(mTextEdit, 1);

  toptopLayout->addStretch();

  mRemoveAction = createAction(topLayout,
        ":/icons/open_icon_library/png/64x64/actions/dialog-close.png",
        "Remove",
        "Delete all registrations after the active time",
        SLOT(removeSlot()));

  mBehindLabel = new QLabel(this);
  mBehindLabel->setToolTip("Number of registrations before the active time");
  topLayout->addWidget(mBehindLabel);

  mRewindAction = createAction(topLayout,
      ":/icons/open_icon_library/png/64x64/actions/arrow-left-3.png",
      "Rewind",
      "One step back in registration history, changing active time.\nThis enables looking at a previous system state,\nbut take care to not add more registrations while this state.",
      SLOT(rewindSlot()));

  mForwardAction = createAction(topLayout,
      ":/icons/open_icon_library/png/64x64/actions/arrow-right-3.png",
      "Rewind",
      "One step forward in registration history",
      SLOT(forwardSlot()));

  mInFrontLabel = new QLabel(this);
  mInFrontLabel->setToolTip("Number of registrations after active time");
  topLayout->addWidget(mInFrontLabel);

  mFastForwardAction = createAction(topLayout,
      ":/icons/open_icon_library/png/64x64/actions/arrow-right-double-3.png",
      "Fast Forward",
      "Step to latest registration",
      SLOT(fastForwardSlot()));

  mDetailsAction = createAction(topLayout,
      ":/icons/open_icon_library/png/64x64/actions/system-run-5.png",
      "Details",
      "Show registration history",
      SLOT(showDetailsSlot()));
  mDetailsAction->setCheckable(true);
  mDetailsAction->setChecked(false);

  mShowAllAction = createAction(topLayout,
      ":/icons/open_icon_library/png/64x64/actions/system-run-5.png",
      "Show All",
      "Show all entries, including parent changes",
      SLOT(updateSlot()));
  mShowAllAction->setCheckable(true);
  mShowAllAction->setChecked(false);

  topLayout->addStretch();
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

bool RegistrationHistoryWidget::validRegistrationType(QString type) const
{
  return (type!="Set Parent Frame")&&(type!="From MHD file");
}


RegistrationHistoryWidget::TimeMap RegistrationHistoryWidget::getVisibleRegistrationTimes() const
{
  TimeMap input = this->getRegistrationTimes();

  if (mShowAllAction->isChecked())
    return input;

  TimeMap retval;

  for (TimeMap::const_iterator iter=input.begin(); iter!=input.end(); ++iter)
  {
    if (this->validRegistrationType(iter->second))
      retval.insert(*iter);
  }
  return retval;
}

/** get a map of all registration times and their corresponding descriptions.
 * Near-simultaneous times are filtered out, keeping only the newest in the group.
 */
std::map<QDateTime,QString> RegistrationHistoryWidget::getRegistrationTimes() const
{
  TimeMap retval;

  std::vector<ssc::RegistrationHistoryPtr> allHistories = this->getAllRegistrationHistories();

  retval[QDateTime(QDate(2000,1,1))] = "initial";

  for (unsigned i=0; i<allHistories.size(); ++i)
  {
    std::vector<ssc::RegistrationTransform> current = allHistories[i]->getData();
    for (unsigned j=0; j<current.size(); ++j)
    {
      if (!current[j].mTimestamp.isValid())
        continue;
//      if (!this->validRegistrationType(current[j].mType))
//        continue;
      retval[current[j].mTimestamp] = current[j].mType;
    }
    std::vector<ssc::ParentFrame> frames = allHistories[i]->getParentFrames();
    for (unsigned j=0; j<frames.size(); ++j)
    {
      if (!frames[j].mTimestamp.isValid())
        continue;
//      if (!this->validRegistrationType(frames[j].mType))
//        continue;
      retval[frames[j].mTimestamp] = frames[j].mType;
    }
  }

  return retval;
}

///**Return an iterator to the first time that is at or after the current time.
// *
// */
//RegistrationHistoryWidget::TimeMap::iterator RegistrationHistoryWidget::findCurrentActiveIter(TimeMap& times) const
//{
//  QDateTime active = this->getActiveTime();
//
//  if (!active.isValid())
//    return times.end();
//
//  for (TimeMap::iterator iter=times.begin(); iter!=times.end(); ++iter)
//  {
//    if (active <= iter->first)
//      return iter;
//  }
//  return times.end();
//}
/**Return an iterator to the first time that is at or before the current time.
 *
 */
RegistrationHistoryWidget::TimeMap::iterator RegistrationHistoryWidget::findCurrentActiveIter(TimeMap& times) const
{
  QDateTime active = this->getActiveTime();

  if (!active.isValid())
    return times.end();

  TimeMap::iterator last = times.end();
  for (TimeMap::iterator iter=times.begin(); iter!=times.end(); ++iter)
  {
    if (active < iter->first)
      return last;
    last = iter;
  }
  return last;
}

/** return the current active time
 *
 */
QDateTime RegistrationHistoryWidget::getActiveTime() const
{
  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  if (raw.empty())
    return QDateTime();
  return raw.back()->getActiveTime();
}

/**set a new active time
 *
 */
bool RegistrationHistoryWidget::setActiveTime(QDateTime active)
{
//  std::cout << "diff: " << active.toString(ssc::timestampSecondsFormatNice()) << "  " << this->getActiveTime().toString(ssc::timestampSecondsFormatNice()) << std::endl;
  if (active==this->getActiveTime())
    return false;
//  ssc::messageManager()->sendInfo("Setting active registration time " + active.toString(ssc::timestampSecondsFormatNice()) + ".");

  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  for (unsigned i=0; i<raw.size(); ++i)
  {
    raw[i]->setActiveTime(active);
  }
  return true;
}

/**collect registration histories from the tool manager (patient registration)
 * and images (image registration) and return.
 */
std::vector<ssc::RegistrationHistoryPtr> RegistrationHistoryWidget::getAllRegistrationHistories() const
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

void RegistrationHistoryWidget::rewindSlot()
{
  this->rewindToPreviousVisible();
}

void RegistrationHistoryWidget::forwardSlot()
{
  this->forwardToNextVisible();
}

void RegistrationHistoryWidget::rewindToPreviousVisible()
{
  TimeMap times = this->getRegistrationTimes();
  TimeMap::iterator pos;

  pos = this->findCurrentActiveIter(times);
  pos = this->findCurrentVisible(times, pos);
//  std::cout << "findCurrentVisible " << pos->first.toString(ssc::timestampSecondsFormatNice()) << " " << pos->second << std::endl;
  pos = this->rewindToLastVisible(times, pos);
//  std::cout << "rewindToLastVisible " << pos->first.toString(ssc::timestampSecondsFormatNice()) << " " << pos->second << std::endl;
  pos = this->forwardTowardsNextVisible(times, pos);
//  std::cout << "forwardTowardsNextVisible " << pos->first.toString(ssc::timestampSecondsFormatNice()) << " " << pos->second << std::endl;

  this->setActiveTime(pos->first);
}

void RegistrationHistoryWidget::forwardToNextVisible()
{
  TimeMap times = this->getRegistrationTimes();
  TimeMap::iterator pos;

  pos = this->findCurrentActiveIter(times);
//  std::cout << "findCurrentActiveIter " << pos->first.toString(ssc::timestampSecondsFormatNice()) << " " << pos->second << std::endl;
  pos = this->findCurrentVisible(times, pos);
//  std::cout << "findCurrentVisible " << pos->first.toString(ssc::timestampSecondsFormatNice()) << " " << pos->second << std::endl;
  pos = this->forwardToNextVisible(times, pos);
//  std::cout << "forwardToNextVisible " << pos->first.toString(ssc::timestampSecondsFormatNice()) << " " << pos->second << std::endl;
  pos = this->forwardTowardsNextVisible(times, pos);
//  std::cout << "forwardTowardsNextVisible " << pos->first.toString(ssc::timestampSecondsFormatNice()) << " " << pos->second << std::endl;


  if (pos==times.end())
    this->setActiveTime(QDateTime());
  else
    this->setActiveTime(pos->first);
}

///*find current active position
// */
//RegistrationHistoryWidget::TimeMap::iterator RegistrationHistoryWidget::getCurrentPosition(TimeMap& times)
//{
//  TimeMap::iterator retval = this->findCurrentActiveIter(times);
//  return retval;
//}

/* forward to a visible pos, if not already on one.
 *
 */
RegistrationHistoryWidget::TimeMap::iterator RegistrationHistoryWidget::findCurrentVisible(TimeMap& times, TimeMap::iterator pos)
{
  if (pos==times.end())
    --pos;
  while (pos!=times.begin() && !this->validRegistrationType(pos->second))
    --pos;
  return pos;
}

/*; ///< rewind one visible step
 *
 */
RegistrationHistoryWidget::TimeMap::iterator RegistrationHistoryWidget::rewindToLastVisible(TimeMap& times, TimeMap::iterator pos)
{
  if (pos!=times.begin())
    --pos;
  while (pos!=times.begin() && !this->validRegistrationType(pos->second))
    --pos;
  return pos;
}

/*; ///< forward one visible step
 *
 */
RegistrationHistoryWidget::TimeMap::iterator RegistrationHistoryWidget::forwardToNextVisible(TimeMap& times, TimeMap::iterator pos)
{
  if (pos!=times.end())
    ++pos;
  while (pos!=times.end() && !this->validRegistrationType(pos->second))
    ++pos;
  return pos;
}

/* move forwards until next visible pos is reached. Return iterator just before that.
 *
 */
RegistrationHistoryWidget::TimeMap::iterator RegistrationHistoryWidget::forwardTowardsNextVisible(TimeMap& times, TimeMap::iterator pos)
{
  TimeMap::iterator last = pos;
  ++pos;
  while (pos!=times.end())
  {
    if (this->validRegistrationType(pos->second))
      break;
    last = pos;
    ++pos;
  }
  if (pos==times.end()) // allow forwarding to the end
    return pos;
  return last;
}

QString RegistrationHistoryWidget::debugDump()
{
  TimeMap times = this->getVisibleRegistrationTimes();
//  TimeMap times = this->getRegistrationTimes();
  bool addedBreak = false;
  std::stringstream ss;
  ss << "<html>";
  ss << "<p><i>";
  if (!this->getActiveTime().isValid())
    ss << "Active time: Current";
  else
    ss << "Active time: " << this->getActiveTime().toString(ssc::timestampSecondsFormatNice()) << "";
  ss << "</i></p>";

  ss << "<p><span style=\"color:blue\">";
  for (TimeMap::iterator iter=times.begin(); iter!=times.end(); ++iter)
  {
    if (iter->first.isNull())
      continue;
    if (iter->first > this->getActiveTime() && !addedBreak && this->getActiveTime().isValid())
    {
      ss << "</span> <span style=\"color:gray\">";
//      ss << "</p> <hr /> <p style=\"color:gray\">";
      addedBreak = true;
    }
    //else
    {
      ss << "<br />";
    }
    ss << iter->first.toString(ssc::timestampSecondsFormatNice()) << "\t" << iter->second;
  }
  ss << "</span></p>";

//  std::cout << ss.str() << std::endl;
  return qstring_cast(ss.str());
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

void RegistrationHistoryWidget::showDetailsSlot()
{
  mTextEdit->setVisible(!mTextEdit->isVisible());
}


void RegistrationHistoryWidget::updateSlot()
{
  std::vector<ssc::RegistrationHistoryPtr> raw = getAllRegistrationHistories();
  std::vector<ssc::RegistrationTransform> history = mergeHistory(raw);

  TimeMap times = this->getVisibleRegistrationTimes();
//  std::cout << "active time " << this->getActiveTime().toString(ssc::timestampSecondsFormatNice()) << std::endl;
  TimeMap::iterator current = this->findCurrentActiveIter(times);
//  if (current!=times.end())
//    std::cout << "update current " << current->first.toString(ssc::timestampSecondsFormatNice()) << " " << current->second << std::endl;
  current = this->findCurrentVisible(times, current);
//  if (current!=times.end())
//    std::cout << "update current visible " << current->first.toString(ssc::timestampSecondsFormatNice()) << " " << current->second << std::endl;


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

  QString color;
  if(infront==0)
  {
//    color = QString("QFrame { background-color: green }");
//    color = QString("QFrame { border-color: red blue; border-width: 2px 4px }");
//    color = QString("QFrame {background: qradialgradient(cx:0.5, cy:0.5, radius: 1, fx:0.5, fy:0.5, stop:0 rgb(255,30,0), stop:0.3 rgb(255,50,0), stop:1 lightgray) }");
//    color += QString("QLabel { background-color: transparent }");
//    color = QString("QFrame {background: qconicalgradient(cx:0.5, cy:0.5, angle:30, stop:0 white, stop:1 #00FF00) }");
    color = "";
  }
  else
  {
    color = QString("QFrame {background: qradialgradient(cx:0.5, cy:0.5, radius: 0.5, fx:0.5, fy:0.5, stop:0 rgb(255,30,0), stop:0.8 rgb(255,50,0), stop:1 transparent) }");
    color += QString("QLabel { background-color: transparent }");
  }

  mGroup->setStyleSheet(color);

  mTextEdit->setText(debugDump());


}


}//end namespace cx
