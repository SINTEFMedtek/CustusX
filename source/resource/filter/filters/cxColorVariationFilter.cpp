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

StringPropertyBasePtr ColorVariationFilter::getStringOption(QDomElement root)
{
	QStringList list;
	list << "String0" << "String1";
	return StringProperty::initialize("String", "", "dummy string option",
												 list[0], list, root);
}

DoublePropertyBasePtr ColorVariationFilter::getDoubleOption(QDomElement root)
{
	return DoubleProperty::initialize("Value", "",
	                                             "dummy double value.", 1, DoubleRange(0.1, 10, 0.01), 2,
												 root);
}

BoolPropertyBasePtr ColorVariationFilter::getBoolOption(QDomElement root)
{
	return BoolProperty::initialize("Bool0", "",
	                                           "Dummy bool value.", false, root);
}

void ColorVariationFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getStringOption(mOptions));
	mOptionsAdapters.push_back(this->getDoubleOption(mOptions));
	mOptionsAdapters.push_back(this->getBoolOption(mOptions));
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
	double R_std = 10.0;
	double G_std = 10.0;
	double B_std = 10.0;
	std::random_device rd{};
	std::mt19937 gen{rd()};
	std::normal_distribution<> R_dist{R_mean, R_std};
	std::normal_distribution<> G_dist{G_mean, G_std};
	std::normal_distribution<> B_dist{B_mean, B_std};
	for(int i=0; i<numberOfPolys; i++)
		{
			colors->InsertTuple3(i, std::max(std::min(R_dist(gen),255.0),0.0), std::max(std::min(G_dist(gen),255.0),0.0), std::max(std::min(B_dist(gen),255.0),0.0)); //Make sure 0-255!
		}
	polyData->GetCellData()->SetScalars(colors);

	mesh->setVtkPolyData(polyData);
	mesh->setUseColorFromPolydataScalars(true);

	mesh->meshChanged();
	patientService()->insertData(mesh);
	return true;
}

bool ColorVariationFilter::postProcess()
{
	if (mInputTypes.front()->getData())
		mOutputTypes.front()->setValue(mInputTypes.front()->getData()->getUid());

	return true;
}



} // namespace cx
