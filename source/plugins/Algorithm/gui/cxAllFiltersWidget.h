#ifndef CXALLFILTERSWIDGET_H_
#define CXALLFILTERSWIDGET_H_

#include "cxBaseWidget.h"

#include "cxBaseWidget.h"
#include "cxFilter.h"
#include "cxFilterTimedAlgorithm.h"
#include "cxFilterWidget.h"

namespace cx {

/** Widget for selecting and running a Filter.
 *
 *  Select one filter from a drop-down list, then set it up
 *  and run it. All available filters in the system should be
 *  in this widget.
 *
 * \ingroup cxPluginAlgorithm
 * \date Nov 18, 2012
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class AllFiltersWidget : public BaseWidget
{
	Q_OBJECT
public:
	AllFiltersWidget(QWidget* parent);
	QString defaultWhatsThis() const;

private slots:
	void filterChangedSlot();
	void toggleDetailsSlot();
	void runFilterSlot();
	void finishedSlot();
private:
	FilterGroupPtr mFilters;
	FilterPtr mCurrentFilter;
	ssc::StringDataAdapterXmlPtr mFilterSelector;
	FilterTimedAlgorithmPtr mThread;

	FilterSetupWidget* mSetupWidget;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;
};

} /* namespace cx */
#endif /* CXALLFILTERSWIDGET_H_ */
