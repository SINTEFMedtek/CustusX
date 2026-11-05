/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxBoolProperty.h"
#include "catch.hpp"
#include "cxtestReconstructionManagerFixture.h"
#include "cxtestReconstructRealData.h"
#include "cxtestSyntheticReconstructInput.h"
#include "cxDummyTool.h"
#include "cxReconstructPreprocessor.h"
#include <vtkImageData.h>
#include "cxStringProperty.h"
#include "cxReconstructionExecuter.h"
#include "cxDataLocations.h"

//#include "cxViewService.h"
#include "cxPatientModelService.h"
#include "cxMessageListener.h"


namespace cxtest
{


TEST_CASE("ReconstructManager: PNN on sphere","[unit][usreconstruction][synthetic][not_win32][pnn]")
{
	ReconstructionManagerTestFixture fixture;
	cx::MessageListenerPtr messageListener = cx::MessageListener::createWithQueue();
	fixture.setVerbose(true);

	SyntheticReconstructInputPtr input(new SyntheticReconstructInput);
	input->setOverallBoundsAndSpacing(100, 5);
	input->setSpherePhantom();
	cx::USReconstructInputData inputData = input->generateSynthetic_USReconstructInputData();

	cx::UsReconstructionServicePtr reconstructer = fixture.getManager();
	reconstructer->selectData(inputData);
	reconstructer->getParam("Algorithm")->setValueFromVariant("pnn");
	reconstructer->getParam("Dual Angio")->setValueFromVariant(false);
	reconstructer->getParam("Position Filter Strength")->setValueFromVariant("0");
	// set an algorithm-specific parameter
	fixture.setPNN_InterpolationSteps(1);

	// run the reconstruction in the main thread
	fixture.reconstruct();

	// check validity of output:
	REQUIRE(fixture.getOutput().size()==1);

	SyntheticVolumeComparerPtr comparer = fixture.getComparerForOutput(input, 0);
	comparer->checkRMSBelow(30.0);
	comparer->checkCentroidDifferenceBelow(1);
	comparer->checkMassDifferenceBelow(0.01);
	// check the value in the sphere center:
	comparer->checkValueWithin(input->getPhantom()->getBounds()/2, 200, 255);

	if (comparer->getVerbose())
	{
		comparer->saveOutputToFile("sphere_recman.mhd", fixture.getFileManagerService());
		comparer->saveNominalOutputToFile("sphere_nomman.mhd", fixture.getFileManagerService());
	}

	CHECK(!messageListener->containsErrors());
}

TEST_CASE("ReconstructManager: PNN on angio sphere","[unit][usreconstruction][synthetic][pnn][hide]")
{
	/** Test on a phantom containing a colored sphere and a gray sphere.
	  * Verify that the angio algo reconstructs only the colored, and the
	  * BMode reconstructs only the gray.
	  *
	  */
	ReconstructionManagerTestFixture fixture;
	fixture.setVerbose(false);

	SyntheticReconstructInputPtr input(new SyntheticReconstructInput);
	input->setOverallBoundsAndSpacing(100, 5);
	input->setSpherePhantom();
	cx::USReconstructInputData inputData = input->generateSynthetic_USReconstructInputData();

	cx::UsReconstructionServicePtr reconstructer = fixture.getManager();
	reconstructer->selectData(inputData);
	reconstructer->getParam("Algorithm")->setValueFromVariant("pnn");//default
	reconstructer->getParam("Angio data")->setValueFromVariant(true);
	reconstructer->getParam("Dual Angio")->setValueFromVariant(false);
	// set an algorithm-specific parameter
	fixture.setPNN_InterpolationSteps(1);

	// run the reconstruction in the main thread
	fixture.reconstruct();

	// check validity of output:
	REQUIRE(fixture.getOutput().size()==1);

	SyntheticVolumeComparerPtr comparer = fixture.getComparerForOutput(input, 0);
	comparer->checkRMSBelow(30.0);
	comparer->checkCentroidDifferenceBelow(1);
	comparer->checkMassDifferenceBelow(0.01);
	// check the value in the sphere center:
	comparer->checkValueWithin(input->getPhantom()->getBounds()/2, 200, 255);

	if (comparer->getVerbose())
	{
		comparer->saveOutputToFile("sphere_recman.mhd", fixture.getFileManagerService());
		comparer->saveNominalOutputToFile("sphere_nomman.mhd", fixture.getFileManagerService());
	}
}


TEST_CASE("ReconstructManager: PNN on empty data","[unit][usreconstruction][synthetic][not_win32][pnn]")
{
	ReconstructionManagerTestFixture fixture;
	cx::MessageListenerPtr messageListener = cx::MessageListener::createWithQueue();
	fixture.setVerbose(true);

	SyntheticReconstructInputPtr input(new SyntheticReconstructInput);
	input->setOverallBoundsAndSpacing(1, 10);//A small size number may cause reconstruction to fail
	input->setSpherePhantom();
	cx::USReconstructInputData inputData = input->generateSynthetic_USReconstructInputData();

	cx::UsReconstructionServicePtr reconstructer = fixture.getManager();
	reconstructer->selectData(inputData);
	reconstructer->getParam("Algorithm")->setValueFromVariant("pnn");
	reconstructer->getParam("Dual Angio")->setValueFromVariant(false);
	reconstructer->getParam("Position Filter Strength")->setValueFromVariant("0");
	// set an algorithm-specific parameter
	fixture.setPNN_InterpolationSteps(1);

	// run the reconstruction in the main thread. Expect the reconstruction to fail, but prevent seg. fault.
	fixture.reconstruct(false);

	CHECK(!messageListener->containsErrors());
}

TEST_CASE("ReconstructManager: Angio Reconstruction on real data", "[usreconstruction][integration][not_win32]")
{
	ReconstructionManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::UsReconstructionServicePtr reconstructer = fixture.getManager();

	reconstructer->selectData(realData.getSourceFilename());

	reconstructer->getParam("Algorithm")->setValueFromVariant("pnn");//default
	reconstructer->getParam("Angio data")->setValueFromVariant(true);
	reconstructer->getParam("Dual Angio")->setValueFromVariant(false);
	reconstructer->getParam("Position Filter Strength")->setValueFromVariant("0");
	// set an algorithm-specific parameter
	fixture.setPNN_InterpolationSteps(1);

	// run the reconstruction in the main thread
	fixture.reconstruct();
	// check validity of output:
	REQUIRE(fixture.getOutput().size()==1);
	realData.validateAngioData(fixture.getOutput()[0]);
}

TEST_CASE("ReconstructManager: Threaded Dual Angio on real data", "[usreconstruction][integration][not_win32]")
{
	ReconstructionManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::UsReconstructionServicePtr reconstructer = fixture.getManager();

	reconstructer->selectData(realData.getSourceFilename());

	reconstructer->getParam("Algorithm")->setValueFromVariant("pnn");//default
	reconstructer->getParam("Angio data")->setValueFromVariant(true);
	reconstructer->getParam("Dual Angio")->setValueFromVariant(true);
	reconstructer->getParam("Position Filter Strength")->setValueFromVariant("0");

	// set an algorithm-specific parameter
	fixture.setPNN_InterpolationSteps(1);

	// run threaded reconstruction
	fixture.threadedReconstruct();
	// validate output
	REQUIRE(fixture.getOutput().size()==2);
	realData.validateBModeData(fixture.getOutput()[0]);
	realData.validateAngioData(fixture.getOutput()[1]);

}

namespace
{
/** Asks for a time lag estimate, and returns one that is not to be applied */
class NotAppliedTimeLagAlgorithm : public cx::ReconstructionMethodService
{
public:
	virtual QString getName() const { return "NotAppliedTimeLag"; }
	virtual std::vector<cx::PropertyPtr> getSettings(QDomElement root) { Q_UNUSED(root); return std::vector<cx::PropertyPtr>(); }
	virtual bool reconstruct(cx::ProcessedUSInputDataPtr input, vtkImageDataPtr outputData, QDomElement settings)
	{
		Q_UNUSED(input); Q_UNUSED(outputData); Q_UNUSED(settings);
		return false;
	}
	virtual bool isTimeLagEstimateRequested(QDomElement settings) { Q_UNUSED(settings); return true; }
	virtual cx::TimeLagEstimate estimateTimeLag(cx::ProcessedUSInputDataPtr input, QDomElement settings)
	{
		Q_UNUSED(settings);
		for (int i = 0; i < input->getDimensions()[2]; ++i)
		{
			mEstimateFrames.push_back(input->getFrame(i));
		}
		cx::TimeLagEstimate retval;
		retval.valid = true;
		retval.apply = false;
		return retval;
	}
	std::vector<unsigned char*> mEstimateFrames; ///< the image data of the frames given to the estimate
};
}

TEST_CASE("ReconstructPreprocessor: images and positions match after a time lag estimate that is not applied", "[unit][usreconstruction][synthetic]")
{
	SyntheticReconstructInputPtr generator(new SyntheticReconstructInput);
	generator->defineProbe(cx::DummyToolTestUtilities::createProbeDefinitionLinear(100, 100, Eigen::Array2i(150,150)));
	generator->setSpherePhantom();
	cx::USReconstructInputData inputData = generator->generateSynthetic_USReconstructInputData();
	unsigned recordedFrames = inputData.mFrames.size();

	// Removes the frames without tracking data with the user settings, but not when estimating the lag
	cx::ReconstructCore::InputParams par;
	par.mExtraTimeCalibration = 300;
	cx::ReconstructPreprocessorPtr preprocessor(new cx::ReconstructPreprocessor(cx::PatientModelService::getNullObject()));
	preprocessor->initialize(par, inputData);
	NotAppliedTimeLagAlgorithm algorithm;
	CHECK_FALSE(preprocessor->estimateAndApplyTimeLag(&algorithm));

	std::vector<cx::ProcessedUSInputDataPtr> processedInput = preprocessor->createProcessedInput(std::vector<bool>(1, false));
	REQUIRE(processedInput.size() == 1);
	unsigned frames = processedInput[0]->getFrames().size();
	CHECK(frames < recordedFrames);
	CHECK(processedInput[0]->getDimensions()[2] == int(frames));
}

TEST_CASE("ReconstructPreprocessor: the reconstruction reuses the frames converted for the time lag estimate", "[unit][usreconstruction][synthetic]")
{
	SyntheticReconstructInputPtr generator(new SyntheticReconstructInput);
	generator->defineProbe(cx::DummyToolTestUtilities::createProbeDefinitionLinear(100, 100, Eigen::Array2i(150,150)));
	generator->setSpherePhantom();
	cx::USReconstructInputData inputData = generator->generateSynthetic_USReconstructInputData();

	cx::ReconstructCore::InputParams par;
	cx::ReconstructPreprocessorPtr preprocessor(new cx::ReconstructPreprocessor(cx::PatientModelService::getNullObject()));
	preprocessor->initialize(par, inputData);
	NotAppliedTimeLagAlgorithm algorithm;
	preprocessor->estimateAndApplyTimeLag(&algorithm);

	std::vector<cx::ProcessedUSInputDataPtr> processedInput = preprocessor->createProcessedInput(std::vector<bool>(1, false));
	REQUIRE(processedInput.size() == 1);
	unsigned frames = processedInput[0]->getDimensions()[2];
	REQUIRE(frames == algorithm.mEstimateFrames.size());
	for (unsigned i = 0; i < frames; ++i)
	{
		CHECK(processedInput[0]->getFrame(i) == algorithm.mEstimateFrames[i]);
	}

	// The kept frames are released after use, so a new call converts the frames again
	std::vector<cx::ProcessedUSInputDataPtr> newInput = preprocessor->createProcessedInput(std::vector<bool>(1, false));
	CHECK(newInput[0]->getFrame(0) != algorithm.mEstimateFrames[0]);
}

TEST_CASE("ReconstructPreprocessor: flipping the images mirrors the frames and the mask laterally", "[unit][usreconstruction][synthetic]")
{
	SyntheticReconstructInputPtr generator(new SyntheticReconstructInput);
	generator->defineProbe(cx::DummyToolTestUtilities::createProbeDefinitionLinear(100, 100, Eigen::Array2i(150,150)));
	generator->setSpherePhantom();
	cx::USReconstructInputData inputData = generator->generateSynthetic_USReconstructInputData();

	cx::ReconstructCore::InputParams normalParams;
	cx::ReconstructPreprocessorPtr normalPreprocessor(new cx::ReconstructPreprocessor(cx::PatientModelService::getNullObject()));
	normalPreprocessor->initialize(normalParams, inputData);
	std::vector<cx::ProcessedUSInputDataPtr> normalInput = normalPreprocessor->createProcessedInput(std::vector<bool>(1, false));

	cx::ReconstructCore::InputParams flipParams;
	flipParams.mFlipImages = true;
	cx::ReconstructPreprocessorPtr flipPreprocessor(new cx::ReconstructPreprocessor(cx::PatientModelService::getNullObject()));
	flipPreprocessor->initialize(flipParams, inputData);
	std::vector<cx::ProcessedUSInputDataPtr> flipInput = flipPreprocessor->createProcessedInput(std::vector<bool>(1, false));

	REQUIRE(normalInput.size() == 1);
	REQUIRE(flipInput.size() == 1);
	Eigen::Array3i dims = normalInput[0]->getDimensions();
	REQUIRE(flipInput[0]->getDimensions().isApprox(dims));

	// The processed input tells whether the images were flipped (used for the image orientation check in VIC)
	CHECK_FALSE(normalInput[0]->getImagesFlipped());
	CHECK(flipInput[0]->getImagesFlipped());

	// All pixels are mirrored (x -> width-1-x), the rows are unchanged
	unsigned nMismatch = 0;
	for (int frame = 0; frame < dims[2]; ++frame)
	{
		const unsigned char* normal = normalInput[0]->getFrame(frame);
		const unsigned char* flipped = flipInput[0]->getFrame(frame);
		for (int y = 0; y < dims[1]; ++y)
			for (int x = 0; x < dims[0]; ++x)
				if (flipped[x + y*dims[0]] != normal[(dims[0]-1-x) + y*dims[0]])
					++nMismatch;
	}
	CHECK(nMismatch == 0);

	// The mask follows the images, as it is made from the mirrored probe definition
	const unsigned char* normalMask = static_cast<unsigned char*>(normalInput[0]->getMask()->GetScalarPointer());
	const unsigned char* flipMask = static_cast<unsigned char*>(flipInput[0]->getMask()->GetScalarPointer());
	int maskWidth = normalInput[0]->getMask()->GetDimensions()[0];
	int maskHeight = normalInput[0]->getMask()->GetDimensions()[1];
	unsigned nMaskMismatch = 0;
	for (int y = 0; y < maskHeight; ++y)
		for (int x = 0; x < maskWidth; ++x)
			if (flipMask[x + y*maskWidth] != normalMask[(maskWidth-1-x) + y*maskWidth])
				++nMaskMismatch;
	CHECK(nMaskMismatch == 0);
}

TEST_CASE("ReconstructManager: Flip images L/R is kept for the same recording, and turned off for another", "[unit][usreconstruction][synthetic]")
{
	ReconstructionManagerTestFixture fixture;
	SyntheticReconstructInputPtr generator(new SyntheticReconstructInput);
	generator->setOverallBoundsAndSpacing(100, 5);
	generator->setSpherePhantom();
	cx::USReconstructInputData inputData = generator->generateSynthetic_USReconstructInputData();
	inputData.mFilename = "recording1";

	cx::UsReconstructionServicePtr reconstructer = fixture.getManager();
	reconstructer->selectData(inputData);
	cx::PropertyPtr flip = reconstructer->getParam("Flip images");
	REQUIRE(flip);
	flip->setValueFromVariant(true);
	CHECK(reconstructer->createCoreParameters().mFlipImages);

	reconstructer->selectData(inputData);
	CHECK(flip->getValueAsVariant().toBool());

	inputData.mFilename = "recording2";
	reconstructer->selectData(inputData);
	CHECK_FALSE(flip->getValueAsVariant().toBool());
	CHECK_FALSE(reconstructer->createCoreParameters().mFlipImages);
	CHECK(reconstructer->getSelectedFileData().mFilename == "recording2");
	CHECK(reconstructer->getOutputVolumeParams().isValid());
}

TEST_CASE("ReconstructManager: Preprocessor handles too large clip rect","[integration][usreconstruction][synthetic][not_win32]")
{
	ReconstructionManagerTestFixture fixture;
	fixture.setVerbose(false);

	SyntheticReconstructInputPtr generator(new SyntheticReconstructInput);
	Eigen::Array2i frameSize = Eigen::Array2i(150,150);
	Eigen::Array2i extent = frameSize - 1;
	cx::ProbeDefinition probeDefinition = cx::DummyToolTestUtilities::createProbeDefinitionLinear(100, 100, frameSize);

	//Adding 2 sections creates 3 runs: 1 with the simple case clip rect == extent, the other 2 with extent+1 and +500
	SECTION("Set clip rect just to large")
		probeDefinition.setClipRect_p(cx::DoubleBoundingBox3D(0, extent[0]+1, 0, extent[1]+1, 0, 0));
    SECTION("Set clip rect very large")
		probeDefinition.setClipRect_p(cx::DoubleBoundingBox3D(0, extent[0]+500, 0, extent[1]+500, 0, 0));

	generator->defineProbe(probeDefinition);
	generator->setSpherePhantom();

	cx::ReconstructPreprocessorPtr preprocessor(new cx::ReconstructPreprocessor(cx::PatientModelService::getNullObject()));
	cx::USReconstructInputData inputData = generator->generateSynthetic_USReconstructInputData();
	cx::ReconstructCore::InputParams par;
	preprocessor->initialize(par, inputData);

	std::vector<bool> angio(1, false);
	std::vector<cx::ProcessedUSInputDataPtr> processedInput = preprocessor->createProcessedInput(angio);
	REQUIRE(processedInput.size() == 1);

	{
		Eigen::Array3i dimFirstFrame = processedInput[0]->getDimensions();
		dimFirstFrame[2] = 1;
		INFO("Clip prect: " << probeDefinition.getClipRect_p());
		INFO(dimFirstFrame << " == " << Eigen::Array3i(processedInput[0]->getMask()->GetDimensions()));
		REQUIRE(dimFirstFrame.isApprox(Eigen::Array3i(processedInput[0]->getMask()->GetDimensions())));
	}
}

} // namespace cxtest


