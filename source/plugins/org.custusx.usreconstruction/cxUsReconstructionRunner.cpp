/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxUsReconstructionRunner.h"

#include <QtConcurrent>
#include <vtkPolyData.h>
#include "cxContourFilter.h"
#include "cxImage.h"
#include "cxLogger.h"
#include "cxPatientModelService.h"
#include "cxProperty.h"
#include "cxTimedAlgorithm.h"
#include "cxUSReconstructInputData.h"
#include "cxUsReconstructionExtensionService.h"
#include "cxUsReconstructionService.h"

namespace cx
{

UsReconstructionRunner::Outputs::Outputs() :
	mUsVolume(true),
	mAngioVolume(false),
	mAngioMesh(false)
{
}

UsReconstructionRunner::UsReconstructionRunner(UsReconstructionServicePtr reconstruction, PatientModelServicePtr patientModel) :
	mReconstruction(reconstruction),
	mPatientModel(patientModel),
	mRunning(false),
	mCancel(false)
{
	connect(&mMeshWatcher, &QFutureWatcher<vtkPolyDataPtr>::finished, this, &UsReconstructionRunner::angioMeshFinished);
}

UsReconstructionRunner::~UsReconstructionRunner()
{
	mMeshWatcher.waitForFinished();
}

/** Returns false if a run is going on, or no recording is selected. */
bool UsReconstructionRunner::start(Outputs outputs, std::vector<ExtensionOutputs> extensions)
{
	bool retval = !mRunning && !mReconstruction->getSelectedFilename().isEmpty();
	if (retval)
	{
		mRunning = true;
		mCancel = false;
		mOutputs = outputs;
		mExtensions = extensions;
		mDataBefore = QStringList();
		mCreatedData = QStringList();
		std::map<QString, DataPtr> data = mPatientModel->getDatas();
		for (std::map<QString, DataPtr>::iterator iter = data.begin(); iter != data.end(); ++iter)
		{
			mDataBefore << iter->first;
		}
		if (!this->startUsReconstruction())
		{
			this->startNextExtension();
		}
	}
	return retval;
}

void UsReconstructionRunner::cancel()
{
	mCancel = true;
	emit statusChanged("Cancelling after the current step");
	if (mCurrentExtension)
	{
		mCurrentExtension->cancel();
	}
}

bool UsReconstructionRunner::isRunning() const
{
	return mRunning;
}

void UsReconstructionRunner::setAngioParameters(UsReconstructionServicePtr reconstruction, bool usVolume, bool angio)
{
	PropertyPtr preset = reconstruction->getParam("Preset");
	QString presetName = preset->getValueAsVariant().toString();
	if (angio && presetName == "US B-Mode")
	{
		preset->setValueFromVariant("US Angio");
	}
	else if (!angio && presetName == "US Angio")
	{
		preset->setValueFromVariant("US B-Mode");
	}
	reconstruction->getParam("Angio data")->setValueFromVariant(angio);
	reconstruction->getParam("Dual Angio")->setValueFromVariant(usVolume);
}

bool UsReconstructionRunner::hasColorData() const
{
	return !mReconstruction->getSelectedFileData().is8bit();
}

/** Returns false if no US or angio volume is reconstructed. */
bool UsReconstructionRunner::startUsReconstruction()
{
	bool angio = mOutputs.mAngioVolume || mOutputs.mAngioMesh;
	if (angio && !this->hasColorData())
	{
		reportWarning("The recording has no color data, so no angio volume is reconstructed");
		angio = false;
	}
	mUsThread.reset();
	if (mOutputs.mUsVolume || angio)
	{
		emit statusChanged("Reconstructing US volumes");
		connect(mReconstruction.get(), &UsReconstructionService::reconstructFinished, this, &UsReconstructionRunner::usReconstructionFinished);
		mUsThread = this->startUsReconstructionThread(angio);
		if (!mUsThread)
		{
			disconnect(mReconstruction.get(), &UsReconstructionService::reconstructFinished, this, &UsReconstructionRunner::usReconstructionFinished);
		}
	}
	return bool(mUsThread);
}

/** The angio parameters are set for this reconstruction only. Returns the thread of the new reconstruction, or null. */
TimedAlgorithmPtr UsReconstructionRunner::startUsReconstructionThread(bool angio)
{
	QVariant preset = mReconstruction->getParam("Preset")->getValueAsVariant();
	QVariant angioData = mReconstruction->getParam("Angio data")->getValueAsVariant();
	QVariant dualAngio = mReconstruction->getParam("Dual Angio")->getValueAsVariant();
	std::set<TimedAlgorithmPtr> before = mReconstruction->getThreadedReconstruction();
	setAngioParameters(mReconstruction, mOutputs.mUsVolume, angio);
	mReconstruction->startReconstruction();
	mReconstruction->getParam("Preset")->setValueFromVariant(preset);
	mReconstruction->getParam("Angio data")->setValueFromVariant(angioData);
	mReconstruction->getParam("Dual Angio")->setValueFromVariant(dualAngio);

	TimedAlgorithmPtr retval;
	std::set<TimedAlgorithmPtr> after = mReconstruction->getThreadedReconstruction();
	for (std::set<TimedAlgorithmPtr>::iterator iter = after.begin(); iter != after.end(); ++iter)
	{
		retval = (*iter && !before.count(*iter)) ? *iter : retval;
	}
	return retval;
}

/** Other reconstructions, e.g. after an acquisition, can finish first. */
void UsReconstructionRunner::usReconstructionFinished()
{
	bool ourFinished = !mUsThread || mUsThread->isFinished();
	if (ourFinished)
	{
		disconnect(mReconstruction.get(), &UsReconstructionService::reconstructFinished, this, &UsReconstructionRunner::usReconstructionFinished);
	}
	if (ourFinished && mOutputs.mAngioMesh && !mCancel)
	{
		mAngioVolumes = this->findNewAngioVolumes();
		this->startNextAngioMesh();
	}
	else if (ourFinished)
	{
		this->startNextExtension();
	}
}

std::vector<ImagePtr> UsReconstructionRunner::findNewAngioVolumes() const
{
	std::vector<ImagePtr> retval;
	std::map<QString, ImagePtr> images = mPatientModel->getDataOfType<Image>();
	for (std::map<QString, ImagePtr>::iterator iter = images.begin(); iter != images.end(); ++iter)
	{
		if (!mDataBefore.contains(iter->first) && iter->second->getImageType() == istANGIO)
		{
			retval.push_back(iter->second);
		}
	}
	return retval;
}

void UsReconstructionRunner::startNextAngioMesh()
{
	if (mAngioVolumes.empty() || mCancel)
	{
		this->startNextExtension();
	}
	else
	{
		emit statusChanged("Creating the angio mesh");
		emit progress(0, 0);
		mMeshBase = mAngioVolumes.back();
		mAngioVolumes.pop_back();
		vtkImageDataPtr data = mMeshBase->getBaseVtkImageData();
		double threshold = mReconstruction->getParam("Angio mesh threshold")->getValueAsVariant().toDouble();
		mMeshWatcher.setFuture(QtConcurrent::run([data, threshold]()
		{
			return ContourFilter::execute(data, threshold);
		}));
	}
}

void UsReconstructionRunner::angioMeshFinished()
{
	vtkPolyDataPtr contour = mMeshWatcher.result();
	if (!mCancel && contour && contour->GetNumberOfPoints() > 0)
	{
		ContourFilter::postProcess(mPatientModel, contour, mMeshBase, QColor(Qt::red));
	}
	else if (!mCancel)
	{
		reportWarning(QString("No angio mesh from %1: no voxels above the threshold").arg(mMeshBase->getName()));
	}
	mMeshBase.reset();
	this->startNextAngioMesh();
}

void UsReconstructionRunner::startNextExtension()
{
	bool started = false;
	while (!started && !mExtensions.empty() && !mCancel)
	{
		ExtensionOutputs next = mExtensions.front();
		mExtensions.erase(mExtensions.begin());
		if (next.mExtension && !next.mOutputs.isEmpty())
		{
			mCurrentExtension = next.mExtension;
			emit progress(0, 0);
			this->connectExtension(mCurrentExtension, true);
			started = mCurrentExtension->start(next.mOutputs);
			if (!started)
			{
				this->connectExtension(mCurrentExtension, false);
				mCurrentExtension.clear();
			}
		}
	}
	if (!started)
	{
		this->finish();
	}
}

void UsReconstructionRunner::connectExtension(UsReconstructionExtensionService* extension, bool on)
{
	if (on)
	{
		connect(extension, &UsReconstructionExtensionService::progress, this, &UsReconstructionRunner::progress);
		connect(extension, &UsReconstructionExtensionService::statusChanged, this, &UsReconstructionRunner::statusChanged);
		connect(extension, &UsReconstructionExtensionService::finished, this, &UsReconstructionRunner::extensionFinished);
		connect(extension, &QObject::destroyed, this, &UsReconstructionRunner::extensionFinished);
	}
	else
	{
		disconnect(extension, NULL, this, NULL);
	}
}

void UsReconstructionRunner::extensionFinished()
{
	if (mCurrentExtension)
	{
		this->connectExtension(mCurrentExtension, false);
	}
	mCurrentExtension.clear();
	this->startNextExtension();
}

void UsReconstructionRunner::finish()
{
	std::map<QString, DataPtr> data = mPatientModel->getDatas();
	for (std::map<QString, DataPtr>::iterator iter = data.begin(); iter != data.end(); ++iter)
	{
		if (!mDataBefore.contains(iter->first))
		{
			mCreatedData << iter->first;
		}
	}
	mRunning = false;
	emit statusChanged(mCancel ? "Cancelled" : QString("Created %1 data").arg(mCreatedData.size()));
	emit finished();
}

std::vector<DataPtr> UsReconstructionRunner::getCreatedData() const
{
	std::vector<DataPtr> retval;
	for (int i = 0; i < mCreatedData.size(); ++i)
	{
		DataPtr data = mPatientModel->getData(mCreatedData[i]);
		if (data)
		{
			retval.push_back(data);
		}
	}
	return retval;
}

} // namespace cx
