/*
 * cxFrameForest.cpp
 *
 *  Created on: Sep 23, 2010
 *      Author: christiana
 */

#include "cxFrameForest.h"

namespace cx
{

FrameForest::FrameForest()
{
  std::cout << "Forrest doc2:" << std::endl;
  ssc::DataManager::DataMap allData = ssc::dataManager()->getData();
  mDocument.appendChild(mDocument.createElement("root"));

  for (ssc::DataManager::DataMap::iterator iter=allData.begin(); iter!=allData.end(); ++iter)
  {
    this->insertFrame(iter->second);
  }

  std::cout << "Forrest doc:" << std::endl;
  std::cout << mDocument.toString(4) << std::endl;
}


}
