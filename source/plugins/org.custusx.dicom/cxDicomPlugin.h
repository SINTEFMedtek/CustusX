/*
 * cxDicomPlugin.h
 *
 *  Created on: May 1, 2014
 *      Author: christiana
 */

#ifndef CXDICOMPLUGIN_H_
#define CXDICOMPLUGIN_H_

#include "cxPluginBase.h"
#include "org_custusx_dicom_Export.h"
//using cx::PluginBase;

namespace cx
{

class org_custusx_dicom_EXPORT DicomPlugin : public PluginBase
{
	//Q_INTERFACES(cx::PluginBase)
	Q_INTERFACES(PluginBase)
public:
	DicomPlugin();
	virtual ~DicomPlugin();

	std::vector<PluginWidget> createWidgets() const;

};
typedef boost::shared_ptr<DicomPlugin> DicomPluginPtr;

} /* namespace cx */
#endif /* CXDICOMPLUGIN_H_ */

