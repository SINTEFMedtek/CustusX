#include "shadercallback.h"

#include <vtkOpenGLHelper.h>
#include <vtkShaderProgram.h>

#include "vtkfixture.h"

static bool uploaded = false;

ShaderCallback *ShaderCallback::New()
{ return new ShaderCallback; }

void ShaderCallback::Execute(vtkObject *, unsigned long event, void *cbo)
{
	if(event == vtkCommand::UpdateShaderEvent)
	{
		std::cout << "UpdateShaderEvent" << std::endl;
		vtkOpenGLHelper *cellBO = reinterpret_cast<vtkOpenGLHelper*>(cbo);

		int texture_0 = 0;
		int texture_1 = 1;
		cellBO->Program->SetUniform1iv("my_texture[0]", 1, &texture_0);
		cellBO->Program->SetUniform1iv("my_texture[1]", 1, &texture_1);
	}
}

ShaderCallback::ShaderCallback()
{
}
