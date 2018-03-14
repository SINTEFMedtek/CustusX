/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTIMEDALGORITHMPROGRESSBAR_H_
#define CXTIMEDALGORITHMPROGRESSBAR_H_

#include "cxResourceWidgetsExport.h"

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
 * \addtogroup cx_resource_widgets
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
class cxResourceWidgets_EXPORT TimedAlgorithmProgressBar : public QWidget
{
	Q_OBJECT
public:
	TimedAlgorithmProgressBar(QWidget* parent=NULL);
	virtual ~TimedAlgorithmProgressBar() {}
    void setShowTextLabel(bool on);

	void attach(TimedAlgorithmPtr algorithm);
	void attach(std::set<cx::TimedAlgorithmPtr> threads);

	void detach(TimedAlgorithmPtr algorithm);
	void detach(std::set<cx::TimedAlgorithmPtr> threads);


private slots:
	void algorithmStartedSlot(int maxSteps);
	void algorithmFinishedSlot();
	void productChangedSlot();

private:
	void algorithmFinished(TimedBaseAlgorithm* algo);
	std::set<TimedAlgorithmPtr> mAlgorithm;
	QProgressBar* mProgressBar;
	QLabel* mLabel;
	std::set<TimedBaseAlgorithm*> mStartedAlgos;
	DisplayTimerWidget* mTimerWidget;
    bool mShowTextLabel;
};

/**
 * @}
 */
}
#endif /* CXTIMEDALGORITHMPROGRESSBAR_H_ */
