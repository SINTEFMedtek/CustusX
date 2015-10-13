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
#ifndef CXFILTERWIDGET_H
#define CXFILTERWIDGET_H

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include "cxFilter.h"
#include "cxFilterTimedAlgorithm.h"
#include "cxOptionsWidget.h"

namespace cx
{
typedef boost::shared_ptr<class WidgetObscuredListener> WidgetObscuredListenerPtr;
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
class TimedAlgorithmProgressBar;
class FilterPresetWidget;

/** Helper widget for displaying the input/output/options part of a Filter.
 * Intended to be included in other Filter widgets.
 *
 * \ingroup cx_gui
 * \date Nov 18, 2012
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT FilterSetupWidget : public BaseWidget
{
	Q_OBJECT
public:
	FilterSetupWidget(VisServicesPtr services, QWidget* parent, XmlOptionFile options, bool addFrame);

    void setFilter(FilterPtr filter);
	QString generateHelpText() const;

    void setCompact(bool on); ///< Compact Mode: one group, hide main input/output
	void toggleDetailed();

private slots:
	void obscuredSlot(bool obscured);
	void showAdvancedOptions(int state);
	void rebuildOptions();

private:

	VisServicesPtr mServices;
	XmlOptionFile mOptions;
	FilterPtr mCurrentFilter;

	OptionsWidget* mInputsWidget;
	OptionsWidget* mOutputsWidget;
	OptionsWidget* mOptionsWidget;
	FilterPresetWidget*  mPresetWidget;
	QGroupBox* 	   mOptionsGroupBox;
	QCheckBox*	   mAdvancedButton;
	QGroupBox* 	   mFrame;
	boost::shared_ptr<WidgetObscuredListener> mObscuredListener;
};

}

#endif // CXFILTERWIDGET_H
