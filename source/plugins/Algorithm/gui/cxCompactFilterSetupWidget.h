#ifndef CXCOMPACTFILTERSETUPWIDGET_H_
#define CXCOMPACTFILTERSETUPWIDGET_H_

#include "cxBaseWidget.h"
#include "cxFilterWidget.h"
#include "cxFilter.h"

namespace cx {

/**
 * \brief Helper widget for displaying the input/output/options part of a Filter.
 * Intended to be included in other Filter widgets.
 *
 * \ingroup cxPluginAlgorithm
 * \date Dec 13, 2012
 * \author Christian Askeland, SINTEF
 */
class CompactFilterSetupWidget : public BaseWidget
{
	Q_OBJECT

public:
	CompactFilterSetupWidget(QWidget* parent, ssc::XmlOptionFile options, bool addFrame);
	void setFilter(FilterPtr filter);
	QString defaultWhatsThis() const;

private slots:
	void obscuredSlot(bool obscured);

private:
	ssc::XmlOptionFile mOptions;
	FilterPtr mCurrentFilter;
	OptionsWidget* mOptionsWidget;
	QGroupBox* mFrame;
	boost::shared_ptr<WidgetObscuredListener> mObscuredListener;
};
} /* namespace cx */
#endif /* CXCOMPACTFILTERSETUPWIDGET_H_ */
