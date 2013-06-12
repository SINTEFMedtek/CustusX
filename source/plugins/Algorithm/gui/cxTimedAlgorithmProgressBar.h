// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXTIMEDALGORITHMPROGRESSBAR_H_
#define CXTIMEDALGORITHMPROGRESSBAR_H_

#include <boost/shared_ptr.hpp>
#include <set>
#include <QWidget>
class QProgressBar;
class QLabel;

namespace cx
{
typedef boost::shared_ptr<class TimedBaseAlgorithm> TimedAlgorithmPtr;
class DisplayTimerWidget;

/**
 * \file
 * \addtogroup cxPluginAlgorithm
 * @{
 */

/**\brief Show progress for a TimedBaseAlgorithm
 *
 * The widget contains a QProgressBar and a QLabel, which
 * displays info from a TimedBaseAlgorithm. It listens to
 * the start()/stop() methods of TimedBaseAlgorithm, it shows/hides
 * and display progress info.
 *
 * Use the attach method to connect the TimedBaseAlgorithm to
 * the widget. Use detach to clean up.
 *
 * Multiple algorithms can be added. In this case, the assumption
 * is that they are called in sequence. If not, the last started
 * algo will overwrite the others.
 *
 *  \date Jan 27, 2012
 *  \author christiana
 */
class TimedAlgorithmProgressBar : public QWidget
{
	Q_OBJECT
public:
	TimedAlgorithmProgressBar(QWidget* parent=NULL);
	virtual ~TimedAlgorithmProgressBar() {}
    void setShowTextLabel(bool on);

	void attach(TimedAlgorithmPtr algorithm);
	void detach(TimedAlgorithmPtr algorithm);

private slots:
	void algorithmStartedSlot(int maxSteps);
	void algorithmFinishedSlot();
	void productChangedSlot();

private:
	void algorithmFinished(TimedBaseAlgorithm* algo);
	std::set<TimedAlgorithmPtr> mAlgorithm;
	QProgressBar* mProgressBar;
	QLabel* mLabel;
//	int mStartedAlgos;
	std::set<TimedBaseAlgorithm*> mStartedAlgos;
	DisplayTimerWidget* mTimerWidget;
    bool mShowTextLabel;
};

/**
 * @}
 */
}
#endif /* CXTIMEDALGORITHMPROGRESSBAR_H_ */
