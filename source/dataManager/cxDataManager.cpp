#include <QDomDocument>
#include "cxDataManager.h"

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
void DataManager::addXml(QDomNode& parentNode)
{
  ssc::DataManagerImpl::addXml(parentNode);
}
void DataManager::parseXml(QDomNode& node)
{
  ssc::DataManagerImpl::parseXml(node);
}
bool DataManager::write(QString& folder)
{
  bool success = true;
  //TODO
  std::cout << "Implement DataManager::write" << std::endl;
  return success;
}
bool DataManager::load(QString& folder)
{
  bool success = true;
  //TODO
  std::cout << "Implement DataManager::load" << std::endl;
  return success;
}
/*
QDomDocument DataManager::save() //TODO MOVE TO MainWindow
{
	QDomDocument doc("CustusX3 patient file");
  //QDomElement docElement = doc.documentElement();
  QDomElement docElement = doc.createElement("custus3");
  doc.appendChild(docElement);
  // Call getXml() of all manager that have things that should be saved
  docElement.appendChild(this->getXml(doc));
  return doc;
}

void DataManager::load(QDomDocument& doc)//TODO MOVE TO MainWindow
{
  QDomNode topNode = doc.namedItem("custus3");
  // Call parseXml() of all managers that have things that should be loaded
  QDomNode dataManagerNode = topNode.namedItem("DataManager");
  if (!dataManagerNode.isNull())
    this->parseXml(dataManagerNode);
  else
    std::cout << "Warning: DataManager::load(): No DataManager node" << std::endl;
}*/

}//namespace cx
