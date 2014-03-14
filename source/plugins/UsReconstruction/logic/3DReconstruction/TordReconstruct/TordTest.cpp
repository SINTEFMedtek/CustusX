#include "TordTest.h"

#include <iostream>
#include <vtkImageData.h>
#include <QDomElement>
#include <recConfig.h>
#include <sscUSFrameData.h>
#include "sscMessageManager.h"

namespace cx
{


TordTest::TordTest()
{
	mAlgorithm = TordAlgorithmPtr(new TordAlgorithm);

	mMethods.push_back("VNN");
	mMethods.push_back("VNN2");
	mMethods.push_back("DW");
	mMethods.push_back("Anisotropic");
	mPlaneMethods.push_back("Heuristic");
	mPlaneMethods.push_back("Closest");
}

TordTest::~TordTest()
{

}

void TordTest::enableProfiling()
{
	mAlgorithm->setProfiling(true);
}

double TordTest::getKernelExecutionTime()
{
	mAlgorithm->getKernelExecutionTime();
}

QString TordTest::getName() const
{
	return "TordTest";
}

std::vector<DataAdapterPtr> TordTest::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;

	retval.push_back(this->getMethodOption(root));
	retval.push_back(this->getRadiusOption(root));
	retval.push_back(this->getPlaneMethodOption(root));
	retval.push_back(this->getMaxPlanesOption(root));
	retval.push_back(this->getNStartsOption(root));
	retval.push_back(this->getNewnessWeightOption(root));
	retval.push_back(this->getBrightnessWeightOption(root));
	return retval;
}

bool TordTest::reconstruct(ProcessedUSInputDataPtr input, vtkImageDataPtr outputData, QDomElement settings)
{

	int nClosePlanes = getMaxPlanesOption(settings)->getValue();

	int method = getMethodID(settings);
	float radius = getRadiusOption(settings)->getValue();
	int planeMethod = getPlaneMethodID(settings);
	int nStarts = getNStartsOption(settings)->getValue();
	float newnessWeight = getNewnessWeightOption(settings)->getValue();
	float brightnessWeight = getBrightnessWeightOption(settings)->getValue();

	messageManager()->sendInfo(
			QString("Method: %1, radius: %2, planeMethod: %3, nClosePlanes: %4, nPlanes: %5, nStarts: %6 ").arg(method).arg(
					radius).arg(planeMethod).arg(nClosePlanes).arg(input->getDimensions()[2]).arg(nStarts));

	if (!mAlgorithm->initCL(QString(TORD_KERNEL_PATH) + "/kernels.cl", nClosePlanes, input->getDimensions()[2], method, planeMethod, nStarts, newnessWeight, brightnessWeight))
		return false;

	bool ret = mAlgorithm->reconstruct(input, outputData, radius, nClosePlanes);

	return ret;
}

StringDataAdapterXmlPtr TordTest::getMethodOption(QDomElement root)
{
	QStringList methods;
	for (std::vector<QString>::iterator it = mMethods.begin(); it != mMethods.end(); ++it)
	{
		QString method = *it;
		methods << method;
	}
	return StringDataAdapterXml::initialize("Method", "", "Which algorithm to use for reconstruction", methods[0],
			methods, root);
}

DoubleDataAdapterXmlPtr TordTest::getNewnessWeightOption(QDomElement root)
{
	return DoubleDataAdapterXml::initialize("Newness weight", "", "Newness weight", 0, DoubleRange(0.0, 10, 0.1), 1,
			root);
}

DoubleDataAdapterXmlPtr TordTest::getBrightnessWeightOption(QDomElement root)
{
	return DoubleDataAdapterXml::initialize("Brightness weight", "", "Brightness weight", 0, DoubleRange(0.0, 10, 0.1),
			1, root);
}

StringDataAdapterXmlPtr TordTest::getPlaneMethodOption(QDomElement root)
{
	QStringList methods;
	for (std::vector<QString>::iterator it = mPlaneMethods.begin(); it != mPlaneMethods.end(); ++it)
	{
		QString method = *it;
		methods << method;
	}
	return StringDataAdapterXml::initialize("Plane method", "", "Which method to use for finding close planes",
			methods[0], methods, root);
}

DoubleDataAdapterXmlPtr TordTest::getRadiusOption(QDomElement root)
{
	return DoubleDataAdapterXml::initialize("Radius (mm)", "", "Radius of kernel. mm.", 1, DoubleRange(0.1, 10, 0.1), 1,
			root);
}

DoubleDataAdapterXmlPtr TordTest::getMaxPlanesOption(QDomElement root)
{
	return DoubleDataAdapterXml::initialize("nPlanes", "", "Number of planes to include in closest planes", 8,
			DoubleRange(1, 200, 1), 0, root);
}

DoubleDataAdapterXmlPtr TordTest::getNStartsOption(QDomElement root)
{
	return DoubleDataAdapterXml::initialize("nStarts", "", "Number of starts for multistart searchs", 3,
			DoubleRange(1, 16, 1), 0, root);
}

int TordTest::getMethodID(QDomElement root)
{
	return find(mMethods.begin(), mMethods.end(), this->getMethodOption(root)->getValue()) - mMethods.begin();
}

int TordTest::getPlaneMethodID(QDomElement root)
{
	return find(mPlaneMethods.begin(), mPlaneMethods.end(), this->getPlaneMethodOption(root)->getValue())
			- mPlaneMethods.begin();
}

}

