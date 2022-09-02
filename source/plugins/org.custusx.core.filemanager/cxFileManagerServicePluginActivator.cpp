/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxDICOMReader.h"
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
	mRegisteredFileReaderWriterServices.push_back(RegisteredService::create<DICOMReader>(context, new DICOMReader(patientModelService), FileReaderWriterService_iid));

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
