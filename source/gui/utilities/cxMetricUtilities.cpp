#include "cxMetricUtilities.h"

#include "cxFrameMetricWrapper.h"
#include "cxToolMetricWrapper.h"
#include "cxDataInterface.h"
#include "cxLabeledLineEditWidget.h"
#include "cxLogger.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"

namespace
{
template<class T, class SUPER>
boost::shared_ptr<T> castTo(boost::shared_ptr<SUPER> data)
{
	return boost::dynamic_pointer_cast<T>(data);
}

template<class T, class SUPER>
bool isType(boost::shared_ptr<SUPER> data)
{
	return (castTo<T>(data) ? true : false);
}
template<class WRAPPER, class METRIC, class SUPER>
boost::shared_ptr<WRAPPER> createMetricWrapperOfType(cx::VisServicesPtr services, boost::shared_ptr<SUPER> data)
{
	return boost::shared_ptr<WRAPPER>(new WRAPPER(services, castTo<METRIC>(data)));
}
}

namespace cx {

SingleMetricWidget::SingleMetricWidget(QWidget *parent, MetricBasePtr wrapper, QLabel* valueLabel) :
	BaseWidget(parent, wrapper->getType()+"_single_metric_widget", wrapper->getType()+" Metric Widget"),
	mWrapper(wrapper),
	mValueLabel(valueLabel)
{
}

DataPtr SingleMetricWidget::getData()
{
	return mWrapper->getData();
}

void SingleMetricWidget::prePaintEvent()
{
	mWrapper->update();
	QString value = mWrapper->getValue();
	mValueLabel->setText(value);
}


MetricUtilities::MetricUtilities(VisServicesPtr services) :
	mServices(services)
{

}

MetricBasePtr MetricUtilities::createMetricWrapper(DataPtr data)
{
	if (isType<PointMetric>(data))
	  return createMetricWrapperOfType<PointMetricWrapper, PointMetric>(mServices, data);
	if (isType<DistanceMetric>(data))
	  return createMetricWrapperOfType<DistanceMetricWrapper, DistanceMetric>(mServices, data);
	if (isType<AngleMetric>(data))
	  return createMetricWrapperOfType<AngleMetricWrapper, AngleMetric>(mServices, data);
	if (isType<FrameMetric>(data))
	  return createMetricWrapperOfType<FrameMetricWrapper, FrameMetric>(mServices, data);
	if (isType<ToolMetric>(data))
	  return createMetricWrapperOfType<ToolMetricWrapper, ToolMetric>(mServices, data);
	if (isType<PlaneMetric>(data))
	  return createMetricWrapperOfType<PlaneMetricWrapper, PlaneMetric>(mServices, data);
	if (isType<DonutMetric>(data))
	  return createMetricWrapperOfType<DonutMetricWrapper, DonutMetric>(mServices, data);
	if (isType<CustomMetric>(data))
	  return createMetricWrapperOfType<CustomMetricWrapper, CustomMetric>(mServices, data);
	if (isType<SphereMetric>(data))
	  return createMetricWrapperOfType<SphereMetricWrapper, SphereMetric>(mServices, data);
	if (isType<RegionOfInterestMetric>(data))
	  return createMetricWrapperOfType<RegionOfInterestMetricWrapper, RegionOfInterestMetric>(mServices, data);

	return MetricBasePtr();
}

/** create new metric wrappers for all metrics in PaSM
 *
 */
std::vector<MetricBasePtr> MetricUtilities::createMetricWrappers()
{
	std::vector<MetricBasePtr> retval;
	std::map<QString, DataPtr> all = mServices->patient()->getDatas();
	for (std::map<QString, DataPtr>::iterator iter=all.begin(); iter!=all.end(); ++iter)
	{
		MetricBasePtr wrapper = this->createMetricWrapper(iter->second);
		if (wrapper)
		{
			retval.push_back(wrapper);
		}
	}
	return retval;
}

QWidget* MetricUtilities::createMetricWidget(DataPtr data)
{
	MetricBasePtr wrapper = this->createMetricWrapper(data);

	QWidget* widget = wrapper->createWidget();
	wrapper->update();

	QString value = wrapper->getValue();
	QString type = wrapper->getType();

	QLabel* valueLabel = new QLabel(value);

	StringPropertyDataNameEditablePtr nameAdapter = StringPropertyDataNameEditable::New();
	nameAdapter->setData(data);

	SingleMetricWidget* topWidget = new SingleMetricWidget(NULL, wrapper, valueLabel);
	connect(data.get(), &Data::transformChanged, topWidget, &BaseWidget::setModified);

	QGroupBox* groupBox = new QGroupBox("Metric type: "+ type, topWidget);
	groupBox->setFlat(true);
	QVBoxLayout* verticalLayout = new QVBoxLayout(groupBox);
	verticalLayout->setMargin(4);

	QHBoxLayout* valueLayout = new QHBoxLayout();
	valueLayout->addWidget(new QLabel("Value: "));
	valueLayout->addWidget(valueLabel);

	verticalLayout->addWidget(new LabeledLineEditWidget(topWidget, nameAdapter));
	verticalLayout->addLayout(valueLayout);
	verticalLayout->addWidget(widget, 1);

	QHBoxLayout* topLayout = new QHBoxLayout(topWidget);
	topLayout->addWidget(groupBox);

	return topWidget;
}

}//cx