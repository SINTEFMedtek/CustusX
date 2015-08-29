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

#include "cxTool.h"

#include <vtkConeSource.h>
#include "cxToolNull.h"
#include "cxNullDeleter.h"

namespace cx
{
vtkPolyDataPtr Tool::createDefaultPolyDataCone()
{
    vtkConeSourcePtr coneSource = vtkConeSourcePtr::New();
    coneSource->SetResolution(25);
    coneSource->SetRadius(10);
    coneSource->SetHeight(100);

    coneSource->SetDirection(0, 0, 1);
    double newCenter[3];
    coneSource->GetCenter(newCenter);
    newCenter[2] = newCenter[2] - coneSource->GetHeight() / 2;
    coneSource->SetCenter(newCenter);

    coneSource->Update();
    return coneSource->GetOutput();
}

Tool::Tool(const QString &uid, const QString &name) :
	mUid(uid), mName(name)
{
	if (name.isEmpty())
		mName = uid;
}

ToolPtr Tool::getNullObject()
{
	static ToolPtr mNull;
	if (!mNull)
		mNull.reset(new ToolNull, null_deleter());
	return mNull;
}
} //cx

