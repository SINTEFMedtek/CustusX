/*
 * cxAcquisitionPlugin.h
 *
 *  Created on: Jun 15, 2011
 *      Author: christiana
 */

#ifndef CXACQUISITIONPLUGIN_H_
#define CXACQUISITIONPLUGIN_H_

#include "cxPluginBase.h"
class QDomNode;

namespace ssc
{
	typedef boost::shared_ptr<class Reconstructer> ReconstructerPtr;
}

namespace cx
{
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;
typedef boost::shared_ptr<class AcquisitionPlugin> AcquisitionPluginPtr;

/**Rewrite to be a descendant of PluginBase, with generic API for creating plugins...
 *
 */
class AcquisitionPlugin : public PluginBase
{
	Q_OBJECT
public:
	AcquisitionPlugin(ssc::ReconstructerPtr reconstructer);
	virtual ~AcquisitionPlugin();

  AcquisitionDataPtr getAcquisitionData() { return mAcquisitionData; }
	virtual std::vector<PluginWidget> createWidgets() const;

signals:

private slots:
	void clearSlot();
	void duringSavePatientSlot();
	void duringLoadPatientSlot();

private:
  //Interface for saving/loading
  void addXml(QDomNode& dataNode); ///< adds xml information about the StateManager and its variabels
  void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of the StateManager.

  AcquisitionDataPtr mAcquisitionData;
};

}

#endif /* CXACQUISITIONPLUGIN_H_ */
