#ifndef CXOPTIONSWIDGET_H_
#define CXOPTIONSWIDGET_H_

#include <QStackedLayout>
#include "sscDataAdapter.h"
#include "cxSelectDataStringDataAdapter.h"

namespace cx {

/** Widget for displaying a list of DataAdapter instances.
 * A stack of widgets is created if the setOptions() is called with different uids.
 *
 * \ingroup cxPluginAlgorithm
 * \date Nov 20, 2012
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class OptionsWidget : public QWidget
{
	Q_OBJECT
public:
	OptionsWidget(QWidget* parent);

	/**
	  * Set options for a given uid. Setting different uid will change the widget
	  * content but store the previous contents, making it easy to swap between uids.
	  */
	void setOptions(QString uid, std::vector<DataAdapterPtr> options, bool showAdvanced);
	void setOptions(QString uid, std::vector<SelectDataStringDataAdapterBasePtr> options, bool showAdvanced);
	QString getCurrentUid();
	void showAdvanced(bool show); //whether or not to display adapters marked as advanced
	void rebuild();

private:
	void clear();
	void populate(bool showAdvanced);
	QWidget* createGroupHeaderWidget(QString title);

	QStackedLayout* mStackedLayout;
	std::vector<DataAdapterPtr> mOptions;
	QString mUid;
	bool mShowAdvanced;
};
} /* namespace cx */
#endif /* CXOPTIONSWIDGET_H_ */
