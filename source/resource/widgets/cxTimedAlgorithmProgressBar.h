/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
