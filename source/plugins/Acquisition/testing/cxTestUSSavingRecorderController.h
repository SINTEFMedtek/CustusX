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

#ifndef CXTESTUSSAVINGRECORDERCONTROLLER_H
#define CXTESTUSSAVINGRECORDERCONTROLLER_H

#include <QApplication>
#include "boost/function.hpp"
#include "sscForwardDeclarations.h"
#include "cxAcquisitionData.h"
#include "cxUSSavingRecorder.h"
#include "cxUSReconstructInputData.h"

/** Helper class for testing cx::USSavingRecorder
 *
 * \ingroup cxTest
 * \date April 17, 2013
 * \author christiana
 */
class TestUSSavingRecorderController : public QObject
{
	Q_OBJECT

public:
	TestUSSavingRecorderController(QObject* parent);

	void addOperation(boost::function0<void> operation);

	void setTool(ssc::ToolPtr tool);
	void addVideoSource(int width, int height);

	void startRecord();
	void wait(int time);
	void stopRecord();
	void saveAndWaitForCompleted();
	void verifyMemData(QString uid);
	void verifySaveData();

	static QString getDataPath();

private slots:
	void runOperations();
	void dataSaved(QString filename);

private:
	void verifySaveData(QString filename);

	cx::RecordSessionPtr mSession;
	ssc::ToolPtr mTool;
	std::vector<ssc::VideoSourcePtr> mVideo;
	QStringList mSavedData;

	cx::USSavingRecorderPtr mRecorder;
	std::vector<boost::function0<void> > mOperations;
};

#endif // CXTESTUSSAVINGRECORDERCONTROLLER_H
