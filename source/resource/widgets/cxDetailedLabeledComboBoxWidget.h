#ifndef CXDETAILEDLABELEDCOMBOBOXWIDGET_H_
#define CXDETAILEDLABELEDCOMBOBOXWIDGET_H_

#include "cxLabeledComboBoxWidget.h"

namespace cx
{
/**\brief Composite widget for string selection with .
 *
 * Consists of <namelabel, combobox, detailsbutton>.
 *
 * \ingroup cx_resource_widgets
 */
class DetailedLabeledComboBoxWidget : public LabeledComboBoxWidget
{
	Q_OBJECT

public:
	DetailedLabeledComboBoxWidget(QWidget* parent, StringDataAdapterPtr, QGridLayout* gridLayout = 0, int row = 0);
	virtual ~DetailedLabeledComboBoxWidget();

signals:
	void detailsTriggered();

private slots:
	void toggleDetailsSlot();
};

} /* namespace cxtest */

#endif /* CXDETAILEDLABELEDCOMBOBOXWIDGET_H_ */
