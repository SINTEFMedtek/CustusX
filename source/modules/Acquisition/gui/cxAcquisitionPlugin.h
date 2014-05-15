/*
 * cxAcquisitionPlugin.h
 *
 *  \date Jun 15, 2011
 *      \author christiana
 */

#ifndef CXACQUISITIONPLUGIN_H_
#define CXACQUISITIONPLUGIN_H_

#include "cxGUIExtenderService.h"
class QDomNode;

namespace cx
{
typedef boost::shared_ptr<class ReconstructionManager> ReconstructManagerPtr;
}

/**
 * \defgroup cx_module_acquisition Acquisition Plugin
 * \ingroup cx_modules
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
* \addtogroup cx_module_acquisition
* @{
*/

/**
 *
 */
class AcquisitionPlugin: public GUIExtenderService
{
Q_OBJECT
public:
	AcquisitionPlugin(ReconstructManagerPtr reconstructer);
	virtual ~AcquisitionPlugin();

	AcquisitionDataPtr getAcquisitionData()
	{
		return mAcquisitionData;
	}
	virtual std::vector<CategorizedWidget> createWidgets() const;

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
