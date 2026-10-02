/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKINGSYSTEMCOMPARISON_H_
#define CXTRACKINGSYSTEMCOMPARISON_H_

#include "cxResourceExport.h"

#include <map>
#include <vector>
#include <QObject>
#include <QStringList>
#include <QTimer>
#include "cxToolPositionRecorder.h"

class QProcess;

namespace cx
{
typedef boost::shared_ptr<class VideoService> VideoServicePtr;

/** One tracking system to run in a comparison. */
struct cxResource_EXPORT TrackingComparisonRun
{
	QString implementation; ///< Tracking system implementation: igstk, ndi or openigtlink
	QString command;        ///< Started before and stopped after the run, e.g. an OpenIGTLink tracking server. Empty: none.
	QStringList arguments;
};

struct cxResource_EXPORT TrackingComparisonResult
{
	QString label;
	ToolPositionStatisticsMap statistics;
};

struct cxResource_EXPORT TrackingComparisonSettings
{
	QString toolConfigFile; ///< The tool configuration to use. Its tracking system implementation is replaced for each run.
	QString outputFolder;
	std::vector<TrackingComparisonRun> runs;
	int recordDuration_ms = 10000;
	int settleTime_ms = 2000;          ///< Wait after tracking has started, before recording
	int startTimeout_ms = 30000;
	int commandStartupDelay_ms = 3000; ///< Wait after starting the command, before starting tracking
	int pauseBetweenRuns_ms = 3000;    ///< Lets the previous system release the device
	bool openIGTLinkApplyReference = true; ///< Make OpenIGTLink positions relative to the reference, for servers that send them in tracker space
	QString openIGTLinkHost = "127.0.0.1";
	int openIGTLinkPort = 18944;
};

/** \brief Runs several tracking systems one after the other on the same tools, and records the positions.
 *
 * For each run: write a copy of the tool configuration with the run's tracking system
 * implementation, start the run's command (if any), start tracking, wait for the tools to
 * settle, record the positions for a while (ToolPositionRecorder), stop tracking and the
 * command. OpenIGTLink tracking is started through the video connection, as from the GUI.
 * Writes one CSV file per run and a summary to the output folder.
 *
 * Only one system can use the device at a time, so the runs are never in parallel.
 *
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT TrackingSystemComparison : public QObject
{
	Q_OBJECT
public:
	TrackingSystemComparison(TrackingServicePtr trackingService, VideoServicePtr videoService);
	virtual ~TrackingSystemComparison();

	void start(TrackingComparisonSettings settings);
	void cancel();
	bool isRunning() const;
	bool wasSuccessful() const;
	QString getSummary() const;

	/** Returns the changed configuration, empty on failure. applyReference: OpenIGTLink only, see TrackingComparisonSettings. */
	static QString setTrackingSystemImplementation(QString configXml, QString implementation, bool applyReference = false);
	static QString getRunLabel(int index, QString implementation);
	static QStringList splitArguments(QString arguments); ///< Split on spaces, except inside double quotes
	/** Distance between the mean positions of each tool in two runs, mm. Reference tools are left out. */
	static std::map<QString, double> getMeanDifferences(const ToolPositionStatisticsMap& first, const ToolPositionStatisticsMap& other);
	static QStringList compareRuns(const std::vector<TrackingComparisonResult>& results); ///< Summary lines, each run against the first
	double getMaxMeanDifference_mm() const; ///< Largest difference from the first run, for any tool

signals:
	void progress(QString message);
	void finished(bool success);

private:
	enum Step { sIDLE, sWAIT_FOR_COMMAND, sWAIT_FOR_TRACKING, sSETTLE, sRECORD, sWAIT_FOR_STOP, sPAUSE };

	void onTimer();
	void startRun();
	void startTracking();
	void checkTrackingStarted();
	void startRecording();
	void stopRun(QString result);
	void checkTrackingStopped();
	void finish();
	bool writeConfiguration(QString filename, QString implementation);
	void startCommand(const TrackingComparisonRun& run);
	void stopCommand();
	void openOpenIGTLinkConnection();
	void closeOpenIGTLinkConnection();
	void setOpenIGTLinkOption(QString uid, QString value); ///< Remembers the original value, restored when closing
	void wait(Step step, int duration_ms);
	void log(QString message);
	TrackingComparisonRun currentRun() const;

	TrackingServicePtr mTrackingService;
	VideoServicePtr mVideoService;
	TrackingComparisonSettings mSettings;
	ToolPositionRecorder mRecorder;
	QTimer mTimer;
	QProcess* mProcess;
	Step mStep;
	double mStepDeadline_ms;
	int mRunIndex;
	bool mSuccess;
	bool mOpenIGTLinkOpened;
	QString mOriginalToolConfigFile;
	QString mOriginalConnectionMethod;
	std::map<QString, QString> mOriginalOpenIGTLinkOptions;
	QStringList mSummary;
	std::vector<TrackingComparisonResult> mResults;
};

} // namespace cx

#endif /* CXTRACKINGSYSTEMCOMPARISON_H_ */
