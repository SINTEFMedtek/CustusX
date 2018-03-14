/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMETRICMANAGER_H
#define CXMETRICMANAGER_H

#include "cxGuiExport.h"

#include "cxForwardDeclarations.h"
#include "cxDataMetric.h"
#include "cxDefinitions.h"
#include "cxCoordinateSystemHelpers.h"

class QDomElement;

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
class cxGui_EXPORT MetricManager : public QObject
{
	Q_OBJECT
public:
	MetricManager(ViewServicePtr viewService, PatientModelServicePtr patientModelService, TrackingServicePtr trackingService, SpaceProviderPtr spaceProvider);

	DataMetricPtr getMetric(QString uid);
	int getNumberOfMetrics() const;
	void moveToMetric(QString uid);
	void setSelection(std::set<QString> selection);
	void setActiveUid(QString uid);
	QString getActiveUid() const { return mActiveLandmark; }
	void exportMetricsToXMLFile(QString& filename);
	void importMetricsFromXMLFile(QString& filename);
	void importMetricsFromMNITagFile(QString& filename, bool testmode=false); ///< Note: testmode is available to skip dialog popup for running automatic tests
	PointMetricPtr addPoint(Vector3D point, CoordinateSystem space=CoordinateSystem(csREF), QString uid="point%1",  QColor color = QColor(240, 170, 255, 255));
	DistanceMetricPtr addDistance(QString uid = "distance%1");

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
    void addCustomButtonClickedSlot();
	void addROIButtonClickedSlot();

private:
	void setManualToolPosition(Vector3D p_r);
	std::vector<DataPtr> refinePointArguments(std::vector<DataPtr> args, unsigned argNo);
	std::vector<DataPtr> getSpecifiedNumberOfValidArguments(MetricReferenceArgumentListPtr arguments, int numberOfRequiredArguments=-1);
	void installNewMetric(DataMetricPtr metric);
	PointMetricPtr addPointInDefaultPosition();
	std::vector<DataMetricPtr> getAllMetrics() const;
	DataPtr loadDataFromXMLNode(QDomElement node);
	DataPtr createData(QString type, QString uid, QString name);

	QString mActiveLandmark; ///< uid of surrently selected landmark.
	std::set<QString> mSelection;
	PatientModelServicePtr mPatientModelService;
	ViewServicePtr mViewService;
	TrackingServicePtr mTrackingService;
	SpaceProviderPtr mSpaceProvider;
	QColor getRandomColor();
	std::vector<QString> dialogForSelectingVolumesForImportedMNITagFile(int number_of_volumes, QString description);
	void resolveUnknownParentSpacesForPointMetrics(QDomNode dataNode, std::map<QString, QString> mapping_of_unknown_to_known_spaces, DataPtr data);
};


} // namespace cx

#endif // CXMETRICMANAGER_H
