/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLIVERSEGMENTATIONRUNNER_H_
#define CXLIVERSEGMENTATIONRUNNER_H_

#include <QObject>
#include <QList>
#include <QString>
#include "org_custusx_liver_Export.h"
#include "cxForwardDeclarations.h"

namespace cx
{

typedef boost::shared_ptr<class GenericScriptFilter> GenericScriptFilterPtr;
typedef boost::shared_ptr<class FilterTimedAlgorithm> FilterTimedAlgorithmPtr;

/**
 * One queued filter run: an .ini file to run against a specific image.
 *
 * key identifies this run for progress reporting (see LiverSegmentationWidget) -
 * it is set by the caller rather than derived from image, since image may be
 * a resampled copy of the volume the caller (and its progress bars) actually
 * refer to.
 *
 * \ingroup org_custusx_liver
 */
struct org_custusx_liver_EXPORT QueuedRun
{
	QString iniFileName;
	ImagePtr image;
	QString key;
	// Passed through to TotalSegmentator as "--fast": its own low-resolution
	// (3mm) model, using substantially less memory and time at the cost of
	// coarser output.
	bool fastMode = false;
	// Overrides the memory limit TotalSegmentator's own subprocess is
	// launched under (see _process_utils.py). 0 means "use the automatic,
	// machine-dependent default".
	double memoryLimitGB = 0;
};

/**
 * Runs a queue of liver .ini filter files sequentially, each against its
 * own image, one GenericScriptFilter run at a time.
 *
 * \ingroup org_custusx_liver
 */
class org_custusx_liver_EXPORT LiverSegmentationRunner : public QObject
{
	Q_OBJECT
public:
	explicit LiverSegmentationRunner(VisServicesPtr services, QObject* parent = 0);
	virtual ~LiverSegmentationRunner();

	void start(QList<QueuedRun> queue);
	// Stops the currently running filter (if any) and drops the rest of the
	// queue - allFinished() still fires once the running filter actually stops.
	void stop();
	bool isRunning() const;

signals:
	void filterStarted(QString key);
	// percent is 0-100 for the currently running filter; negative means
	// indeterminate (mirrors the running script's own "PROGRESS: N" output).
	void progressChanged(int percent);
	void allFinished();

private slots:
	void onFilterFinished();
	void onScriptOutput(const QString& line);

private:
	void runNext();
	GenericScriptFilterPtr createFilter(const QueuedRun& run);

	VisServicesPtr mServices;
	QList<QueuedRun> mQueue;
	GenericScriptFilterPtr mCurrentFilter;
	FilterTimedAlgorithmPtr mCurrentThread;
};
typedef boost::shared_ptr<LiverSegmentationRunner> LiverSegmentationRunnerPtr;

} /* namespace cx */

#endif /* CXLIVERSEGMENTATIONRUNNER_H_ */
