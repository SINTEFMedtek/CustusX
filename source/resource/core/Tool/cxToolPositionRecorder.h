/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOOLPOSITIONRECORDER_H_
#define CXTOOLPOSITIONRECORDER_H_

#include "cxResourceExport.h"

#include <map>
#include <vector>
#include <QFile>
#include <QObject>
#include "cxTool.h"
#include "cxVector3D.h"

namespace cx
{
typedef boost::shared_ptr<class TrackingService> TrackingServicePtr;

/** \brief Position statistics for one tool: rate, mean position and spread.
 *
 * \ingroup cx_resource_core_tool
 */
struct cxResource_EXPORT ToolPositionStatistics
{
	int positions = 0;
	int positionsWithWarning = 0;
	int visibilityChanges = 0;
	double firstTimestamp_ms = 0.0;
	double lastTimestamp_ms = 0.0;
	Vector3D mean = Vector3D::Zero();
	Vector3D sumSquaredDeviations = Vector3D::Zero();

	void addPosition(const Vector3D& position, double timestamp_ms, bool warning);
	Vector3D getStandardDeviation() const;
	double getRate_Hz() const;
	QString toString() const;
};

/** \brief Records tool positions, visibility and position warnings to a CSV file.
 *
 * Used for comparing tracking systems and for sampling positions while moving tools
 * in and out of the measurement volume. One row per event: a position, the tool going
 * visible/hidden, or a changed position warning. Positions are prMt (tool tip in the
 * patient reference space), in mm.
 *
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT ToolPositionRecorder : public QObject
{
	Q_OBJECT
public:
	explicit ToolPositionRecorder(TrackingServicePtr trackingService = TrackingServicePtr());
	virtual ~ToolPositionRecorder();

	void setTools(std::vector<ToolPtr> tools); ///< Record these tools instead of the tracking service's
	bool start(QString filename, QString label); ///< label is written in the first column, e.g. the tracking system
	void stop();
	bool isRecording() const;
	QString getFilename() const;
	int getRowCount() const;
	std::map<QString, ToolPositionStatistics> getStatistics() const; ///< By tool uid
	QString getSummary() const;

	static QString getHeader();
	static QString formatRow(QString label, QString toolUid, QString event, double timestamp_ms, double received_ms,
							 bool visible, const Transform3D& prMt, QString warning, QString metadata);
	static QString quoteCsv(QString text);

signals:
	void rowCountChanged(int rows);

private:
	void connectToTools();
	void disconnectFromTools();
	std::vector<ToolPtr> getToolsToRecord() const;
	void onTransform(ToolPtr tool, const Transform3D& prMt, double timestamp_ms);
	void onEvent(ToolPtr tool, QString event);
	void writeRow(const QString& row);

	TrackingServicePtr mTrackingService;
	std::vector<ToolPtr> mTools;
	std::vector<QMetaObject::Connection> mConnections;
	QMetaObject::Connection mStateConnection;
	QFile mFile;
	QString mLabel;
	int mRowCount;
	std::map<QString, ToolPositionStatistics> mStatistics;
};
typedef boost::shared_ptr<ToolPositionRecorder> ToolPositionRecorderPtr;

} // namespace cx

#endif /* CXTOOLPOSITIONRECORDER_H_ */
