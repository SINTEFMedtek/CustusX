#include "cxOptionsWidget.h"

#include <QLabel>
#include "cxBaseWidget.h"
#include "cxDataAdapterHelper.h"

namespace cx {

OptionsWidget::OptionsWidget(QWidget* parent) :
		mShowAdvanced(false)
{
	this->setSizePolicy(this->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
	mStackedLayout = new QStackedLayout(this);
	mStackedLayout->setMargin(0);
}

void OptionsWidget::setOptions(QString uid, std::vector<SelectDataStringDataAdapterBasePtr> options, bool showAdvanced)
{
	std::vector<DataAdapterPtr> converted;
	std::copy(options.begin(), options.end(), std::back_inserter(converted));
	this->setOptions(uid, converted, showAdvanced);
}

void OptionsWidget::setOptions(QString uid, std::vector<DataAdapterPtr> options, bool showAdvanced)
{
	// return if already on uid
	if (mStackedLayout->currentWidget() && (uid == mStackedLayout->currentWidget()->objectName()))
		return;

	mOptions = options;
	mUid = uid;

	this->showAdvanced(showAdvanced);
}

QString OptionsWidget::getCurrentUid()
{
	return mStackedLayout->currentWidget()->objectName();
}

void OptionsWidget::showAdvanced(bool show)
{
	mShowAdvanced = show;
	this->rebuild();
}

void OptionsWidget::rebuild()
{
	this->clear();
	this->populate(mShowAdvanced);
}

void OptionsWidget::clear()
{
	QLayoutItem *child;
	while ((child = mStackedLayout->takeAt(0)) != 0)
	{
		// delete both the layoutitem AND the widget. Not auto done because layoutitem is no QObject.
		QWidget* widget = child->widget();
		delete child;
		delete widget;
	}
}

void OptionsWidget::populate(bool showAdvanced)
{
	// No existing found,
	// create a new stack element for this uid:
	QWidget* widget = new QWidget(this);
	widget->setObjectName(mUid);
	mStackedLayout->addWidget(widget);
	QGridLayout* layout = new QGridLayout(widget);
	layout->setMargin(layout->margin()/2);

	std::map<QString, QWidget*> groupWidgets;
	QWidget* otherWidget = NULL;
	for (unsigned i = 0; i < mOptions.size(); ++i)
	{
		if(showAdvanced || (!showAdvanced && !mOptions[i]->getAdvanced()))
		{
			QWidget* groupWidget = NULL;
			QGridLayout* groupLayout = NULL;

			//make new group if needed
			QString groupName = mOptions[i]->getGroup();
			if(groupName.isEmpty())
				groupName = "other";
			std::map<QString, QWidget*>::iterator it = groupWidgets.find(groupName);
			if(it == groupWidgets.end())
			{
				groupWidget = new QWidget(widget);
				groupWidget->setObjectName(groupName);
				groupLayout = new QGridLayout(groupWidget);
				groupLayout->setMargin(groupLayout->margin()/2);
				QWidget* temp = this->createGroupHeaderWidget(groupName);
				groupLayout->addWidget(temp,0,0,1,2);
				layout->addWidget(groupWidget);
				groupWidgets[groupName] = groupWidget;
				if(groupName == "other")
					otherWidget = temp;
			}
			else
			{
				groupWidget = it->second;
				groupLayout = (QGridLayout*) groupWidget->layout();
			}

			//count groupwidgets items to determine row
			int itemsInGroup = groupLayout->count();

			//make dataadaptewidget and add to existing group
			blockSignals(true);
			createDataWidget(groupWidget, mOptions[i], groupLayout, ++itemsInGroup);
			blockSignals(false);
		}
	}

	//hide group header if only one the "other" group exists
	if((groupWidgets.size() == 1) && (otherWidget != NULL))
		otherWidget->hide();

	mStackedLayout->setCurrentWidget(widget);
}

QWidget* OptionsWidget::createGroupHeaderWidget(QString title)
{
	QWidget* retval = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(retval);
	layout->setMargin(0);
	layout->setSpacing(0);

	QLabel* label = new QLabel(title);
	QFont font = label->font();
	font.setPointSize(8);
	label->setFont(font);
	layout->addWidget(label);
	layout->addWidget(BaseWidget::createHorizontalLine());

	return retval;
}

} /* namespace cx */
