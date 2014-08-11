#include "cxDetailedLabeledComboBoxWidget.h"

#include <QAction>
#include <QToolButton>

namespace cx
{
DetailedLabeledComboBoxWidget::DetailedLabeledComboBoxWidget(QWidget* parent, StringDataAdapterPtr adapter, QGridLayout* gridLayout, int row) :
		LabeledComboBoxWidget(parent, adapter, gridLayout, row)
{
	//Add detailed button
	QAction* detailsAction = this->createAction(this,
		  QIcon(":/icons/open_icon_library/system-run-5.png"),
		  "Details", "Show Details",
		  SLOT(toggleDetailsSlot()),
		  NULL);

	QToolButton* detailsButton = new QToolButton();
	detailsButton->setObjectName("DetailedButton");
	detailsButton->setDefaultAction(detailsAction);


	if (gridLayout) // add to input gridlayout
	{
		gridLayout->addWidget(detailsButton, row, 2);
	}
	else // add directly to this
	{
        mTopLayout->addWidget(detailsButton, 2);
	}

    this->setModified();
}

DetailedLabeledComboBoxWidget::~DetailedLabeledComboBoxWidget()
{}

void DetailedLabeledComboBoxWidget::toggleDetailsSlot()
{
	emit detailsTriggered();
}


} /* namespace cxtest */
