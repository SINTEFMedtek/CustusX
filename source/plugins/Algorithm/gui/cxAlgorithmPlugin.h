#ifndef CXALGORITHMPLUGIN_H_
#define CXALGORITHMPLUGIN_H_

#include "cxPluginBase.h"

namespace cx
{
/**
 * \defgroup cx_plugin_algorithm Algorithm Plugin
 * \ingroup cx_plugins
 * \brief Algorithm collection with widgets.
 *
 * See \ref cx::AlgorithmPlugin.
 *
 */

/**
 * \file
 * \addtogroup cx_plugin_algorithm
 * @{
 */

/**
 * \class AlgorithmPlugin
 *
 * \date Jun 15, 2011
 * \author christiana
 */
class AlgorithmPlugin : public PluginBase
{
	Q_OBJECT
public:
	AlgorithmPlugin();
	virtual ~AlgorithmPlugin();

	virtual std::vector<PluginWidget> createWidgets() const;
};
typedef boost::shared_ptr<class AlgorithmPlugin> AlgorithmPluginPtr;

/**
 * @}
 */
}

#endif /* CXALGORITHMPLUGIN_H_ */
