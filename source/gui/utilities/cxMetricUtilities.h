#ifndef CXMETRICUTILITIES_H
#define CXMETRICUTILITIES_H

#include "cxViewService.h"
#include "cxPatientModelService.h"
#include "cxDataMetricWrappers.h"
#include "cxBaseWidget.h"

namespace cx
{

class SingleMetricWidget: public BaseWidget
{
	Q_OBJECT
public:
	SingleMetricWidget(QWidget* parent, MetricBasePtr wrapper, QLabel *valueLabel);
	virtual ~SingleMetricWidget()
	{
	}

	DataPtr getData();
protected:

	virtual void prePaintEvent();
	MetricBasePtr mWrapper;
	QLabel* mValueLabel;
};

class MetricUtilities: public QObject
{
public:
	MetricUtilities(ViewServicePtr viewService, PatientModelServicePtr patientModelService);

	MetricBasePtr createMetricWrapper(DataPtr data);
	std::vector<MetricBasePtr> createMetricWrappers();
	QWidget *createMetricWidget(DataPtr data);

private:
	PatientModelServicePtr mPatientModelService;
	ViewServicePtr mViewService;
};

}//cx

#endif // CXMETRICUTILITIES_H
