/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXUSRECONSTRUCTIONGUIEXTENDERSERVICE_H_
#define CXUSRECONSTRUCTIONGUIEXTENDERSERVICE_H_

#include "org_custusx_usreconstruction_Export.h"

#include "cxGUIExtenderService.h"

namespace cx
{
//typedef boost::shared_ptr<class ReconstructionManager> ReconstructManagerPtr;
typedef boost::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;

/**
 * \brief Us Reconstruction framework with widgets.
 *
 * \ingroup org_custusx_usreconstruction
 *
 */
class org_custusx_usreconstruction_EXPORT UsReconstructionGUIExtenderService : public GUIExtenderService
{
	Q_INTERFACES(cx::GUIExtenderService)
public:
	UsReconstructionGUIExtenderService(UsReconstructionServicePtr usReconstructionService, PatientModelServicePtr patientModelService);
	virtual ~UsReconstructionGUIExtenderService();

  UsReconstructionServicePtr getReconstructer() { return mReconstructer; }
  std::vector<CategorizedWidget> createWidgets() const;

private:
  UsReconstructionServicePtr mReconstructer;

};

//typedef boost::shared_ptr<UsReconstructionPlugin> UsReconstructionPluginPtr;

}//cx

#endif /* CXUSRECONSTRUCTIONGUIEXTENDERSERVICE_H_ */
