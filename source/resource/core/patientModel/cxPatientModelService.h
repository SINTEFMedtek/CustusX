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

#ifndef CXPATIENTMODELSERVICE_H_
#define CXPATIENTMODELSERVICE_H_

#include <QObject>
#include <map>
#include "boost/shared_ptr.hpp"
#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
class QDateTime;

namespace cx
{
typedef boost::shared_ptr<class Data> DataPtr;
typedef boost::shared_ptr<class Image> ImagePtr;
}

#define PatientModelService_iid "cx::PatientModelService"

namespace cx
{
class RegistrationTransform;
class LandmarkProperty;
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class Landmarks> LandmarksPtr;
typedef boost::shared_ptr<class TransferFunctions3DPresets> PresetTransferFunctions3DPtr;

/** \brief The virtual patient
 *
 * PatientModelService provides access to the Patient Specific Model (PaSM).
 *   - data entities
 *   	- volumes
 *   	- surfaces
 *   	- metrics
 *   	- etc
 *   - relations between entities in space, time and structure
 *   - load/save
 *
 * This service replaces the old DataManager and PatientData classes. They
 * are deprecated.
 *
 *  \ingroup cx_resource_core_data
 *  \date 2014-05-15
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 */
class PatientModelService : public QObject
{
	Q_OBJECT
public:
	virtual ~PatientModelService() {}

	virtual void insertData(DataPtr data) = 0;
	virtual void updateRegistration_rMpr(const QDateTime& oldTime, const RegistrationTransform& newTransform) = 0;
	virtual std::map<QString, DataPtr> getData() const = 0;
	virtual DataPtr getData(const QString& uid) const = 0;
	virtual LandmarksPtr getPatientLandmarks() const = 0;
	virtual std::map<QString, LandmarkProperty> getLandmarkProperties() const = 0;
	virtual Transform3D get_rMpr() const = 0;
	virtual ImagePtr getActiveImage() const = 0; ///< used for system state
	virtual void setActiveImage(ImagePtr activeImage) = 0; ///< used for system state
	virtual ImagePtr createDerivedImage(vtkImageDataPtr data, QString uid, QString name, ImagePtr parentImage, QString filePath = "Images") = 0;
	virtual MeshPtr createMesh(vtkPolyDataPtr data, QString uidBase, QString nameBase, QString filePath) = 0;
	virtual void loadData(DataPtr data) = 0;
	virtual void saveData(DataPtr data, const QString& basePath) = 0; ///< Save data to file
	virtual void saveImage(ImagePtr image, const QString& basePath) = 0;///< Save image to file \param image Image to save \param basePath Absolute path to patient data folder
	virtual void saveMesh(MeshPtr mesh, const QString& basePath) = 0;///< Save mesh to file \param mesh to save \param basePath Absolute path to patient data folder
	virtual std::map<QString, VideoSourcePtr> getStreams() const = 0;

	virtual QString getActivePatientFolder() const = 0;
	virtual bool isPatientValid() const = 0;
	virtual DataPtr importData(QString fileName, QString &infoText) = 0;
	virtual void exportPatient(bool niftiFormat) = 0;
	virtual void removePatientData(QString uid) = 0;

	virtual PresetTransferFunctions3DPtr getPresetTransferFunctions3D() const = 0;

	virtual void autoSave() = 0;//TODO remove, and integrate into other functions
	virtual bool isNull() = 0;

	virtual bool getDebugMode() const = 0;
	virtual void setDebugMode(bool on) = 0;

	static PatientModelServicePtr getNullObject();

	ImagePtr getImage(const QString& uid) const; ///< Convenience function casting from Data to Image
	MeshPtr getMesh(const QString& uid) const;  ///< Convenience function casting from Data to Mesh
	VideoSourcePtr getStream(const QString &uid) const; ///< Convenience function getting a specified stream
	void saveData(DataPtr data); ///< Convenience funciton for saving data to avtive patient folder
	void saveImage(ImagePtr image); ///< Convenience funciton for saving image to avtive patient folder
	void saveMesh(MeshPtr mesh);  ///< Convenience funciton for saving mesh to avtive patient folder
signals:
	void dataAddedOrRemoved();
	void activeImageChanged(const QString& uId);
	void debugModeChanged(bool on);
	void rMprChanged();
	void streamLoaded();
};

} // namespace cx
Q_DECLARE_INTERFACE(cx::PatientModelService, PatientModelService_iid)


#endif /* CXPATIENTMODELSERVICE_H_ */
