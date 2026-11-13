/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxMaterialOverridePass.h"

#include <vtkObjectFactory.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkProp.h>
#include <vtkRenderState.h>

namespace cx
{

vtkStandardNewMacro(MaterialOverridePass);

MaterialOverridePass::MaterialOverridePass()
{
	mMaterial = {0.2, 0.9, 0.3, 15};
}

MaterialOverridePass::~MaterialOverridePass()
{
}

bool MaterialOverridePass::Material::operator==(const Material& other) const
{
	return ambient == other.ambient && diffuse == other.diffuse
			&& specular == other.specular && specularPower == other.specularPower;
}

void MaterialOverridePass::SetDelegatePass(vtkRenderPass* delegatePass)
{
	mDelegatePass = delegatePass;
	this->Modified();
}

void MaterialOverridePass::setMaterial(double ambient, double diffuse, double specular, double specularPower)
{
	mMaterial = {ambient, diffuse, specular, specularPower};
	this->Modified();
}

MaterialOverridePass::Material MaterialOverridePass::getMaterial(vtkProperty* property)
{
	return {property->GetAmbient(), property->GetDiffuse(), property->GetSpecular(), property->GetSpecularPower()};
}

void MaterialOverridePass::applyMaterial(vtkProperty* property, Material material)
{
	// The vtk setters only modify the property if the value changes
	property->SetAmbient(material.ambient);
	property->SetDiffuse(material.diffuse);
	property->SetSpecular(material.specular);
	property->SetSpecularPower(material.specularPower);
}

void MaterialOverridePass::overrideMaterial(vtkProperty* property)
{
	Material current = getMaterial(property);
	if (!(current == mMaterial))
	{
		std::map<vtkProperty*, OriginalMaterial>::iterator iter = mOriginalMaterials.find(property);
		if (iter == mOriginalMaterials.end() || iter->second.property != property)
		{
			mOriginalMaterials[property] = {property, current};
		}
		else
		{
			this->updateOriginalMaterial(iter->second.material, current);
		}
		applyMaterial(property, mMaterial);
	}
}

/** The owner has reset some of the values: these are the new originals. A reset to the override value is not noticed. */
void MaterialOverridePass::updateOriginalMaterial(Material& original, const Material& current) const
{
	if (current.ambient != mMaterial.ambient)
	{
		original.ambient = current.ambient;
	}
	if (current.diffuse != mMaterial.diffuse)
	{
		original.diffuse = current.diffuse;
	}
	if (current.specular != mMaterial.specular)
	{
		original.specular = current.specular;
	}
	if (current.specularPower != mMaterial.specularPower)
	{
		original.specularPower = current.specularPower;
	}
}

void MaterialOverridePass::restoreMaterials()
{
	for (std::map<vtkProperty*, OriginalMaterial>::iterator iter = mOriginalMaterials.begin(); iter != mOriginalMaterials.end(); ++iter)
	{
		vtkProperty* property = iter->second.property;
		if (property && getMaterial(property) == mMaterial)
		{
			applyMaterial(property, iter->second.material);
		}
	}
	mOriginalMaterials.clear();
}

void MaterialOverridePass::Render(const vtkRenderState* s)
{
	this->NumberOfRenderedProps = 0;

	for (int i = 0; i < s->GetPropArrayCount(); ++i)
	{
		vtkActor* actor = vtkActor::SafeDownCast(s->GetPropArray()[i]);
		if (actor && actor->GetProperty() && actor->GetProperty()->GetLighting())
		{
			this->overrideMaterial(actor->GetProperty());
		}
	}

	if (mDelegatePass)
	{
		mDelegatePass->Render(s);
		this->NumberOfRenderedProps = mDelegatePass->GetNumberOfRenderedProps();
	}
}

void MaterialOverridePass::ReleaseGraphicsResources(vtkWindow* w)
{
	if (mDelegatePass)
	{
		mDelegatePass->ReleaseGraphicsResources(w);
	}
}

} //namespace cx
