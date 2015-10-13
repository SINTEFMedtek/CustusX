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

#ifndef CXFiltersWidget_H_
#define CXFiltersWidget_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include "cxFilter.h"
#include "cxFilterGroup.h"
#include "cxFilterTimedAlgorithm.h"
#include "cxFilterSetupWidget.h"
#include "cxServiceTrackerListener.h"

namespace cx {
typedef boost::shared_ptr<class VisServices> VisServicesPtr;

/** Widget for selecting and running a Filter.
 *
 *  Select one filter from a drop-down list, then set it up
 *  and run it. All available filters in the system should be
 *  in this widget.
 *
 * \ingroup cx_gui
 * \date Nov 18, 2012
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT FiltersWidget : public BaseWidget
{
	Q_OBJECT
public:
    /**
     * @brief FiltersWidget Widget for displaying N image filters.
     * @param services
     * @param parent
     * @param wantedFilters Specify which filters should be availble in the widget
     * @param optionfileTag Specify under which tag the options for this widget should be saved
     */
    FiltersWidget(VisServicesPtr services, QWidget* parent, QStringList wantedFilters = QStringList(), QString optionfileTag="filterwidget");
    QString generateHelpText() const;

protected slots:
    void addRunButton(QHBoxLayout* filterLayout);

private slots:
	void filterChangedSlot();
	void toggleDetailsSlot();
	void runFilterSlot();
	void finishedSlot();

private:
    void onServiceAdded(Filter* service);
    void onServiceRemoved(Filter *service);
    void appendFiltersThatAreNotServices(VisServicesPtr services);
    void appendFilterServices();
    void appendFilters(VisServicesPtr services);
    void appendFilterIfWanted(FilterPtr filter);
    void configureFilterSelector(XmlOptionFile options);
    void addDetailedButton(QHBoxLayout* filterLayout);
    QHBoxLayout * addFilterSelector(QVBoxLayout* topLayout);
    void addProgressBar(QVBoxLayout* topLayout);
    void addFilterWidget(XmlOptionFile options, VisServicesPtr services, QVBoxLayout* topLayout);
    void setWindowTitleAndObjectNameBasedOnWantedFilters();
    void setupLayout(VisServicesPtr services, XmlOptionFile options);

    QStringList mWantedFilters; //empty list means all available filters
	FilterGroupPtr mFilters;
	FilterPtr mCurrentFilter;
	StringPropertyPtr mFilterSelector;
	FilterTimedAlgorithmPtr mThread;
	FilterSetupWidget* mSetupWidget;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;
	boost::shared_ptr<ServiceTrackerListener<Filter> > mServiceListener;
};

} /* namespace cx */
#endif /* CXFiltersWidget_H_ */
