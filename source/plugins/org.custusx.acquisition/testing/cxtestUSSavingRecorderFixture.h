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
