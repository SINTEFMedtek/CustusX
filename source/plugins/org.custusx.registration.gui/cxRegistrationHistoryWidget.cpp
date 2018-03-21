/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationHistoryWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include "cxImage.h"

#include "cxTime.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxLogger.h"
#include "cxTrackingService.h"

namespace cx
{
RegistrationHistoryWidget::RegistrationHistoryWidget(RegServicesPtr servicesPtr, QWidget* parent, bool compact) :
	BaseWidget(parent, "registration_history_widget", "Registration History"),
	mServices(servicesPtr)
{
	this->setWhatsThis(this->defaultWhatsThis());
	this->setToolTip(this->defaultWhatsThis());

	mGroup = new QFrame;
	mTextEdit = NULL;

	QVBoxLayout* toptopLayout = new QVBoxLayout(this);
	QHBoxLayout* topLayout = new QHBoxLayout;
	this->createControls(topLayout);

	toptopLayout->addWidget(mGroup);
	mGroup->setLayout(topLayout);
	if (!compact)
	{
		mTextEdit = new QTextEdit;
		mTextEdit->setLineWrapMode(QTextEdit::NoWrap);
//		mTextEdit->setVisible(true);
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
	connect(mServices->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &RegistrationHistoryWidget::reconnectSlot);
	connect(mServices->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &RegistrationHistoryWidget::setModified);

	setModified();
}

void RegistrationHistoryWidget::hideEvent(QCloseEvent* event)
{
	QWidget::closeEvent(event);

	for (unsigned i = 0; i < mHistories.size(); ++i)
	{
		disconnect(mHistories[i].get(), &RegistrationHistory::currentChanged, this, &RegistrationHistoryWidget::setModified);
	}
	disconnect(mServices->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &RegistrationHistoryWidget::reconnectSlot);
	disconnect(mServices->patient().get(), &PatientModelService::dataAddedOrRemoved, this, &RegistrationHistoryWidget::setModified);
}

void RegistrationHistoryWidget::reconnectSlot()
{
	for (unsigned i = 0; i < mHistories.size(); ++i)
	{
		disconnect(mHistories[i].get(), &RegistrationHistory::currentChanged, this, &RegistrationHistoryWidget::setModified);
	}

	mHistories = this->getAllRegistrationHistories();

	for (unsigned i = 0; i < mHistories.size(); ++i)
	{
		connect(mHistories[i].get(), &RegistrationHistory::currentChanged, this, &RegistrationHistoryWidget::setModified);
	}
}

/** get a map of all registration times and their corresponding descriptions.
 * Near-simultaneous times are filtered out, keeping only the newest in the group.
 */
RegistrationHistoryWidget::TimeMap RegistrationHistoryWidget::generateRegistrationTimes()
{
	TimeMap retval;

	std::vector<RegistrationHistoryPtr> allHistories = this->getAllRegistrationHistories();

	retval.insert(TimeMapPair(QDateTime(QDate(2000, 1, 1)),"initial"));

	for (unsigned i = 0; i < allHistories.size(); ++i)
	{
		std::vector<RegistrationTransform> current = allHistories[i]->getData();
		for (unsigned j = 0; j < current.size(); ++j)
		{
			retval.insert(TimeMapPair(current[j].mTimestamp, QString("%1 [fixed=%2, moving=%3]").arg(current[j].mType).arg(current[j].mFixed).arg(
					current[j].mMoving)));
		}
		std::vector<ParentSpace> frames = allHistories[i]->getParentSpaces();
		for (unsigned j = 0; j < frames.size(); ++j)
		{
			if(retval.find(frames[j].mTimestamp) == retval.end())//Only print each frame one time
				retval.insert(TimeMapPair(frames[j].mTimestamp, QString("%1 [spaceUid=%2]").arg(frames[j].mType).arg(frames[j].mUid)));
		}
	}

	retval.erase(QDateTime());

	return retval;
}

RegistrationHistoryWidget::TimeMap::iterator RegistrationHistoryWidget::findActiveRegistration(TimeMap& times)
{
	QDateTime activeTime = this->getActiveTime();

	if (!activeTime.isValid())
		return times.end();

	RegistrationHistoryWidget::TimeMapIterators activeRegistrations = findActiveRegistrations(times);
	if (!activeRegistrations.empty())
		return activeRegistrations.back();
	return times.end();
}

RegistrationHistoryWidget::TimeMapIterators RegistrationHistoryWidget::findActiveRegistrations(TimeMap& times)
{
	RegistrationHistoryWidget::TimeMapIterators retval;

	QDateTime activeTime = this->getActiveTime();

	if (!activeTime.isValid())
	{
		retval.push_back(times.end());
		return retval;
	}

	for (TimeMap::iterator iter = times.begin(); iter != times.end(); ++iter)
	{
		if(!retval.empty() && iter->first == retval[0]->first)
			retval.push_back(iter);//Return all registrations at the same point in time
		else if (retval.empty() && iter->first >= activeTime)
			retval.push_back(iter);
	}

	if(retval.empty())
		retval.push_back(times.end());
	return retval;
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
	retval.push_back(mServices->patient()->get_rMpr_History());

	std::map<QString, DataPtr> data = mServices->patient()->getDatas();
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

	TimeMap registrationTimes = this->generateRegistrationTimes();

	if (registrationTimes.size() <= 1)
		return;

	// current points to the timestamp currently in use. end() is current time.
//	std::map<QDateTime, QString>::iterator current = this->findActiveRegistrations(times);
	TimeMapIterators activeRegistrations = this->findActiveRegistrations(registrationTimes);
	TimeMap::iterator activeRegistration = registrationTimes.end();
	if (!activeRegistrations.empty())
		activeRegistration = activeRegistrations.front();

	if (activeRegistration == registrationTimes.begin())
		return;

	if (activeRegistration == registrationTimes.end())
		--activeRegistration; // ignore the last entry

	--activeRegistration;
	report(
			"Rewind: Setting registration time to " + activeRegistration->first.toString(timestampSecondsFormatNice()) + ", ["
					+ activeRegistration->second + "]");
	this->setActiveTime(activeRegistration->first);
}

QString RegistrationHistoryWidget::debugDump()
{
	TimeMap times = this->generateRegistrationTimes();
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
	TimeMap registrationTimes = this->generateRegistrationTimes();

	if (registrationTimes.empty())
		return;

	// current points to the timestamp currently in use. end() is current time.
	TimeMapIterators activeRegistrations = this->findActiveRegistrations(registrationTimes);
	TimeMap::iterator activeRegistration = registrationTimes.end();
	if (!activeRegistrations.empty())
		activeRegistration = activeRegistrations.back();

	if (activeRegistration == registrationTimes.end()) // already at end, ignore
		return;
	++activeRegistration;

	if (activeRegistration == registrationTimes.end() || registrationTimes.rbegin()->first == activeRegistration->first) // if at end or at the last position, interpret as end
	{
		report("Forward: Setting registration time to current, [" + registrationTimes.rbegin()->second + "]");
		this->setActiveTime(QDateTime());
	}
	else
	{
		report("Forward: Setting registration time to " + activeRegistration->first.toString(timestampSecondsFormatNice()) + ", ["+ activeRegistration->second + "]");
		this->setActiveTime(activeRegistration->first);
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

void RegistrationHistoryWidget::prePaintEvent()
{
	TimeMap times = this->generateRegistrationTimes();
	std::map<QDateTime, QString>::iterator current = this->findActiveRegistration(times);
	size_t behind = std::min<int>(distance(times.begin(), current), times.size() - 1);
	size_t infront = times.size() - 1 - behind;

	mRewindAction->setText("Rewind (" + qstring_cast(behind) + ")");
	mForwardAction->setText("Forward (" + qstring_cast(infront) + ")");

	mBehindLabel->setText("(" + qstring_cast(behind) + ")");
	mInFrontLabel->setText("(" + qstring_cast(infront) + ")");

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

	if (mTextEdit)
		mTextEdit->setText(debugDump());
}

} //end namespace cx
