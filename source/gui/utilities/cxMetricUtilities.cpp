#include "cxMetricUtilities.h"

#include "cxFrameMetricWrapper.h"
#include "cxToolMetricWrapper.h"

namespace cx {

MetricUtilities::MetricUtilities(ViewServicePtr viewService, PatientModelServicePtr patientModelService) :
    mPatientModelService(patientModelService),
    mViewService(viewService)
{

}

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
boost::shared_ptr<WRAPPER> createMetricWrapperOfType(cx::ViewServicePtr viewService, cx::PatientModelServicePtr patientModelService, boost::shared_ptr<SUPER> data)
{
	return boost::shared_ptr<WRAPPER>(new WRAPPER(viewService, patientModelService, castTo<METRIC>(data)));
}
}

MetricBasePtr MetricUtilities::createMetricWrapper(DataPtr data)
{
	if (isType<PointMetric>(data))
	  return createMetricWrapperOfType<PointMetricWrapper, PointMetric>(mViewService, mPatientModelService, data);
	if (isType<DistanceMetric>(data))
	  return createMetricWrapperOfType<DistanceMetricWrapper, DistanceMetric>(mViewService, mPatientModelService, data);
	if (isType<AngleMetric>(data))
	  return createMetricWrapperOfType<AngleMetricWrapper, AngleMetric>(mViewService, mPatientModelService, data);
	if (isType<FrameMetric>(data))
	  return createMetricWrapperOfType<FrameMetricWrapper, FrameMetric>(mViewService, mPatientModelService, data);
	if (isType<ToolMetric>(data))
	  return createMetricWrapperOfType<ToolMetricWrapper, ToolMetric>(mViewService, mPatientModelService, data);
	if (isType<PlaneMetric>(data))
	  return createMetricWrapperOfType<PlaneMetricWrapper, PlaneMetric>(mViewService, mPatientModelService, data);
	if (isType<DonutMetric>(data))
	  return createMetricWrapperOfType<DonutMetricWrapper, DonutMetric>(mViewService, mPatientModelService, data);
	if (isType<CustomMetric>(data))
	  return createMetricWrapperOfType<CustomMetricWrapper, CustomMetric>(mViewService, mPatientModelService, data);
	if (isType<SphereMetric>(data))
	  return createMetricWrapperOfType<SphereMetricWrapper, SphereMetric>(mViewService, mPatientModelService, data);
	if (isType<RegionOfInterestMetric>(data))
	  return createMetricWrapperOfType<RegionOfInterestMetricWrapper, RegionOfInterestMetric>(mViewService, mPatientModelService, data);

	return MetricBasePtr();
}

/** create new metric wrappers for all metrics in PaSM
 *
 */
std::vector<MetricBasePtr> MetricUtilities::createMetricWrappers()
{
	std::vector<MetricBasePtr> retval;
	std::map<QString, DataPtr> all = mPatientModelService->getData();
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

	QString name = wrapper->getData()->getName();
//	QString value = wrapper->getValue();
//	QString arguments = wrapper->getArguments();
	QString type = wrapper->getType();

	QWidget* topWidget = new QWidget();

	QHBoxLayout* topLayout = new QHBoxLayout(topWidget);
	QHBoxLayout* headerLayout = new QHBoxLayout(topWidget);

	headerLayout->addWidget(new QLabel("Name: "));
	headerLayout->addWidget(new QLabel(name));

	QGroupBox* groupBox = new QGroupBox("Metric type: "+ type, topWidget);
	groupBox->setFlat(true);
	QVBoxLayout* verticalLayout = new QVBoxLayout(groupBox);
	verticalLayout->setMargin(4);

	verticalLayout->addLayout(headerLayout);
	verticalLayout->addWidget(widget, 1);
	topLayout->addWidget(groupBox);

	topWidget->setLayout(topLayout);

	return topWidget;
}

}//cx