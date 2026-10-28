/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXMATERIALOVERRIDEPASS_H
#define CXMATERIALOVERRIDEPASS_H

#include "org_custusx_core_view_Export.h"
#include <map>
#include <vtkRenderPass.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>

class vtkProperty;

namespace cx
{

/**
 * \file
 * \addtogroup org_custusx_core_view
 * @{
 */

/**
 * Render pass that sets the same surface material (ambient, diffuse, specular)
 * on all lit actors in the renderer, then renders using the delegate pass.
 *
 * Used to give the meshes in one view a different material than in other views,
 * as each view has its own actors. The material is applied before each render, as
 * the reps may reset it. The original material is kept, and can be restored.
 */
class org_custusx_core_view_EXPORT MaterialOverridePass : public vtkRenderPass
{
public:
	static MaterialOverridePass* New();
	vtkTypeMacro(MaterialOverridePass, vtkRenderPass);

	void Render(const vtkRenderState* s) override;
	void ReleaseGraphicsResources(vtkWindow* w) override;

	void SetDelegatePass(vtkRenderPass* delegatePass);

	void setMaterial(double ambient, double diffuse, double specular, double specularPower);
	/** Set the original material back on all actors changed by this pass. */
	void restoreMaterials();

protected:
	MaterialOverridePass();
	~MaterialOverridePass() override;

private:
	MaterialOverridePass(const MaterialOverridePass&) = delete;
	void operator=(const MaterialOverridePass&) = delete;

	struct Material
	{
		double ambient;
		double diffuse;
		double specular;
		double specularPower;
		bool operator==(const Material& other) const;
	};
	static Material getMaterial(vtkProperty* property);
	static void applyMaterial(vtkProperty* property, Material material);
	void overrideMaterial(vtkProperty* property);

	vtkSmartPointer<vtkRenderPass> mDelegatePass;
	Material mMaterial;
	struct OriginalMaterial
	{
		vtkWeakPointer<vtkProperty> property;
		Material material;
	};
	std::map<vtkProperty*, OriginalMaterial> mOriginalMaterials;
};

/**
 * @}
 */
} //namespace cx

#endif // CXMATERIALOVERRIDEPASS_H
