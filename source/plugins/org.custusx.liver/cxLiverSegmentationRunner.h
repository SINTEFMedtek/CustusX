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
#include <QStringList>
#include "org_custusx_liver_Export.h"
#include "cxForwardDeclarations.h"

namespace cx
{

typedef boost::shared_ptr<class GenericScriptFilter> GenericScriptFilterPtr;
typedef boost::shared_ptr<class FilterTimedAlgorithm> FilterTimedAlgorithmPtr;

/**
 * Runs a queue of liver .ini filter files sequentially against the same
 * input image, one GenericScriptFilter run at a time.
 *
 * \ingroup org_custusx_liver
 */
class org_custusx_liver_EXPORT LiverSegmentationRunner : public QObject
{
	Q_OBJECT
public:
	explicit LiverSegmentationRunner(VisServicesPtr services, QObject* parent = 0);
	virtual ~LiverSegmentationRunner();

	void start(QStringList iniFileNames, ImagePtr image);
	// Stops the currently running filter (if any) and drops the rest of the
	// queue - allFinished() still fires once the running filter actually stops.
	void stop();
	bool isRunning() const;

signals:
	void filterStarted(QString iniFileName);
	// percent is 0-100 for the currently running filter; negative means
	// indeterminate (mirrors the running script's own "PROGRESS: N" output).
	void progressChanged(int percent);
	void allFinished();

private slots:
	void onFilterFinished();
	void onScriptOutput(const QString& line);

private:
	void runNext();
	GenericScriptFilterPtr createFilter(QString iniFileName);

	VisServicesPtr mServices;
	QStringList mQueue;
	ImagePtr mImage;
	GenericScriptFilterPtr mCurrentFilter;
	FilterTimedAlgorithmPtr mCurrentThread;
};
typedef boost::shared_ptr<LiverSegmentationRunner> LiverSegmentationRunnerPtr;

} /* namespace cx */

#endif /* CXLIVERSEGMENTATIONRUNNER_H_ */
