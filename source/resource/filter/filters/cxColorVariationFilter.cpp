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

QString ColorVariationFilter::getHelp() const
{
	return "A filter to create variation of colors to a mesh.\n"
	        "The variation is be applied as a normal distributin\n"
	        "with the original color as mean.";
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

void ColorVariationFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getGlobalVarianceOption(mOptions));
	mOptionsAdapters.push_back(this->getLocalVarianceOption(mOptions));
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
	temp->setHelp("Dummy output from the dummy algorithm");
	mOutputTypes.push_back(temp);
}

bool ColorVariationFilter::execute()
{
	MeshPtr mesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mInputTypes[0])->getMesh();
	if (!mesh)
		return false;
	
	vtkPolyDataPtr polyData = mesh->getTransformedPolyDataCopy(Transform3D::Identity());
	
	this->sortPolyData(polyData);
	this->colorPolyData(mesh);

	polyData->GetCellData()->SetScalars(mColors);

	mesh->setVtkPolyData(polyData);
	mesh->setUseColorFromPolydataScalars(true);

	//mesh->meshChanged();
	patientService()->removeData(mesh->getUid());
	patientService()->insertData(mesh);
	return true;
}

bool ColorVariationFilter::postProcess()
{
	if (mInputTypes.front()->getData())
		mOutputTypes.front()->setValue(mInputTypes.front()->getData()->getUid());

	return true;
}

void ColorVariationFilter::sortPolyData(vtkPolyDataPtr polyData)
{
	mPolyToPointsArray.clear();
	mPointToPolysArray.clear();

	int numberOfCells = polyData->GetNumberOfCells();
//	CX_LOG_DEBUG() << "numberOfCells: " << numberOfCells;
	std::vector<std::vector<int>> pointToPolysArray(polyData->GetNumberOfPoints());
	for(vtkIdType i = 0; i < numberOfCells; i++)
	{
		vtkIdListPtr points = polyData->GetCell(i)->GetPointIds();
		int numberOfIds = points->GetNumberOfIds();
		//CX_LOG_DEBUG() << "numberOfIds: " << numberOfIds;
		std::vector<int> pointsArray;
		for(vtkIdType j = 0; j < numberOfIds; j++)
		{
			int p = points->GetId(j);
			pointsArray.push_back(p);
			pointToPolysArray[p].push_back(i);
		}
		mPolyToPointsArray.push_back(pointsArray);
	}
	mPointToPolysArray = pointToPolysArray;
		//debug
	CX_LOG_DEBUG() << "mPointToPolysArray.size(): " << mPointToPolysArray.size();
	for(int i = 0; i < mPointToPolysArray.size(); i++)
	{
		std::cout << i << ": ";
		for(int j = 0; j < mPointToPolysArray[i].size(); j++)
			std::cout << mPointToPolysArray[i][j] << " ";
		std::cout << " " << endl;
	}
	CX_LOG_DEBUG() << "mPolyToPointsArray.size(): " << mPolyToPointsArray.size();
	for(int i = 0; i < mPolyToPointsArray.size(); i++)
	{
		std::cout << i << ": ";
		for(int j = 0; j < mPolyToPointsArray[i].size(); j++)
			std::cout << mPolyToPointsArray[i][j] << " ";
		std::cout << " " << endl;
	}

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
	mGlobalVariance = this->getGlobalVarianceOption(mOptions)->getValue();
	mLocalVariance = this->getLocalVarianceOption(mOptions)->getValue();

	mAssignedColorValues.clear();
	mAssignedColorValues = std::vector<bool>(numberOfPolys, false);

	this->applyColorToNeighbourPolys(0, mR_mean, mG_mean, mB_mean);
	for(int i=0; i<numberOfPolys; i++)
		if(!mAssignedColorValues[i])
			for(int j=0; j<mPolyToPointsArray[i].size(); j++)
				this->applyColorToNeighbourPolys(mPolyToPointsArray[i][j], mR_mean, mG_mean, mB_mean);

	return mColors;
}

void ColorVariationFilter::applyColorToNeighbourPolys(int pointIndex, double R, double G, double B)
{

	std::vector<int> neighbourPolysList = this->applyColorAndFindNeighbours(pointIndex, R, G, B);

	for(int i=0; i<neighbourPolysList.size(); i++)
	{
		std::vector<int> neighbourPointsList = mPolyToPointsArray[neighbourPolysList[i]];
		//CX_LOG_DEBUG() << "Polygon nr: " << neighbourPolysList[i];
		for(int j=0; j<neighbourPointsList.size(); j++)
		{
			//CX_LOG_DEBUG() << "Point nr: " << neighbourPointsList[j];
			std::vector<double> newColor = generateColor(R, G, B);
			this->applyColorToNeighbourPolys(neighbourPointsList[j], newColor[0], newColor[1], newColor[2]);
		}
	}
}

std::vector<int> ColorVariationFilter::applyColorAndFindNeighbours(int pointIndex, double R, double G, double B)
{
	CX_LOG_DEBUG() << "Point index: " << pointIndex;
	std::vector<int> neighbourPolysList = mPointToPolysArray[pointIndex];
	std::vector<int> removeIndexList;
	//CX_LOG_DEBUG() << "Color: " << std::to_string(R) << " - " << std::to_string(G) << " - " << std::to_string(B);
	for(int i=0; i<neighbourPolysList.size(); i++)
	{
		if(!mAssignedColorValues[neighbourPolysList[i]]) //Check if tuple is already assigned a color
		{
			mColors->InsertTuple3(neighbourPolysList[i], R, G, B);
			mAssignedColorValues[neighbourPolysList[i]] = true;
			//CX_LOG_DEBUG() << "Assigning color to neighbour: " << neighbourPolysList[i];
		}
		else
		{
			removeIndexList.push_back(i);
			//CX_LOG_DEBUG() << "Tuple already assigned color, removing index from list: " << neighbourPolysList[i];
		}
	}

	//CX_LOG_DEBUG() << "neighbourPolysList.size(): " << neighbourPolysList.size();
	int N = removeIndexList.size();
	for(int i=N-1; i>=0; i--) //Removeing neighbour polys which is already assigned a color
		neighbourPolysList.erase(neighbourPolysList.begin() + removeIndexList[i]);

	CX_LOG_DEBUG() << "neighbourPolysList.size(): " << neighbourPolysList.size();

	return neighbourPolysList;
}

std::vector<double> ColorVariationFilter::generateColor(double R, double G, double B)
{
	std::random_device rd{};
	std::mt19937 gen{rd()};

	std::normal_distribution<> R_dist{mR_mean, mGlobalVariance};
	std::normal_distribution<> G_dist{mG_mean, mGlobalVariance};
	std::normal_distribution<> B_dist{mB_mean, mGlobalVariance};

	std::vector<double> color;
	color.push_back( std::max(std::min( std::max(std::min(R_dist(gen),R+mLocalVariance),R-mLocalVariance) ,254.999),0.0001) );
	color.push_back( std::max(std::min( std::max(std::min(G_dist(gen),G+mLocalVariance),G-mLocalVariance) ,254.999),0.0001) );
	color.push_back( std::max(std::min( std::max(std::min(B_dist(gen),B+mLocalVariance),B-mLocalVariance) ,254.999),0.0001) );

	return color;
}

} // namespace cx
