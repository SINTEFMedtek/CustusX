/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPatientData.h"

#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QTextStream>
#include <QApplication>

#include "cxTime.h"
#include "cxLogger.h"
#include "cxUtilHelpers.h"
#include "cxCustomMetaImage.h"
#include "cxMesh.h"

#include "cxSettings.h"

#include <vtkPolyData.h>
#include <vtkPointData.h>

#include "cxDataManager.h"
#include "cxImage.h"
#include "cxTypeConversions.h"
#include "cxConfig.h"
#include "cxSessionStorageService.h"
#include "cxXMLNodeWrapper.h"
#include "cxDataFactory.h"

namespace cx
{


PatientData::PatientData(DataServicePtr dataManager, SessionStorageServicePtr session) :
	mDataManager(dataManager),
	mSession(session)
{
	connect(mSession.get(), &SessionStorageService::sessionChanged, this, &PatientData::patientChanged);
	connect(mSession.get(), &SessionStorageService::cleared, this, &PatientData::onCleared);
//	connect(mSession.get(), &SessionStorageService::cleared, this, &PatientData::cleared);
	connect(mSession.get(), &SessionStorageService::isLoading, this, &PatientData::onSessionLoad);
	connect(mSession.get(), &SessionStorageService::isSaving, this, &PatientData::onSessionSave);
}

PatientData::~PatientData()
{}

QString PatientData::getActivePatientFolder() const
{
	return mSession->getRootFolder();
}

bool PatientData::isPatientValid() const
{
	return mSession->isValid();
}

void PatientData::onCleared()
{
	mDataManager->clear();
}

void PatientData::onSessionLoad(QDomElement &node)
{
	XMLNodeParser root(node);
	QDomElement dataManagerNode = root.descend("managers/datamanager").node().toElement();

	if (!dataManagerNode.isNull())
		mDataManager->parseXml(dataManagerNode, mSession->getRootFolder());
}

void PatientData::onSessionSave(QDomElement &node)
{
	XMLNodeAdder root(node);
	QDomElement managerNode = root.descend("managers").node().toElement();

	mDataManager->addXml(managerNode);

	// save position transforms into the mhd files.
	// This hack ensures data files can be used in external programs without an explicit export.
	DataManager::ImagesMap images = mDataManager->getImages();
	for (DataManager::ImagesMap::iterator iter = images.begin(); iter != images.end(); ++iter)
	{
		if(!iter->second->getFilename().isEmpty())
		{
			CustomMetaImagePtr customReader = CustomMetaImage::create(mSession->getRootFolder() + "/" + iter->second->getFilename());
			customReader->setTransform(iter->second->get_rMd());
		}
	}

}

void PatientData::autoSave()
{
	if (settings()->value("Automation/autoSave").toBool())
		mSession->save();
}

void PatientData::exportPatient(PATIENT_COORDINATE_SYSTEM externalSpace)
{
	QString targetFolder = mSession->getRootFolder() + "/Export/"
					+ QDateTime::currentDateTime().toString(timestampSecondsFormat());

	DataManager::ImagesMap images = mDataManager->getImages();
	for (DataManager::ImagesMap::iterator iter = images.begin(); iter != images.end(); ++iter)
	{
		iter->second->save(targetFolder);
	}

	DataManager::MeshMap meshes = mDataManager->getMeshes();
	for (DataManager::MeshMap::iterator iter = meshes.begin(); iter != meshes.end(); ++iter)
	{
		MeshPtr mesh = iter->second;

		Transform3D rMd = mesh->get_rMd();
		Transform3D sMr = createTransformFromReferenceToExternal(externalSpace);
		Transform3D sMd = sMr * rMd;

		vtkPolyDataPtr poly = mesh->getTransformedPolyDataCopy(sMd);
		// create a copy with the SAME UID as the original. Do not load this one into the datamanager!
		mesh = mDataManager->getDataFactory()->createSpecific<Mesh>(mesh->getUid(), mesh->getName());
		mesh->setVtkPolyData(poly);
		mesh->setFilename("Images");
		mesh->save(targetFolder);
	}

	report("Exported patient data to " + targetFolder + ".");
}

DataPtr PatientData::importData(QString fileName, QString &infoText)
{
	if (fileName.isEmpty())
	{
		QString text = "Import canceled";
		report(text);
		infoText = "<font color=red>" + text + "</font>";
		return DataPtr();
	}

	QFileInfo fileInfo(fileName);
	QString strippedFilename = changeExtension(fileInfo.fileName(), "");
	QString uid = strippedFilename + "_" + QDateTime::currentDateTime().toString(timestampSecondsFormat());

	if (mDataManager->getData(uid))
	{
		QString text = "Data with uid " + uid + " already exists. Import canceled.";
		reportWarning(text);
		infoText = "<font color=red>" + text + "</font>";
		return DataPtr();
	}

	// Read files before copy
	DataPtr data = mDataManager->loadData(uid, fileName);
	if (!data)
	{
		QString text = "Error with data file: " + fileName + " Import canceled.";
		reportWarning(text);
		infoText = "<font color=red>" + text + "</font>";
		return DataPtr();
	}
	data->setAcquisitionTime(QDateTime::currentDateTime());

	data->save(mSession->getRootFolder());

	// remove redundant line breaks
	infoText = infoText.split("<br>", QString::SkipEmptyParts).join("<br>");

	return data;
}

void PatientData::removeData(QString uid)
{
	mDataManager->removeData(uid, this->getActivePatientFolder());
}

} // namespace cx

