/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXOPENIGTLINKGUIEXTENDERSERVICE_H
#define CXOPENIGTLINKGUIEXTENDERSERVICE_H

#include "org_custusx_core_openigtlink3_Export.h"
#include "cxGUIExtenderService.h"
class ctkPluginContext;

#include "igtlioLogic.h"

namespace cx
{
/*
typedef boost::shared_ptr<class NetworkServiceImpl> NetworkServiceImplPtr;
typedef boost::shared_ptr<class NetworkConnectionHandle> NetworkConnectionHandlePtr;
typedef boost::shared_ptr<class NetworkDataTransfer> NetworkDataTransferPtr;
class NetworkConnection;
*/

class org_custusx_core_openigtlink3_EXPORT OpenIGTLink3GuiExtenderService : public GUIExtenderService
{
public:
	OpenIGTLink3GuiExtenderService(ctkPluginContext* context, igtlio::LogicPointer logic);
    virtual ~OpenIGTLink3GuiExtenderService();

    std::vector<CategorizedWidget> createWidgets() const;
private:
    //mutable GUIExtenderService::CategorizedWidget mWidget;
    //NetworkServiceImplPtr mConnections;
//	NetworkConnectionHandlePtr mClient;
	ctkPluginContext* mContext;
    //NetworkDataTransferPtr mDataTransfer;
	igtlio::LogicPointer mLogic;
};
typedef boost::shared_ptr<OpenIGTLink3GuiExtenderService> OpenIGTLink3GuiExtenderServicePtr;

}
#endif //CXOPENIGTLINKGUIEXTENDERSERVICE_H
