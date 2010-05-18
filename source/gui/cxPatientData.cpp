/*
 * cxPatientData.cpp
 *
 *  Created on: May 18, 2010
 *      Author: christiana
 */
#include "cxPatientData.h"

#include <QtGui>
#include <QDomDocument>

#include "sscTime.h"

#include "cxDataManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxRegistrationManager.h"
#include "cxToolManager.h"
#include "cxMessageManager.h"
#include "cxFileCopied.h"

namespace cx
{

PatientData::PatientData(QObject* parent) :
    QObject(parent),
    mSettings(new QSettings())
{

}

QString PatientData::getActivePatientFolder() const
{
  return mActivePatientFolder;
}

void PatientData::setActivePatient(const QString& activePatientFolder)
{
  mActivePatientFolder = activePatientFolder;
  //TODO
  //Update gui in some way to show which patient is active
}

void PatientData::newPatient(QString choosenDir)
{
  createPatientFolders(choosenDir);
}

//void PatientData::loadPatientFileSlot()
void PatientData::loadPatient(QString choosenDir)
{
//  // Open file dialog
//  QString choosenDir = QFileDialog::getExistingDirectory(this, tr("Open directory"),
//                                                         mSettings->value("globalPatientDataFolder").toString(),
//                                                         QFileDialog::ShowDirsOnly);
  if (choosenDir == QString::null)
    return; // On cancel

  // Set active patient folder, relative to globalPatientDataFolder
  QDir patientDataDir(mSettings->value("globalPatientDataFolder").toString());
  //mActivePatientFolder = patientDataDir.relativeFilePath(choosenDir);
  this->setActivePatient(patientDataDir.relativeFilePath(choosenDir));

  QFile file(choosenDir+"/custusdoc.xml");
  if(file.open(QIODevice::ReadOnly))
  {
    QDomDocument doc;
    QString emsg;
    int eline, ecolumn;
    // Read the file
    if (!doc.setContent(&file, false, &emsg, &eline, &ecolumn))
    {
      messageManager()->sendError("Could not parse XML file :"
                                 +file.fileName().toStdString()+" because: "
                                 +emsg.toStdString()+"");
    }
    else
    {
      //Read the xml
      this->readLoadDoc(doc);
    }
    file.close();
  }
  else //User have created the directory create xml file and folders
  {
    createPatientFolders(choosenDir);
  }
}

void PatientData::savePatient()
{

  if(mActivePatientFolder.isEmpty())
  {
//    messageManager()->sendWarning("No patient selected, select or create patient before saving!");
//    this->newPatientSlot();
    return;
  }

  //Gather all the information that needs to be saved
  QDomDocument doc;
  this->generateSaveDoc(doc);

  QString activePatientDir = mSettings->value("globalPatientDataFolder").toString();
  activePatientDir += "/"+mActivePatientFolder;
  QFile file(activePatientDir + "/custusdoc.xml");
  if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    QTextStream stream(&file);
    stream << doc.toString();
    file.close();
    messageManager()->sendInfo("Created "+file.fileName().toStdString());
  }
  else
  {
    messageManager()->sendError("Could not open "+file.fileName().toStdString()
                               +" Error: "+file.errorString().toStdString());
  }

  //Write the data to file, fx modified images... etc...
  //TODO Implement when we know what we want to save here...
}

void PatientData::importData(QString fileName)
{
//  this->savePatientFileSlot();

//  messageManager()->sendInfo("Importing data...");
//  QString fileName = QFileDialog::getOpenFileName( this,
//                                  QString(tr("Select data file")),
//                                  mSettings->value("globalPatientDataFolder").toString(),
//                                  tr("Image/Mesh (*.mhd *.mha *.stl *.vtk)"));
  if(fileName.isEmpty())
  {
    messageManager()->sendInfo("Import canceled");
    return;
  }

  QString globalPatientFolderPath = mSettings->value("globalPatientDataFolder").toString();
  QString patientsImageFolder = globalPatientFolderPath+"/"+mActivePatientFolder+"/Images/";
  QString patientsSurfaceFolder = globalPatientFolderPath+"/"+mActivePatientFolder+"/Surfaces/";

  QDir dir;
  if(!dir.exists(patientsImageFolder))
  {
    dir.mkpath(patientsImageFolder);
    messageManager()->sendInfo("Made new directory: "+patientsImageFolder.toStdString());
  }
  if(!dir.exists(patientsSurfaceFolder))
  {
    dir.mkpath(patientsSurfaceFolder);
    messageManager()->sendInfo("Made new directory: "+patientsSurfaceFolder.toStdString());
  }

  QFileInfo fileInfo(fileName);
  QString fileType = fileInfo.suffix();
  QString pathToNewFile = patientsImageFolder+fileInfo.fileName();
  QFile fromFile(fileName);
  QString uid = fileInfo.fileName()+"_"+fileInfo.created().toString(ssc::timestampSecondsFormat());

  //Need to wait for the copy to finish...

  // Read files before copy
  ssc::DataPtr data;

  if(fileType.compare("mhd", Qt::CaseInsensitive) == 0 ||
     fileType.compare("mha", Qt::CaseInsensitive) == 0)
  {
    data = dataManager()->loadImage(uid.toStdString(), fileName.toStdString(), ssc::rtMETAIMAGE);
  }else if(fileType.compare("stl", Qt::CaseInsensitive) == 0)
  {
    data = dataManager()->loadMesh(uid.toStdString(), fileName.toStdString(), ssc::mrtSTL);
    pathToNewFile = patientsSurfaceFolder+fileInfo.fileName();
  }else if(fileType.compare("vtk", Qt::CaseInsensitive) == 0)
  {
    data = dataManager()->loadMesh(uid.toStdString(), fileName.toStdString(), ssc::mrtPOLYDATA);
    pathToNewFile = patientsSurfaceFolder+fileInfo.fileName();
  }
  data->setName(fileInfo.fileName().toStdString());

  QDir patientDataDir(mSettings->value("globalPatientDataFolder").toString()
                      +"/"+mActivePatientFolder);
  FileCopied *fileCopied = new FileCopied(pathToNewFile.toStdString(),
                                          patientDataDir.relativeFilePath(pathToNewFile).toStdString(),
                                          data);
  connect(fileCopied, SIGNAL(fileCopiedCorrectly()),
          this, SLOT(savePatient()));
  QTimer::singleShot(5000, fileCopied, SLOT(areFileCopiedSlot()));// Wait 5 seconds

  //Copy file
  if(fileName != pathToNewFile) //checks if we need to copy
  {
    QFile toFile(pathToNewFile);
    if(fromFile.copy(toFile.fileName()))
    {
      //messageMan()->sendInfo("File copied to new location: "+pathToNewFile.toStdString());
    }else
    {
      messageManager()->sendError("First copy failed!");
      return;
    }
    if(!toFile.flush())
      messageManager()->sendWarning("Failed to copy file"+toFile.fileName().toStdString());
    if(!toFile.exists())
      messageManager()->sendWarning("File not copied");
    //make sure we also copy the .raw file in case if mhd/mha
    if(fileType.compare("mhd", Qt::CaseInsensitive) == 0)
    {
      //presuming the other file is a raw file
      //TODO: what if it's not?
      QString originalRawFile = fileName.replace(".mhd", ".raw");
      QString newRawFile = pathToNewFile.replace(".mhd", ".raw");
      fromFile.setFileName(originalRawFile);
      toFile.setFileName(newRawFile);

      if(fromFile.copy(toFile.fileName()))
      {
        //messageMan()->sendInfo("File copied to new location: "+newRawFile.toStdString());
      }
      else
      {
        messageManager()->sendError("Second copy failed!");
        return;
      }
      if(!toFile.flush())
        messageManager()->sendWarning("Failed to copy file"+toFile.fileName().toStdString());
      if(!toFile.exists())
        messageManager()->sendWarning("File not copied");

    }else if(fileType.compare("mha", Qt::CaseInsensitive) == 0)
    {
      //presuming the other file is a raw file
      //TODO: what if it's not?
      QString originalRawFile = fileName.replace(".mha", ".raw");
      QString newRawFile = pathToNewFile.replace(".mha", ".raw");
      fromFile.setFileName(originalRawFile);
      toFile.setFileName(newRawFile);

      if(fromFile.copy(toFile.fileName()))
      {
        //messageMan()->sendInfo("File copied to new location: "+newRawFile.toStdString());
      }
      else
      {
        messageManager()->sendError("Second copy failed!");
        return;
      }
    }
  }
}

void PatientData::createPatientFolders(QString choosenDir)
{
  if(!choosenDir.endsWith(".cx3"))
    choosenDir.append(".cx3");

  // Set active patient folder. Use path relative to the globalPatientDataFolder
  QString patientDatafolder = mSettings->value("globalPatientDataFolder").toString();
  QDir patientDataDir(patientDatafolder);
  //mActivePatientFolder = patientDataDir.relativeFilePath(choosenDir);
  this->setActivePatient(patientDataDir.relativeFilePath(choosenDir));
  messageManager()->sendInfo("Selected a patient to work with.");

  // Create folders
  if(!QDir().exists(choosenDir))
  {
    QDir().mkdir(choosenDir);
    messageManager()->sendInfo("Made a new patient folder: "+choosenDir.toStdString());
  }

  QString newDir = choosenDir;
  newDir.append("/Images");
  if(!QDir().exists(newDir))
  {
    QDir().mkdir(newDir);
    messageManager()->sendInfo("Made a new image folder: "+newDir.toStdString());
  }

  newDir = choosenDir;
  newDir.append("/Surfaces");
  if(!QDir().exists(newDir))
  {
    QDir().mkdir(newDir);
    messageManager()->sendInfo("Made a new surface folder: "+newDir.toStdString());
  }

  newDir = choosenDir;
  newDir.append("/Logs");
  if(!QDir().exists(newDir))
  {
    QDir().mkdir(newDir);
    messageManager()->sendInfo("Made a new logging folder: "+newDir.toStdString());
  }

  this->savePatient();
}


/**
 * Xml version 1.0: Knows about the nodes: \n
 * \<managers\> \n
 *   \<datamanager\> \n
 *     \<image\> \n
 *        \<uid\> //an images unique id \n
 *        \<name\> //an images name \n
 *        \<transferfunctions\> //an images transferefunction \n
 *            \<alpha\> //a transferefunctions alpha values \n
 *            \<color\> //a transferefunctions color values
 */
/**
 * Xml version 2.0: Knows about the nodes: \n
 * \<patient\> \n
 *  \<active_patient\> //relative path to this patients folder \n
 *  \<managers\> \n
 *     \<datamanager\> \n
 *       \<image\> \n
 *         \<uid\> //an images unique id \n
 *         \<name\> //an images name \n
 *         \<transferfunctions\> //an images transferefunction \n
 *            \<alpha\> //a transferefunctions alpha values \n
 *            \<color\> //a transferefunctions color values \n
 */
void PatientData::generateSaveDoc(QDomDocument& doc)
{
  doc.appendChild(doc.createProcessingInstruction("xml version =", "'2.0'"));

  QDomElement patientNode = doc.createElement("patient");
  QDomElement activePatientNode = doc.createElement("active_patient");
  activePatientNode.appendChild(doc.createTextNode(mActivePatientFolder.toStdString().c_str()));
  patientNode.appendChild(activePatientNode);
  doc.appendChild(patientNode);

  QDomElement managerNode = doc.createElement("managers");
  patientNode.appendChild(managerNode);

  dataManager()->addXml(managerNode);
  toolManager()->addXml(managerNode);
  viewManager()->addXml(managerNode);
  registrationManager()->addXml(managerNode);

  //TODO Implement
  /*
  messageManager()->getXml(doc); //TODO
  repManager->getXml(doc); //TODO
  */

  messageManager()->sendInfo("Xml file ready to be written to disk.");
}
void PatientData::readLoadDoc(QDomDocument& doc)
{
  //Get all the nodes
  QDomNode patientNode = doc.namedItem("patient");
  QDomNode managerNode = patientNode.namedItem("managers");

  //Evaluate the xml nodes and load what's needed
  QDomNode dataManagerNode = managerNode.namedItem("datamanager");
  if(!patientNode.isNull())
  {
    QDomElement activePatientNode = patientNode.namedItem("active_patient").toElement();
    if(!activePatientNode.isNull())
    {
      //mActivePatientFolder = activePatientNode.text();
      this->setActivePatient(activePatientNode.text());
      messageManager()->sendInfo("Active patient loaded to be "
                                +mActivePatientFolder.toStdString());
    }
  }
  if (!dataManagerNode.isNull())
  {
    QString absolutePatientPath = mSettings->value("globalPatientDataFolder").toString()+"/"+mActivePatientFolder;
    dataManager()->parseXml(dataManagerNode, absolutePatientPath);
  }

  QDomNode toolmanagerNode = managerNode.namedItem("toolManager");
  toolManager()->parseXml(toolmanagerNode);

  QDomNode viewmanagerNode = managerNode.namedItem("viewManager");
  viewManager()->parseXml(viewmanagerNode);

  QDomNode registrationNode = managerNode.namedItem("registrationManager");
  registrationManager()->parseXml(registrationNode);
}


} // namespace cx

