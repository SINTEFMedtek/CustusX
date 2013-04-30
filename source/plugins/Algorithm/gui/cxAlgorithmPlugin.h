#ifndef CXALGORITHMPLUGIN_H_
#define CXALGORITHMPLUGIN_H_

#include "cxPluginBase.h"

namespace cx
{
/**
 * \defgroup cxPluginAlgorithm Algorithm Plugin
 * \ingroup cxPlugins
 * \brief Algorithm collection with widgets.
 *
 * See \ref cx::AlgorithmPlugin.
 *
 */

/**
 * \file
 * \addtogroup cxPluginAlgorithm
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
