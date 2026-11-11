/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXUSRECONSTRUCTIONRUNNER_H
#define CXUSRECONSTRUCTIONRUNNER_H

#include "org_custusx_usreconstruction_Export.h"
#include <vector>
#include <QFutureWatcher>
#include <QObject>
#include <QPointer>
#include <QStringList>
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxUSReconstructInputData.h"

namespace cx
{
typedef std::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;
typedef std::shared_ptr<class TimedBaseAlgorithm> TimedAlgorithmPtr;
class UsReconstructionExtensionService;

/** \brief Reconstructs the selected outputs from the selected recording, one after the other.
 *
 * The US and angio volumes are reconstructed first, then the angio mesh,
 * then the outputs of each extension.
 *
 * \ingroup org_custusx_usreconstruction
 */
class org_custusx_usreconstruction_EXPORT UsReconstructionRunner : public QObject
{
	Q_OBJECT
public:
	struct Outputs
	{
		Outputs();
		bool mUsVolume;
		bool mAngioVolume;
		bool mAngioMesh; ///< Also gives the angio volume
	};
	struct ExtensionOutputs
	{
		QPointer<UsReconstructionExtensionService> mExtension;
		QStringList mOutputs;
	};

	UsReconstructionRunner(UsReconstructionServicePtr reconstruction, PatientModelServicePtr patientModel);
	virtual ~UsReconstructionRunner();

	bool start(Outputs outputs, std::vector<ExtensionOutputs> extensions);
	void cancel();
	bool isRunning() const;
	bool isReconstructingUs() const; ///< True in the US volume step of a run
	std::vector<DataPtr> getCreatedData() const; ///< The data added to the patient by the last run

signals:
	void progress(int done, int total);
	void statusChanged(QString text);
	void finished();

private slots:
	void usReconstructionFinished();
	void angioMeshFinished();
	void extensionFinished();

private:
	bool startUsReconstruction();
	TimedAlgorithmPtr startUsReconstructionThread(bool angio);
	void startNextAngioMesh();
	void startNextExtension();
	void finish();
	bool hasColorData() const;
	std::vector<ImagePtr> findNewAngioVolumes() const;
	void connectExtension(UsReconstructionExtensionService* extension, bool on);

	UsReconstructionServicePtr mReconstruction;
	PatientModelServicePtr mPatientModel;
	bool mRunning;
	bool mCancel;
	Outputs mOutputs;
	std::vector<ExtensionOutputs> mExtensions;
	USReconstructInputData mRecording; ///< The recording selected at the start, also used by the extensions
	QPointer<UsReconstructionExtensionService> mCurrentExtension;
	QStringList mDataBefore;
	QStringList mCreatedData;
	TimedAlgorithmPtr mUsThread;
	std::vector<ImagePtr> mAngioVolumes;
	ImagePtr mMeshBase;
	QFutureWatcher<vtkPolyDataPtr> mMeshWatcher;
};

} // namespace cx

#endif // CXUSRECONSTRUCTIONRUNNER_H
