/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <random>
#include <cmath>

#include <vtkCellData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyData.h>

#include "cxColorVariationFilter.h"
#include "cxTypeConversions.h"
#include "cxSelectDataStringProperty.h"
#include "cxData.h"
#include "cxDoubleProperty.h"
#include "cxStringProperty.h"
#include "cxBoolProperty.h"
#include "cxVisServices.h"
#include "cxMesh.h"
#include "vtkForwardDeclarations.h"
#include "cxLogger.h"
#include "cxRegistrationTransform.h"

namespace cx
{

ColorVariationFilter::ColorVariationFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString ColorVariationFilter::getName() const
{
	return "Color variation";
}

QString ColorVariationFilter::getType() const
{
	return "color_variation_filter";
}

QString ColorVariationFilter::getNameSuffixColorVariation()
{
	return "_colored";
}

QString ColorVariationFilter::getHelp() const
{
	return "A filter to create variation of colors to a mesh. \n"
					"The variation is applied as a normal distributin \n"
					"with the original color as mean. A filter \n"
					"is applied for smooth color transitions.";
}

DoublePropertyPtr ColorVariationFilter::getGlobalVarianceOption(QDomElement root)
{
	DoublePropertyPtr retval = DoubleProperty::initialize("Global variance", "",
															"Select the global color variance", 10.0, DoubleRange(1.0, 100.0, 0.5), 1.0, root);
	return retval;
}

DoublePropertyPtr ColorVariationFilter::getLocalVarianceOption(QDomElement root)
{
	DoublePropertyPtr retval = DoubleProperty::initialize("Local variance", "",
															"Select the local color variance", 1.0, DoubleRange(0.1, 10.0, 0.1), 1.0, root);
	return retval;
}

DoublePropertyPtr ColorVariationFilter::getSmoothingOption(QDomElement root)
{
	DoublePropertyPtr retval = DoubleProperty::initialize("Smoothing", "",
															"Select the number of smoothing iterations", 3, DoubleRange(0, 10, 1), 0, root);
	return retval;
}

void ColorVariationFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getGlobalVarianceOption(mOptions));
	mOptionsAdapters.push_back(this->getLocalVarianceOption(mOptions));
	mOptionsAdapters.push_back(this->getSmoothingOption(mOptions));
}

void ColorVariationFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectMesh::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Input mesh to be applied color variation.");
	mInputTypes.push_back(temp);
}

void ColorVariationFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectMesh::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Output mesh");
	mOutputTypes.push_back(temp);
}

bool ColorVariationFilter::execute()
{
	MeshPtr inputMesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mInputTypes[0])->getMesh();
	if (!inputMesh)
		return false;
	
	double globaleVatiance = this->getGlobalVarianceOption(mOptions)->getValue();
	double localeVatiance = this->getLocalVarianceOption(mOptions)->getValue();
	int smoothingIterations = int(this->getSmoothingOption(mOptions)->getValue());
	
	mOutputMesh = this->execute(inputMesh, globaleVatiance, localeVatiance, smoothingIterations);

	if(mOutputTypes.size() > 0)
		mOutputTypes[0]->setValue(mOutputMesh->getUid());
	return true;
}

MeshPtr ColorVariationFilter::execute(MeshPtr inputMesh, double globaleVariance, double localeVariance, int smoothingIterations)
{
	if (!inputMesh)
		return MeshPtr();

	vtkPolyDataPtr polyData = inputMesh->getTransformedPolyDataCopy(inputMesh->get_rMd());
	mGlobalVariance = globaleVariance;
	mLocalVariance = localeVariance;
	
	this->sortPolyData(polyData);
	this->colorPolyData(inputMesh);
	this->smoothColorsInMesh(smoothingIterations);
	polyData->GetCellData()->SetScalars(mColors);

	QString uidColoredModel = inputMesh->getUid() + ColorVariationFilter::getNameSuffixColorVariation();
	QString nameColoredModel = inputMesh->getName() + ColorVariationFilter::getNameSuffixColorVariation();

	MeshPtr outputMesh = patientService()->createSpecificData<Mesh>(uidColoredModel, nameColoredModel);
	outputMesh->setVtkPolyData(polyData);
	outputMesh->setColor(inputMesh->getColor());
	outputMesh->setUseColorFromPolydataScalars(true);
	outputMesh->get_rMd_History()->setParentSpace(inputMesh->getParentSpace());
	patientService()->insertData(outputMesh);

	return outputMesh;
}

MeshPtr ColorVariationFilter::getOutputMesh()
{
	return mOutputMesh;
}

bool ColorVariationFilter::postProcess()
{
	if (mOutputTypes.front()->getData())
		mOutputTypes.front()->setValue(mOutputTypes.front()->getData()->getUid());

	return true;
}

void ColorVariationFilter::sortPolyData(vtkPolyDataPtr polyData)
{
	mPolyToPointsArray.clear();
	mPointToPolysArray.clear();

	vtkIdType numberOfCells = polyData->GetNumberOfCells();

	mPolyToPointsArray = std::vector<std::vector<vtkIdType>>(numberOfCells);

	std::vector<std::vector<vtkIdType>> pointToPolysArray(polyData->GetNumberOfPoints());
	for(vtkIdType i = 0; i < numberOfCells; i++)
	{
		vtkIdListPtr points = polyData->GetCell(i)->GetPointIds();
		vtkIdType numberOfIds = points->GetNumberOfIds();
		std::vector<vtkIdType> pointsArray(numberOfIds);
		for(vtkIdType j = 0; j < numberOfIds; j++)
		{
			vtkIdType p = points->GetId(j);
			pointsArray[j]= p;
			pointToPolysArray[p].resize(pointToPolysArray[p].size()+1, i);
		}
		mPolyToPointsArray[i] = pointsArray;
	}
	mPointToPolysArray = pointToPolysArray;

}

vtkUnsignedCharArrayPtr ColorVariationFilter::colorPolyData(MeshPtr mesh)
{
	if(mPolyToPointsArray.empty() || mPointToPolysArray.empty())
		return mColors;

	vtkPolyDataPtr polyData = mesh->getTransformedPolyDataCopy(Transform3D::Identity());

	mAssignedColorValues.clear();
	mColors = vtkUnsignedCharArrayPtr::New();
	mColors->SetNumberOfComponents(3);
	int numberOfPolys = polyData->GetNumberOfCells();
	mColors->SetNumberOfTuples(numberOfPolys);
	QColor originalColor = mesh->getColor();

	mR_mean = originalColor.red();
	mG_mean = originalColor.green();
	mB_mean = originalColor.blue();
	
	this->generateColorDistribution();

	mAssignedColorValues.clear();
	mAssignedColorValues = std::vector<bool>(numberOfPolys, false);

	for(int i=0; i<numberOfPolys; i++) //Loop needed if multiple independent meshes in model.
		if(!mAssignedColorValues[i])
			for(int j=0; j<mPolyToPointsArray[i].size(); j++)
				this->applyColorToNeighbourPolys(mPolyToPointsArray[i][j], mR_mean, mG_mean, mB_mean);

	return mColors;
}

void ColorVariationFilter::applyColorToNeighbourPolys(int startIndex, double R, double G, double B)
{

	std::vector<vtkIdType> polyIndexColoringQueue = this->applyColorAndFindNeighbours(startIndex, R, G, B);
	std::vector<double> color {R, G, B};
	std::vector<std::vector<double>> polyColorColoringQueue(polyIndexColoringQueue.size(), color);

	while(!polyIndexColoringQueue.empty())
	{
		std::vector<vtkIdType> neighbourPointsList = mPolyToPointsArray[polyIndexColoringQueue[0]]; //Prosess first index in FIFO queue

		for(int i=0; i<neighbourPointsList.size(); i++)
		{
			std::vector<double> newColor = generateColor(polyColorColoringQueue[0][0], polyColorColoringQueue[0][1], polyColorColoringQueue[0][2]);
			std::vector<vtkIdType> polyIndexToColor = this->applyColorAndFindNeighbours(neighbourPointsList[i], newColor[0], newColor[1], newColor[2]);
			polyIndexColoringQueue.insert(polyIndexColoringQueue.end(), polyIndexToColor.begin(), polyIndexToColor.end());
			polyColorColoringQueue.resize(polyColorColoringQueue.size()+polyIndexToColor.size(), newColor);
		}
		polyIndexColoringQueue.erase(polyIndexColoringQueue.begin());
		polyColorColoringQueue.erase(polyColorColoringQueue.begin());
	}
}

std::vector<vtkIdType> ColorVariationFilter::applyColorAndFindNeighbours(int pointIndex, double R, double G, double B)
{
	std::vector<vtkIdType> neighbourPolysList = mPointToPolysArray[pointIndex];
	std::vector<int> removeIndexList;

	for(int i=0; i<neighbourPolysList.size(); i++)
	{
		if(!mAssignedColorValues[neighbourPolysList[i]]) //Check if tuple is already assigned a color
		{
			mColors->InsertTuple3(neighbourPolysList[i], R, G, B);
			mAssignedColorValues[neighbourPolysList[i]] = true;
		}
		else
		{
			removeIndexList.push_back(i); //Remove tuple if already assigned
		}
	}

	int N = removeIndexList.size();
	for(int i=N-1; i>=0; i--) //Removeing neighbour polys which is already assigned a color
		neighbourPolysList.erase(neighbourPolysList.begin() + removeIndexList[i]);

	return neighbourPolysList;
}

void ColorVariationFilter::generateColorDistribution()
{
	mR_dist = std::normal_distribution<> {mR_mean, mGlobalVariance};
	mG_dist = std::normal_distribution<> {mG_mean, mGlobalVariance};
	mB_dist = std::normal_distribution<> {mB_mean, mGlobalVariance};
}

std::vector<double> ColorVariationFilter::generateColor(double R, double G, double B)
{

	std::vector<double> color;
	color.push_back( std::max(std::min( std::max(std::min(mR_dist(m_gen),R+mLocalVariance),R-mLocalVariance) ,254.999),0.0001) );
	color.push_back( std::max(std::min( std::max(std::min(mG_dist(m_gen),G+mLocalVariance),G-mLocalVariance) ,254.999),0.0001) );
	color.push_back( std::max(std::min( std::max(std::min(mB_dist(m_gen),B+mLocalVariance),B-mLocalVariance) ,254.999),0.0001) );

	return color;
}

void ColorVariationFilter::smoothColorsInMesh(int iterations)
{
	for(int itr=0; itr<iterations; itr++)
	{
		vtkUnsignedCharArrayPtr newColors = mColors;
		int numberofPolys = newColors->GetNumberOfTuples();
	
		Eigen::MatrixXd allColors(numberofPolys,3);
		for(int i=0; i<numberofPolys; i++)
		{
			double colorTuple[3];
			mColors->GetTuple(i, colorTuple);
			allColors(i,0) = colorTuple[0];
			allColors(i,1) = colorTuple[1];
			allColors(i,2) = colorTuple[2];
		}
	
		for(int i=0; i<numberofPolys; i++)
		{
			std::vector<vtkIdType> connectedPoints = mPolyToPointsArray[i];
			std::vector<int> neighbourPolys;
			for(int j=0; j<connectedPoints.size(); j++)
			{
				std::vector<vtkIdType> newNeighbours = mPointToPolysArray[connectedPoints[j]];
				neighbourPolys.insert(neighbourPolys.end(), newNeighbours.begin(), newNeighbours.end());
			}
			neighbourPolys.erase( unique( neighbourPolys.begin(), neighbourPolys.end() ), neighbourPolys.end() );// remove duplicates
			std::vector<std::vector<double>> neighbourColors;
			Eigen::Vector3d sumNeighbourColors = Eigen::Vector3d::Zero();
			for(int j=0; j<neighbourPolys.size(); j++)
			{
				sumNeighbourColors(0) += allColors(neighbourPolys[j],0);
				sumNeighbourColors(1) += allColors(neighbourPolys[j],1);
				sumNeighbourColors(2) += allColors(neighbourPolys[j],2);
			}
			
			Eigen::Vector3d color;
			for(int j=0; j<3; j++)
				color(j) = ( sumNeighbourColors(j) + allColors(i,j) ) / (neighbourPolys.size() + 1); //Average of current color and all neighbour colors
			
			newColors->InsertTuple3(i, color[0], color[1], color[2]);
		}
	
		mColors = newColors;
	}
}

} // namespace cx
