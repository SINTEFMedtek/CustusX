/*
 * cxAcquisitionPlugin.h
 *
 *  \date Jun 15, 2011
 *      \author christiana
 */

#ifndef CXACQUISITIONPLUGIN_H_
#define CXACQUISITIONPLUGIN_H_

#include "cxPluginBase.h"
class QDomNode;

namespace cx
{
typedef boost::shared_ptr<class ReconstructManager> ReconstructManagerPtr;
}

/**
 * \defgroup cx_plugin_acquisition Acquisition Plugin
 * \ingroup cx_plugins
 * \brief Handles the us acquisition process.
 *
 * See \ref cx::USAcquisition.
 * See \ref cx::AcquisitionPlugin.
 *
 */

namespace cx
{
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;
typedef boost::shared_ptr<class AcquisitionPlugin> AcquisitionPluginPtr;


/**
* \file
* \addtogroup cx_plugin_acquisition
* @{
*/

/**Rewrite to be a descendant of PluginBase, with generic API for creating plugins...
 *
 */
class AcquisitionPlugin: public PluginBase
{
Q_OBJECT
public:
	AcquisitionPlugin(ReconstructManagerPtr reconstructer);
	virtual ~AcquisitionPlugin();

	AcquisitionDataPtr getAcquisitionData()
	{
		return mAcquisitionData;
	}
	virtual std::vector<PluginWidget> createWidgets() const;

signals:

private slots:
	void clearSlot();
	void duringSavePatientSlot();
	void duringLoadPatientSlot();

private:
	//Interface for saving/loading
	void addXml(QDomNode& dataNode); ///< adds xml information about the StateService and its variabels
	void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of the StateService.

	AcquisitionDataPtr mAcquisitionData;
};

/**
* @}
*/
}

#endif /* CXACQUISITIONPLUGIN_H_ */
