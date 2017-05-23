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

#include "cxPortImplService.h"
#include <QFileInfo>
#include "cxTypeConversions.h"
#include "cxUtilHelpers.h"
#include "cxNullDeleter.h"

namespace cx
{

PortImplService::PortImplService(ctkPluginContext *context)
{
}

PortImplService::~PortImplService()
{
}

bool PortImplService::isNull()
{
	return false;
}

bool PortImplService::canLoad(const QString &type, const QString &filename)
{
	PortServicePtr reader = this->findReader(filename);
	if (reader)
		return reader->canLoad(type, filename);
	else
		return false;
}

DataPtr PortImplService::load(const QString &uid, const QString &filename)
{
	PortServicePtr reader = this->findReader(filename);
	if (reader)
		return reader->load(uid, filename);
	else
		return DataPtr();
}

PortServicePtr PortImplService::findReader(const QString& path, const QString& type)
{
	for (std::set<PortServicePtr>::iterator iter = mDataReaders.begin(); iter != mDataReaders.end(); ++iter)
	{
		if ((*iter)->canLoad(type, path))
			return *iter;
	}
	return PortServicePtr();
}

vtkImageDataPtr PortImplService::loadVtkImageData(QString filename)
{
	PortServicePtr reader = this->findReader(filename);
	if (reader)
		return reader->loadVtkImageData(filename);
	return vtkImageDataPtr();
}

vtkPolyDataPtr PortImplService::loadVtkPolyData(QString filename)
{
	PortServicePtr reader = this->findReader(filename);
	if (reader)
		return reader->loadVtkPolyData(filename);
	return vtkPolyDataPtr();
}

QString PortImplService::findDataTypeFromFile(QString filename)
{
	PortServicePtr reader = this->findReader(filename);
	if (reader)
		return reader->canLoadDataType();
	return "";
}

bool PortImplService::readInto(DataPtr data, QString path)
{
	PortServicePtr reader = this->findReader(path, data->getType());
	if (reader)
		reader->readInto(data, path);

	if(data)
	{
		QFileInfo fileInfo(qstring_cast(path));
		data->setName(changeExtension(fileInfo.fileName(), ""));
		data->setFilename(path); // need path even when not set explicitly: nice for testing
	}

}

void PortImplService::saveImage(ImagePtr image, const QString &filename)
{
	PortServicePtr reader = this->findReader(filename);
	if (reader)
		return reader->saveImage(image, filename);
}

void PortImplService::addPort(PortService *service)
{
	//TODO
	// adding a service inside a smartpointer... not so smart, think it is fixed with null_deleter
	mDataReaders.insert(PortServicePtr(service, null_deleter()));
}

void PortImplService::removePort(PortService *service)
{
	//TODO
	std::cout << "ERROR: unable to remove PortService" << std::endl;

}

} // cx
