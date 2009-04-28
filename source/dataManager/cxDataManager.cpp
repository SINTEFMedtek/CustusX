#include "cxDataManager.h"

#include <QDomDocument>

namespace cx
{
DataManager* DataManager::mCxInstance = NULL;

DataManager* DataManager::getInstance()
{
  if (mCxInstance == NULL)
  {
    mCxInstance = new DataManager();
    ssc::DataManager::setInstance(mCxInstance);
  }
  return mCxInstance;
}
DataManager::DataManager()
{}
DataManager::~DataManager()
{}

void DataManager::save(QString filename)
{
	QDomDocument doc("CustusX 3 save file");
	// Call getXml() of all manager that have things that should be saved
  QDomNode datanode = this->getXml(doc);
}

void DataManager::load(QString filename)
{
  QDomNode datanode;
	// Call parseXml() of all managers that have things that should be loaded
  this->parseXml(datanode);
}

QDomNode DataManager::getXml(QDomDocument& doc)
{
  QDomElement datanode = doc.createElement("DataManager");
	
	// Call getXml() of all objects that have things that should be saved
	
  return datanode;
}

void DataManager::parseXml(QDomNode& datanode)
{
	// Call parseXml() of all object that have things that should be loaded
}

}//namespace cx
