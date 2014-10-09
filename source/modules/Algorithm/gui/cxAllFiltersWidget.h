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

#ifndef CXALLFILTERSWIDGET_H_
#define CXALLFILTERSWIDGET_H_

#include "cxPluginAlgorithmExport.h"

#include "cxBaseWidget.h"
#include "cxFilter.h"
#include "cxFilterTimedAlgorithm.h"
#include "cxFilterWidget.h"
#include "cxServiceTrackerListener.h"

namespace cx {

/** Widget for selecting and running a Filter.
 *
 *  Select one filter from a drop-down list, then set it up
 *  and run it. All available filters in the system should be
 *  in this widget.
 *
 * \ingroup cx_module_algorithm
 * \date Nov 18, 2012
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class cxPluginAlgorithm_EXPORT AllFiltersWidget : public BaseWidget
{
	Q_OBJECT
public:
	AllFiltersWidget(VisualizationServicePtr visualizationService, PatientModelServicePtr patientModelService, QWidget* parent);
	QString defaultWhatsThis() const;

private slots:
	void filterChangedSlot();
	void toggleDetailsSlot();
	void runFilterSlot();
	void finishedSlot();
private:
	FilterGroupPtr mFilters;
	FilterPtr mCurrentFilter;
	StringDataAdapterXmlPtr mFilterSelector;
	FilterTimedAlgorithmPtr mThread;

	FilterSetupWidget* mSetupWidget;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;

	boost::shared_ptr<ServiceTrackerListener<Filter> > mServiceListener;
	void onServiceAdded(Filter* service);
	void onServiceRemoved(Filter *service);
};

} /* namespace cx */
#endif /* CXALLFILTERSWIDGET_H_ */
