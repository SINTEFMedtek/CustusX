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

#include "cxFileManagerServicePluginActivator.h"

#include <QtPlugin>

#include "cxImportExportGuiExtenderService.h"
#include "cxFileManagerImplService.h"
#include "cxMetaImageReader.h"
#include "cxPNGImageReader.h"
#include "cxPolyDataMeshReader.h"
#include "cxXMLPolyDataMeshReader.h"
#include "cxStlMeshReader.h"
#include "cxNIfTIReader.h"
#include "cxMNIReaderWriter.h"
#include "cxLogger.h"
#include "cxPatientModelServiceProxy.h"
#include "cxViewServiceProxy.h"

namespace cx
{

FileManagerServicePluginActivator::FileManagerServicePluginActivator()
{
}

FileManagerServicePluginActivator::~FileManagerServicePluginActivator()
{
	if(!mRegisteredFileReaderWriterServices.empty())
		CX_LOG_ERROR() << "FileReaderWriterServices is not empty.";
}

void FileManagerServicePluginActivator::start(ctkPluginContext* context)
{
	FileManagerImpService *filemanager = new FileManagerImpService(context);
	mRegisteredFileManagerService = RegisteredService::create<FileManagerImpService>(context, filemanager, FileManagerService_iid);

	PatientModelServicePtr patientModelService = PatientModelServiceProxy::create(context);
	ViewServicePtr viewService = ViewServiceProxy::create(context);

	mRegisteredFileReaderWriterServices.push_back(RegisteredService::create<MetaImageReader>(context, new MetaImageReader(patientModelService), FileReaderWriterService_iid));
	mRegisteredFileReaderWriterServices.push_back(RegisteredService::create<PNGImageReader>(context, new PNGImageReader(patientModelService), FileReaderWriterService_iid));
	mRegisteredFileReaderWriterServices.push_back(RegisteredService::create<PolyDataMeshReader>(context, new PolyDataMeshReader(patientModelService), FileReaderWriterService_iid));
	mRegisteredFileReaderWriterServices.push_back(RegisteredService::create<XMLPolyDataMeshReader>(context, new XMLPolyDataMeshReader(patientModelService), FileReaderWriterService_iid));
	mRegisteredFileReaderWriterServices.push_back(RegisteredService::create<StlMeshReader>(context, new StlMeshReader(patientModelService), FileReaderWriterService_iid));
	mRegisteredFileReaderWriterServices.push_back(RegisteredService::create<NIfTIReader>(context, new NIfTIReader(patientModelService), FileReaderWriterService_iid));
	mRegisteredFileReaderWriterServices.push_back(RegisteredService::create<MNIReaderWriter>(context, new MNIReaderWriter(patientModelService, viewService), FileReaderWriterService_iid));

	mRegisteredGuiExtenderService = RegisteredService::create<ImportExportGuiExtenderService>(context, new ImportExportGuiExtenderService(context), GUIExtenderService_iid);
}

void FileManagerServicePluginActivator::stop(ctkPluginContext* context)
{

	for(std::vector<RegisteredServicePtr>::iterator it = mRegisteredFileReaderWriterServices.begin(); it != mRegisteredFileReaderWriterServices.end(); )
	{
		(*it).reset();
		it = mRegisteredFileReaderWriterServices.erase(it);
	}

	mRegisteredGuiExtenderService.reset();
	mRegisteredFileManagerService.reset();

	Q_UNUSED(context);
}

} // cx
