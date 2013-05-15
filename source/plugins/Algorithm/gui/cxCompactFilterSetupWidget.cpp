#include "cxCompactFilterSetupWidget.h"

#include "cxThresholdPreview.h"
#include "cxSelectDataStringDataAdapter.h"

namespace cx {

CompactFilterSetupWidget::CompactFilterSetupWidget(QWidget* parent, ssc::XmlOptionFile options, bool addFrame) :
    BaseWidget(parent, "FilterSetupWidget", "FilterSetup")
{
	mFrame = NULL;

	QVBoxLayout* toptopLayout = new QVBoxLayout(this);
	toptopLayout->setMargin(0);

	mOptionsWidget = new OptionsWidget(this);

	if (addFrame)
	{
		mFrame = this->wrapInGroupBox(mOptionsWidget, "Algorithm");
		toptopLayout->addWidget(mFrame);
	}
	else
		toptopLayout->addWidget(mOptionsWidget);

	mObscuredListener.reset(new WidgetObscuredListener(this));
	connect(mObscuredListener.get(), SIGNAL(obscured(bool)), this, SLOT(obscuredSlot(bool)));

	mOptions = options;
}

void CompactFilterSetupWidget::obscuredSlot(bool obscured)
{
	if (mCurrentFilter)
		mCurrentFilter->setActive(!obscured);
}

QString CompactFilterSetupWidget::defaultWhatsThis() const
{
	QString name("None");
	QString help("");
	if (mCurrentFilter)
	{
		name = mCurrentFilter->getName();
		help = mCurrentFilter->getHelp();
	}
	return QString("<html>"
	               "<h4>%1</h4>"
	               "<p>%2</p>"
	               "</html>").arg(name).arg(help);
}

void CompactFilterSetupWidget::setFilter(FilterPtr filter)
{
	if (filter==mCurrentFilter)
		return;

	if (mCurrentFilter)
		mCurrentFilter->setActive(false);

	mCurrentFilter = filter;

	if (mFrame)
		mFrame->setTitle(mCurrentFilter->getName());

	if (mCurrentFilter)
	{
		mCurrentFilter->setActive(!mObscuredListener->isObscured());

		std::vector<SelectDataStringDataAdapterBasePtr> inputTypes = mCurrentFilter->getInputTypes();
		std::vector<SelectDataStringDataAdapterBasePtr> outputTypes = mCurrentFilter->getOutputTypes();
		std::vector<DataAdapterPtr> options = mCurrentFilter->getOptions();

		std::vector<DataAdapterPtr> all;
		std::remove_copy(inputTypes.begin(), inputTypes.end(), std::back_inserter(all), inputTypes[0]);
		std::remove_copy(outputTypes.begin(), outputTypes.end(), std::back_inserter(all), outputTypes[0]);
		std::copy(options.begin(), options.end(), std::back_inserter(all));

		mOptionsWidget->setOptions(mCurrentFilter->getUid(), all, false);
	}
	else
		mOptionsWidget->setOptions("", std::vector<DataAdapterPtr>(), false);
}
} /* namespace cx */
