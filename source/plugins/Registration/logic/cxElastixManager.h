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
#ifndef CXELASTIXMANAGER_H_
#define CXELASTIXMANAGER_H_

#include <QObject>
#include "sscForwardDeclarations.h"
#include "sscXmlOptionItem.h"
#include "cxRegistrationManager.h"
#include "sscBoolDataAdapterXml.h"
#include "sscStringDataAdapterXml.h"
#include "cxElastixParameters.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginRegistration
 * @{
 */

typedef boost::shared_ptr<class ElastixExecuter> ElastixExecuterPtr;

/**
 * \brief Manager for interfacing to the ElastiX registration package.
 *
 * \date Feb 4, 2012
 * \author Christian Askeland, SINTEF
 */
class ElastixManager : public QObject
{
	Q_OBJECT
public:
	ElastixManager(RegistrationManagerPtr regManager);
	virtual ~ElastixManager();

	ssc::BoolDataAdapterXmlPtr getDisplayProcessMessages() { return mDisplayProcessMessages; }
	ElastixExecuterPtr getExecuter() { return mExecuter; }
	ElastixParametersPtr getParameters() { return mParameters; }

	void execute();
signals:
	void elastixChanged();

private slots:
	void executionFinishedSlot();
	void preprocessExecuter();

private:
	void addNonlinearData();

	RegistrationManagerPtr mRegistrationManager;
	ElastixParametersPtr mParameters;
	ssc::XmlOptionFile mOptions;
	ssc::BoolDataAdapterXmlPtr mDisplayProcessMessages;
	ElastixExecuterPtr mExecuter;
};
typedef boost::shared_ptr<ElastixManager> ElastixManagerPtr;

/**
 * @}
 */
} /* namespace cx */
#endif /* CXELASTIXMANAGER_H_ */
