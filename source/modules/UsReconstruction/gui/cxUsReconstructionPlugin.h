/*
 * cxUsReconstructionPlugin.h
 *
 *  \date Jun 15, 2011
 *      \author christiana
 */

#ifndef CXUSRECONSTRUCTIONPLUGIN_H_
#define CXUSRECONSTRUCTIONPLUGIN_H_

#include "cxPluginBase.h"

namespace cx
{
	typedef boost::shared_ptr<class ReconstructManager> ReconstructManagerPtr;
}

namespace cx
{

/**
 * \defgroup cx_module_usreconstruction UsReconstruction Plugin
 * \ingroup cx_modules
 * \brief Us Reconstruction framework with widgets.
 *
 * See \ref cx::UsReconstructionPlugin.
 *
 */

/**
 * \file
 * \addtogroup cx_module_usreconstruction
 * @{
 */

class UsReconstructionPlugin : public PluginBase
{
	Q_OBJECT
public:
	UsReconstructionPlugin();
	virtual ~UsReconstructionPlugin();

  ReconstructManagerPtr getReconstructer() { return mReconstructer; }
  std::vector<PluginWidget> createWidgets() const;

private:
  ReconstructManagerPtr mReconstructer;

private slots:
	void patientChangedSlot();

};

typedef boost::shared_ptr<UsReconstructionPlugin> UsReconstructionPluginPtr;

/**
 * @}
 */
}

#endif /* CXUSRECONSTRUCTIONPLUGIN_H_ */
