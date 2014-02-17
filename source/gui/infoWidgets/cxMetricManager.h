// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXMETRICMANAGER_H
#define CXMETRICMANAGER_H

#include "cxForwardDeclarations.h"
#include "sscDataMetric.h"

namespace cx
{

typedef boost::shared_ptr<class MetricReferenceArgumentList> MetricReferenceArgumentListPtr;
typedef boost::shared_ptr<class MetricManager> MetricManagerPtr;
/** 
 *
 *
 * \ingroup cx
 * \date 2014-02-17
 * \author christiana
 */
class MetricManager : public QObject
{
	Q_OBJECT
public:
	MetricManager();

	DataMetricPtr getMetric(QString uid);
	void moveToMetric(QString uid);
	void setSelection(std::set<QString> selection);
	void setActiveUid(QString uid);
	QString getActiveUid() const { return mActiveLandmark; }
	void exportMetricsToFile(QString filename);

signals:
	void activeMetricChanged();
	void metricsChanged();

public slots:
	void loadReferencePointsSlot();
	void addPointButtonClickedSlot();
	void addFrameButtonClickedSlot();
	void addToolButtonClickedSlot();
	void addPlaneButtonClickedSlot();
	void addAngleButtonClickedSlot();
	void addDistanceButtonClickedSlot();
	void addSphereButtonClickedSlot();
	void addDonutButtonClickedSlot();

private:
	void setManualToolPosition(Vector3D p_r);
	PointMetricPtr addPoint(Vector3D point, CoordinateSystem space=CoordinateSystem(csREF), QString name="point%1");
	std::vector<DataPtr> refinePointArguments(std::vector<DataPtr> args, unsigned argNo);
	std::vector<DataPtr> getSpecifiedNumberOfValidArguments(MetricReferenceArgumentListPtr arguments, int numberOfRequiredArguments=-1);
	void installNewMetric(DataMetricPtr metric);
	PointMetricPtr addPointInDefaultPosition();
	std::vector<DataMetricPtr> getAllMetrics();

	QString mActiveLandmark; ///< uid of surrently selected landmark.
	std::set<QString> mSelection;
};


} // namespace cx

#endif // CXMETRICMANAGER_H
