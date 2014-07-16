#include "cxRegistrationHistoryWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include "cxImage.h"
#include "cxReporter.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxTime.h"
#include "cxTypeConversions.h"

namespace cx
{
RegistrationHistoryWidget::RegistrationHistoryWidget(QWidget* parent, bool compact) :
		BaseWidget(parent, "RegistrationHistoryWidget", "Registration History")
{
	this->setWhatsThis(this->defaultWhatsThis());
	this->setToolTip(this->defaultWhatsThis());

	mGroup = new QFrame;

	mTextEdit = new QTextEdit;
	mTextEdit->setLineWrapMode(QTextEdit::NoWrap);

	QVBoxLayout* toptopLayout = new QVBoxLayout(this);
	QHBoxLayout* topLayout = new QHBoxLayout;
	this->createControls(topLayout);

	toptopLayout->addWidget(mGroup);
	mGroup->setLayout(topLayout);
	if (!compact)
	{
		mTextEdit->setVisible(true);
		toptopLayout->addWidget(mTextEdit, 1);
		toptopLayout->addStretch();
		topLayout->addStretch();
	}else
	{
		toptopLayout->setContentsMargins(QMargins(0,0,0,0));
		topLayout->setContentsMargins(QMargins(0,0,0,0));
	}

}

RegistrationHistoryWidget::~RegistrationHistoryWidget()
{
}

QString RegistrationHistoryWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Registration history.</h3>"
			"<p>"
			"Use the registration history to rewind the system to previous time. When history is rewinded, "
			"all registrations performed after the active time is ignored by the system."
			"</p>"
			"<p>"
			"<b>NB:</b> While a previous time is active, <em>no new registrations or adding of data</em> should be performed. "
			"This will lead to undefined behaviour!</b>"
			"</p>"
			"</html>";
}

void RegistrationHistoryWidget::createControls(QHBoxLayout* layout)
{
	mRemoveAction = this->createAction(layout, ":/icons/open_icon_library/dialog-close.png", "Remove",
			"Delete all registrations after the active time", SLOT(removeSlot()));

	mBehindLabel = new QLabel(this);
	mBehindLabel->setToolTip("Number of registrations before the active time");
	layout->addWidget(mBehindLabel);

	mRewindAction = this->createAction(layout, ":/icons/open_icon_library/arrow-left-3.png", "Rewind",
					"One step back in registration history, changing active time.\nThis enables looking at a previous system state,\nbut take care to not add more registrations while this state.",
					SLOT(rewindSlot()));

	mForwardAction = this->createAction(layout, ":/icons/open_icon_library/arrow-right-3.png",
			"Rewind", "One step forward in registration history", SLOT(forwardSlot()));

	mInFrontLabel = new QLabel(this);
	mInFrontLabel->setToolTip("Number of registrations after active time");
	layout->addWidget(mInFrontLabel);

	mFastForwardAction = this->createAction(layout,
			":/icons/open_icon_library/arrow-right-double-3.png", "Fast Forward",
			"Step to latest registration", SLOT(fastForwardSlot()));
}

void RegistrationHistoryWidget::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);

	this->reconnectSlot();
	connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(reconnectSlot()));
	connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(updateSlot()));

	updateSlot();
}

void RegistrationHistoryWidget::hideEvent(QCloseEvent* event)
{
	QWidget::closeEvent(event);

	for (unsigned i = 0; i < mHistories.size(); ++i)
	{
		disconnect(mHistories[i].get(), SIGNAL(currentChanged()), this, SLOT(updateSlot()));
	}
	disconnect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(updateSlot()));
	disconnect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(reconnectSlot()));
}

void RegistrationHistoryWidget::reconnectSlot()
{
	for (unsigned i = 0; i < mHistories.size(); ++i)
	{
		disconnect(mHistories[i].get(), SIGNAL(currentChanged()), this, SLOT(updateSlot()));
	}

	mHistories = this->getAllRegistrationHistories();

	for (unsigned i = 0; i < mHistories.size(); ++i)
	{
		connect(mHistories[i].get(), SIGNAL(currentChanged()), this, SLOT(updateSlot()));
	}
}

/** get a map of all registration times and their corresponding descriptions.
 * Near-simultaneous times are filtered out, keeping only the newest in the group.
 */
std::map<QDateTime, QString> RegistrationHistoryWidget::getRegistrationTimes()
{
	TimeMap retval;

	std::vector<RegistrationHistoryPtr> allHistories = this->getAllRegistrationHistories();

	retval[QDateTime(QDate(2000, 1, 1))] = "initial";

	for (unsigned i = 0; i < allHistories.size(); ++i)
	{
		std::vector<RegistrationTransform> current = allHistories[i]->getData();
		for (unsigned j = 0; j < current.size(); ++j)
		{
			retval[current[j].mTimestamp] = QString("%1 [f=%2, m=%3]").arg(current[j].mType).arg(current[j].mFixed).arg(
					current[j].mMoving);
		}
		std::vector<ParentSpace> frames = allHistories[i]->getParentSpaces();
		for (unsigned j = 0; j < frames.size(); ++j)
		{
			retval[frames[j].mTimestamp] = QString("%1 [val=%2]").arg(frames[j].mType).arg(frames[j].mValue);
		}
	}

	retval.erase(QDateTime());

	return retval;
}

RegistrationHistoryWidget::TimeMap::iterator RegistrationHistoryWidget::findCurrentActiveIter(TimeMap& times)
{
	QDateTime active = this->getActiveTime();

	if (!active.isValid())
		return times.end();

	for (TimeMap::iterator iter = times.begin(); iter != times.end(); ++iter)
	{
		if (iter->first >= active)
			return iter;
	}
	return times.end();
}

QDateTime RegistrationHistoryWidget::getActiveTime()
{
	std::vector<RegistrationHistoryPtr> raw = getAllRegistrationHistories();

	for (unsigned i = 0; i < raw.size(); ++i)
	{
		if (raw[i]->isNull())
			continue;
		return raw[i]->getActiveTime();
	}

	return QDateTime();
}

void RegistrationHistoryWidget::setActiveTime(QDateTime active)
{
	std::vector<RegistrationHistoryPtr> raw = getAllRegistrationHistories();
	for (unsigned i = 0; i < raw.size(); ++i)
	{
		raw[i]->setActiveTime(active);
	}
}

/**collect registration histories from the tool manager (patient registration)
 * and images (image registration) and return.
 */
std::vector<RegistrationHistoryPtr> RegistrationHistoryWidget::getAllRegistrationHistories()
{
	std::vector<RegistrationHistoryPtr> retval;
	retval.push_back(dataManager()->get_rMpr_History());

	std::map<QString, DataPtr> data = dataManager()->getData();
	for (std::map<QString, DataPtr>::iterator iter = data.begin(); iter != data.end(); ++iter)
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

	report(
			"Removing all registration performed later than " + active.toString(timestampSecondsFormatNice()) + ".");

	std::vector<RegistrationHistoryPtr> raw = getAllRegistrationHistories();
	for (unsigned i = 0; i < raw.size(); ++i)
	{
		raw[i]->removeNewerThan(active);
	}

	debugDump();
}

std::vector<RegistrationTransform> RegistrationHistoryWidget::mergeHistory(const std::vector<RegistrationHistoryPtr>& allHistories)
{
	std::vector<RegistrationTransform> history;
	for (unsigned i = 0; i < allHistories.size(); ++i)
	{
		std::vector<RegistrationTransform> current = allHistories[i]->getData();
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

	if (times.size() <= 1)
		return;

	// current points to the timestamp currently in use. end() is current time.
	std::map<QDateTime, QString>::iterator current = this->findCurrentActiveIter(times);

	if (current == times.begin())
		return;

	if (current == times.end())
		--current; // ignore the last entry

	--current;
	report(
			"Rewind: Setting registration time to " + current->first.toString(timestampSecondsFormatNice()) + ", ["
					+ current->second + "]");
	this->setActiveTime(current->first);
}

QString RegistrationHistoryWidget::debugDump()
{
	TimeMap times = this->getRegistrationTimes();
	bool addedBreak = false;
	std::stringstream ss;
	ss << "<html>";
	ss << "<p><i>";
	if (!this->getActiveTime().isValid())
		ss << "Active time: Current \n";
	else
		ss << "Active time: " << this->getActiveTime().toString(timestampSecondsFormatNice()) << "\n";
	ss << "</i></p>";

	ss << "<p><span style=\"color:blue\">";
	for (TimeMap::iterator iter = times.begin(); iter != times.end(); ++iter)
	{
		if (iter->first > this->getActiveTime() && !addedBreak && this->getActiveTime().isValid())
		{
			ss << "</span> <span style=\"color:gray\">";
			addedBreak = true;
		}
		//else
		{
			ss << "<br />";
		}
		ss << iter->first.toString(timestampSecondsFormatNice()) << "\t" << iter->second;
	}
	ss << "</span></p>";

	return qstring_cast(ss.str());
}

template<class T>
QAction* RegistrationHistoryWidget::createAction(QLayout* layout, QString iconName, QString text, QString tip, T slot)
{
	QAction* action = new QAction(QIcon(iconName), text, this);
	action->setToolTip(tip);
	connect(action, SIGNAL(triggered()), this, slot);

	QToolButton* button = new QToolButton();
	button->setDefaultAction(action);
	layout->addWidget(button);

	return action;
}

/** jump forward to one second ahead of the NEXT registration
 */
void RegistrationHistoryWidget::forwardSlot()
{
	std::map<QDateTime, QString> times = this->getRegistrationTimes();

	if (times.empty())
		return;

	// current points to the timestamp currently in use. end() is current time.
	TimeMap::iterator current = this->findCurrentActiveIter(times);

	if (current == times.end()) // already at end, ignore
		return;
	++current;

	if (current == times.end() || times.rbegin()->first == current->first) // if at end or at the last position, interpret as end
	{
		report("Forward: Setting registration time to current, [" + times.rbegin()->second + "]");
		this->setActiveTime(QDateTime());
	}
	else
	{
		report("Forward: Setting registration time to " + current->first.toString(timestampSecondsFormatNice()) + ", ["+ current->second + "]");
		this->setActiveTime(current->first);
	}
}

/**Use the newest available registration.
 * Negates any call to usePreviousRegistration.
 */
void RegistrationHistoryWidget::fastForwardSlot()
{
	std::vector<RegistrationHistoryPtr> raw = getAllRegistrationHistories();
	report("Fast Forward: Setting registration time to current.");

	for (unsigned i = 0; i < raw.size(); ++i)
	{
		raw[i]->setActiveTime(QDateTime());
	}
}

void RegistrationHistoryWidget::updateSlot()
{
	std::vector<RegistrationHistoryPtr> raw = getAllRegistrationHistories();
	std::vector<RegistrationTransform> history = mergeHistory(raw);

	TimeMap times = this->getRegistrationTimes();
	std::map<QDateTime, QString>::iterator current = this->findCurrentActiveIter(times);
	size_t behind = std::min<int>(distance(times.begin(), current), times.size() - 1);
	size_t infront = times.size() - 1 - behind;

	mRewindAction->setText("Rewind (" + qstring_cast(behind) + ")");
	mForwardAction->setText("Forward (" + qstring_cast(infront) + ")");

	mBehindLabel->setText("(" + qstring_cast(behind) + ")");
	mInFrontLabel->setText("(" + qstring_cast(infront) + ")");

	mRewindAction->setEnabled(behind > 0);
	mRewindAction->setEnabled(behind > 0);
	mRemoveAction->setEnabled(infront != 0);
	mForwardAction->setEnabled(infront != 0);
	mFastForwardAction->setEnabled(infront != 0);

	QString color;
	if (infront == 0)
	{
		color = "";
	}
	else
	{
		color = QString("QFrame {background: qradialgradient(cx:0.5, cy:0.5, radius: 0.5, fx:0.5, fy:0.5, stop:0 rgb(255,30,0), stop:0.8 rgb(255,50,0), stop:1 transparent) }");
		color += QString("QLabel {background-color: transparent }");
	}

	mGroup->setStyleSheet(color);

	mTextEdit->setText(debugDump());
}

} //end namespace cx
