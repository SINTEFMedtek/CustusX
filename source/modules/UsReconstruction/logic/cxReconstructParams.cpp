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

#include "cxReconstructParams.h"

#include "cxDataManager.h"
#include "cxStringDataAdapterXml.h"
#include "cxDoubleDataAdapterXml.h"
#include "cxBoolDataAdapterXml.h"
#include "cxTransferFunctions3DPresets.h"
#include "cxDoubleRange.h"

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace cx
{

ReconstructParams::ReconstructParams(XmlOptionFile settings)
{
	mSettings = settings;
	mSettings.getElement("algorithms");

	mOrientationAdapter = StringDataAdapterXml::initialize("Orientation", "",
		"Algorithm to use for output volume orientation", "MiddleFrame",
		QString("PatientReference MiddleFrame").split(" "),
		mSettings.getElement());
	connect(mOrientationAdapter.get(), SIGNAL(valueWasSet(int)), this, SIGNAL(changedInputSettings()));

	PresetTransferFunctions3DPtr presets = dataManager()->getPresetTransferFunctions3D();
	mPresetTFAdapter = StringDataAdapterXml::initialize("Preset", "",
		"Preset transfer function to apply to the reconstructed volume", "US B-Mode", presets->getPresetList("US"),
		mSettings.getElement());

	connect(mPresetTFAdapter.get(), SIGNAL(valueWasSet(int)), this, SIGNAL(transferFunctionChanged()));
	//connect(mPresetTFAdapter.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	mMaskReduce = StringDataAdapterXml::initialize("Reduce mask (% in 1D)", "",
		"Speedup by reducing mask size", "3",
		QString("0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15").split(" "),
		mSettings.getElement());
	connect(mMaskReduce.get(), SIGNAL(valueWasSet(int)), this, SIGNAL(changedInputSettings()));

	mAlignTimestamps = BoolDataAdapterXml::initialize("Align timestamps", "",
		"Align the first of tracker and frame timestamps, ignoring lags.", false,
		mSettings.getElement());
	connect(mAlignTimestamps.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	mTimeCalibration = DoubleDataAdapterXml::initialize("Extra Temporal Calib", "",
		"Set an offset in the frame timestamps, in addition to the one used in acquisition", 0.0,
		DoubleRange(-1000, 1000, 10), 0,
		mSettings.getElement());
	connect(mTimeCalibration.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	double maxVolumeSizeFactor = 1024*1024;
	mMaxVolumeSize = DoubleDataAdapterXml::initialize("Volume Size", "",
		"Output Volume Size (Mb)", 32*maxVolumeSizeFactor,
		DoubleRange(maxVolumeSizeFactor, maxVolumeSizeFactor*500, maxVolumeSizeFactor), 0,
		mSettings.getElement());
	mMaxVolumeSize->setInternal2Display(1.0/maxVolumeSizeFactor);
	connect(mMaxVolumeSize.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	mAngioAdapter = BoolDataAdapterXml::initialize("Angio data", "",
		"Ultrasound angio data is used as input", false,
		mSettings.getElement());
	connect(mAngioAdapter.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	mCreateBModeWhenAngio = BoolDataAdapterXml::initialize("Dual Angio", "",
		"If angio requested, also create a B-mode reconstruction based on the same data set.", true,
		mSettings.getElement());
	connect(mCreateBModeWhenAngio.get(), SIGNAL(valueWasSet()), this, SIGNAL(changedInputSettings()));

	mAlgorithmAdapter = StringDataAdapterXml::initialize("Algorithm", "", "Choose algorithm to use for reconstruction",
			QString(), QStringList(), mSettings.getElement());
	connect(mAlgorithmAdapter.get(), SIGNAL(valueWasSet(int)), this, SIGNAL(changedInputSettings()));
}

ReconstructParams::~ReconstructParams()
{
	mSettings.save();
}



}
