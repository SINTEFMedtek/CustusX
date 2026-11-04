/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTESTFAKERECONSTRUCTIONEXTENSION_H
#define CXTESTFAKERECONSTRUCTIONEXTENSION_H

#include "cxtest_org_custusx_usreconstruction_export.h"
#include <QLabel>
#include <QTimer>
#include "cxUsReconstructionExtensionService.h"
#include "cxPatientModelService.h"
#include "cxImage.h"
#include "cxVolumeHelpers.h"

namespace cxtest
{

/** An extension that adds an image with the uid "fake_<output>" for each started output. */
class CXTEST_ORG_CUSTUSX_USRECONSTRUCTION_EXPORT FakeReconstructionExtension : public cx::UsReconstructionExtensionService
{
	Q_OBJECT
public:
	FakeReconstructionExtension(cx::PatientModelServicePtr patientModel) : mPatientModel(patientModel), mRunning(false) {}
	virtual QString getName() const { return "Fake"; }
	virtual std::vector<Output> getOutputs() const
	{
		return std::vector<Output>{Output{"fakeA", "Fake A", ""}, Output{"fakeB", "Fake B", ""}};
	}
	virtual QWidget* createSettingsWidget(QWidget* parent) { return new QLabel("Fake settings", parent); }
	virtual bool start(QStringList outputUids)
	{
		mStarted << outputUids;
		mRunning = true;
		QTimer::singleShot(0, this, [this, outputUids]()
		{
			for (int i=0; i<outputUids.size(); ++i)
			{
				QString uid = "fake_" + outputUids[i];
				mPatientModel->insertData(cx::ImagePtr(new cx::Image(uid, cx::generateVtkImageData(Eigen::Array3i(2, 2, 2), cx::Vector3D(1, 1, 1), 0), uid)));
			}
			mRunning = false;
			emit finished();
		});
		return true;
	}
	virtual void cancel() {}
	virtual bool isRunning() const { return mRunning; }

	QStringList mStarted;

private:
	cx::PatientModelServicePtr mPatientModel;
	bool mRunning;
};

} // namespace cxtest

#endif // CXTESTFAKERECONSTRUCTIONEXTENSION_H
