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
	
	vtkUnsignedCharArrayPtr colors = vtkUnsignedCharArrayPtr::New();
	
	colors->SetNumberOfComponents(3);
	int numberOfPolys = polyData->GetNumberOfCells();
	colors->SetNumberOfTuples(numberOfPolys);
	QColor originalColor = mesh->getColor();
	
	double R_mean = originalColor.red();
	double G_mean = originalColor.green();
	double B_mean = originalColor.blue();
	double glabalVariance = this->getGlobalVarianceOption(mOptions)->getValue();
	double LocalVariance = this->getLocalVarianceOption(mOptions)->getValue();

	this->sortPolyData(polyData);

	std::random_device rd{};
	std::mt19937 gen{rd()};
	std::normal_distribution<> R_dist{R_mean, glabalVariance};
	std::normal_distribution<> G_dist{G_mean, glabalVariance};
	std::normal_distribution<> B_dist{B_mean, glabalVariance
	};
	for(int i=0; i<numberOfPolys; i++)
		{
			colors->InsertTuple3(i, std::max(std::min(R_dist(gen),254.999),0.0001), std::max(std::min(G_dist(gen),254.999),0.0001), std::max(std::min(B_dist(gen),254.999),0.0001)); //Make sure 0-255!
		}
	polyData->GetCellData()->SetScalars(colors);

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
//	CX_LOG_DEBUG() << "pointToPolysArray.size(): " << pointToPolysArray.size();
//	for(int i = 0; i < pointToPolysArray.size(); i++)
//	{
//		std::cout << i << ": ";
//		for(int j = 0; j < pointToPolysArray[i].size(); j++)
//			std::cout << pointToPolysArray[i][j] << " ";
//		std::cout << " " << endl;
//	}

}

} // namespace cx
