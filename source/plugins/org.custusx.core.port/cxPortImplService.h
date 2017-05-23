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

#ifndef CXPORTIMPLSERVICE_H
#define CXPORTIMPLSERVICE_H

#include "cxPortService.h"
#include "org_custusx_core_port_Export.h"
class ctkPluginContext;

namespace cx
{

class org_custusx_core_port_EXPORT PortImplService : public PortService
{
public:
	Q_INTERFACES(cx::PortService)

	PortImplService(ctkPluginContext *context);
	virtual ~PortImplService();
	virtual bool isNull();

	bool canLoad(const QString& type, const QString& filename);
	DataPtr load(const QString& uid, const QString& filename);
	vtkImageDataPtr loadVtkImageData(QString filename);
	vtkPolyDataPtr loadVtkPolyData(QString filename);
	QString findDataTypeFromFile(QString filename);
	bool readInto(DataPtr data, QString path);
	void saveImage(ImagePtr image, const QString& filename);

	void addPort(PortService *service);
	void removePort(PortService *service);

private:
	PortServicePtr findReader(const QString& path, const QString& type="unknown");
	std::set<PortServicePtr> mDataReaders;
};

typedef boost::shared_ptr<PortImplService> PortImplServicePtr;

} //cx

#endif // CXPORTIMPLSERVICE_H
