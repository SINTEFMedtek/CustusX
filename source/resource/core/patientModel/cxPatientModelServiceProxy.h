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

#ifndef CXPATIENTMODELSERVICEPROXY_H
#define CXPATIENTMODELSERVICEPROXY_H

#include "cxPatientModelService.h"
#include <boost/shared_ptr.hpp>
#include "cxServiceTrackerListener.h"
class ctkPluginContext;

namespace cx
{

typedef boost::shared_ptr<class RegistrationService> RegistrationServicePtr;

/** \brief Always provides a PatientModelService
 *
 * Use the Proxy design pattern.
 * Uses ServiceTrackerListener to either provide an
 * implementation of PatientModelService or
 * the null object (PatientModelServiceNull)
 *
 *  \ingroup cx_resource_core_registration
 *  \date 2014-09-10
 *  \author Ole Vegard Solberg, SINTEF
 */
class PatientModelServiceProxy : public PatientModelService
{
public:
	PatientModelServiceProxy(ctkPluginContext *context);
	virtual ~PatientModelServiceProxy();

	virtual void insertData(DataPtr data);
	virtual void updateRegistration_rMpr(const QDateTime& oldTime, const RegistrationTransform& newTransform);
	virtual std::map<QString, DataPtr> getData() const;
	virtual DataPtr getData(const QString& uid) const;
	virtual LandmarksPtr getPatientLandmarks() const;
	virtual std::map<QString, LandmarkProperty> getLandmarkProperties() const;
	virtual Transform3D get_rMpr() const;
	virtual ImagePtr getActiveImage() const;
	virtual void setActiveImage(ImagePtr activeImage);
	virtual ImagePtr createDerivedImage(vtkImageDataPtr data, QString uid, QString name, ImagePtr parentImage, QString filePath);
	virtual MeshPtr createMesh(vtkPolyDataPtr data, QString uidBase, QString nameBase, QString filePath);
	virtual void loadData(DataPtr data);
	virtual void saveData(DataPtr data, const QString& basePath); ///< Save data to file
	virtual void saveImage(ImagePtr image, const QString& basePath);
	virtual void saveMesh(MeshPtr mesh, const QString& basePath);
	virtual std::map<QString, VideoSourcePtr> getStreams() const;

	virtual QString getActivePatientFolder() const;

	virtual void autoSave();
	virtual bool isNull();

	virtual bool getDebugMode() const;
	virtual void setDebugMode(bool on);

private:
	void initServiceListener();
	void onServiceAdded(PatientModelService* service);
	void onServiceRemoved(PatientModelService *service);

	ctkPluginContext *mPluginContext;
	PatientModelServicePtr mPatientModelService;
	boost::shared_ptr<ServiceTrackerListener<PatientModelService> > mServiceListener;
};

} // cx
#endif // CXPATIENTMODELSERVICEPROXY_H
