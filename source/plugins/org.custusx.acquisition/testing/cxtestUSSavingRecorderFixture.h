/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTUSSAVINGRECORDERFIXTURE_H
#define CXTESTUSSAVINGRECORDERFIXTURE_H

#include "cxtest_org_custusx_acquisition_export.h"

#include <QApplication>
#include "boost/function.hpp"
#include "cxForwardDeclarations.h"
#include "cxAcquisitionData.h"
#include "cxUSSavingRecorder.h"
#include "cxUSReconstructInputData.h"

namespace cxtest
{

/** Helper class for testing cx::USSavingRecorder
 *
 * \ingroup cxTest
 * \date April 17, 2013
 * \author christiana
 */
class CXTEST_ORG_CUSTUSX_ACQUISITION_EXPORT USSavingRecorderFixture : public QObject
{
	Q_OBJECT

public:
	USSavingRecorderFixture(QObject* parent=NULL);
	~USSavingRecorderFixture();

	void addOperation(boost::function0<void> operation);

	void setTool(cx::ToolPtr tool);
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
	void setUp();
	void tearDown();

	void verifySaveData(QString filename);

	cx::RecordSessionPtr mSession;
	cx::ToolPtr mTool;
	std::vector<cx::VideoSourcePtr> mVideo;
	QStringList mSavedData;

	cx::USSavingRecorderPtr mRecorder;
	std::vector<boost::function0<void> > mOperations;
};

} // namespace cxtest


#endif // CXTESTUSSAVINGRECORDERFIXTURE_H
