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
class TimedAlgorithmProgressBar;

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
	bool isRunning() const;
	TimedAlgorithmProgressBar* getProgressBar();

signals:
	void filterStarted(QString iniFileName);
	void allFinished();

private slots:
	void onFilterFinished();

private:
	void runNext();
	GenericScriptFilterPtr createFilter(QString iniFileName);

	VisServicesPtr mServices;
	TimedAlgorithmProgressBar* mProgressBar;
	QStringList mQueue;
	ImagePtr mImage;
	GenericScriptFilterPtr mCurrentFilter;
	FilterTimedAlgorithmPtr mCurrentThread;
};
typedef boost::shared_ptr<LiverSegmentationRunner> LiverSegmentationRunnerPtr;

} /* namespace cx */

#endif /* CXLIVERSEGMENTATIONRUNNER_H_ */
