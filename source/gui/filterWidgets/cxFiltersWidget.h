/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
