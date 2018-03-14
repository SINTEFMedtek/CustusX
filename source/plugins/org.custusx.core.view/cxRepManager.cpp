/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRepManager.h"

namespace cx
{


RepManager* RepManager::mTheInstance = NULL;

RepManager* RepManager::getInstance()
{
  if(mTheInstance == NULL)
   {
     mTheInstance = new RepManager();
   }
   return mTheInstance;
}

void RepManager::destroyInstance()
{
  delete mTheInstance;
  mTheInstance = NULL;
}

RepManager::RepManager()
{
}

RepManager::~RepManager()
{
}

}//namespace cx
