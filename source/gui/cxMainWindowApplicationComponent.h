#ifndef CXMAINWINDOWAPPLICATIONCOMPONENT_H
#define CXMAINWINDOWAPPLICATIONCOMPONENT_H


/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLogicManager.h"
#include "cxApplication.h"

namespace cx
{

/**
 * An ApplicationComponent adding a Widget as MainWindow to
 * the LogicManager.
 */
template<class MAIN_WINDOW>
class MainWindowApplicationComponent : public ApplicationComponent
{
public:
	virtual void create()
	{
		if (this->exists())
			return;
		mMainWindow = new MAIN_WINDOW;
		cx::bringWindowToFront(mMainWindow);
	}

	virtual bool exists() const
	{
		return mMainWindow != 0;
	}

	virtual void destroy()
	{
		if (!this->exists())
			return;

		delete mMainWindow;
	}

//private:
	QPointer<MAIN_WINDOW> mMainWindow;
};

} // namespace cx


#endif // CXMAINWINDOWAPPLICATIONCOMPONENT_H
