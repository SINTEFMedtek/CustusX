// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXREGISTRATIONPLUGIN_H_
#define CXREGISTRATIONPLUGIN_H_

#include "cxPluginBase.h"

/**
 * \defgroup cxPluginRegistration Registration Plugin
 * \ingroup cxPlugins
 * \brief Registration collection with widgets.
 *
 * See \ref cx::RegistrationPlugin.
 *
 */

namespace cx
{
typedef boost::shared_ptr<class RegistrationPlugin> RegistrationPluginPtr;
typedef boost::shared_ptr<class RegistrationManager> RegistrationManagerPtr;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;

/**
 * \file
 * \addtogroup cxPluginRegistration
 * @{
 */

/**Rewrite to be a descendant of PluginBase, with generic API for creating plugins...
 *
 *  \date Jun 16, 2011
 *  \author christiana
 */
class RegistrationPlugin : public PluginBase
{
	Q_OBJECT
public:
	RegistrationPlugin(AcquisitionDataPtr acquisitionData);
	virtual ~RegistrationPlugin() {}

//  AcquisitionDataPtr getAcquisitionData() { return mAcquisitionData; }
	virtual std::vector<PluginWidget> createWidgets() const;

signals:

private slots:

private:
	RegistrationManagerPtr mRegistrationManager;
};

/**
 * @}
 */
}

#endif /* CXREGISTRATIONPLUGIN_H_ */
