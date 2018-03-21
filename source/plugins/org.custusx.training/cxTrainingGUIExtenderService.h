/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRAININGGUIEXTENDERSERVICE_H_
#define CXTRAININGGUIEXTENDERSERVICE_H_

#include "cxGUIExtenderService.h"
#include "org_custusx_training_Export.h"
class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class TrainingEngine> TrainingEnginePtr;

/**
 * Implementation of Training service.
 *
 * \ingroup org_custusx_training
 *
 * \date 2016-03-14
 * \author Christian Askeland
 */
class org_custusx_training_EXPORT TrainingGUIExtenderService : public GUIExtenderService
{
	Q_INTERFACES(cx::GUIExtenderService)
public:
	TrainingGUIExtenderService(ctkPluginContext *context);
	virtual ~TrainingGUIExtenderService();

	virtual std::vector<CategorizedWidget> createWidgets() const;

private:
  ctkPluginContext* mContext;
};
typedef boost::shared_ptr<TrainingGUIExtenderService> TrainingGUIExtenderServicePtr;

} /* namespace cx */

#endif /* CXTRAININGGUIEXTENDERSERVICE_H_ */
