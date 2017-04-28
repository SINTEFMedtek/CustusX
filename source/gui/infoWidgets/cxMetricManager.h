/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	void importMetricsFromMNITagFile(QString& filename);
	PointMetricPtr addPoint(Vector3D point, CoordinateSystem space=CoordinateSystem(csREF), QString uid="point%1",  QColor color = QColor(240, 170, 255, 255));

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
};


} // namespace cx

#endif // CXMETRICMANAGER_H
