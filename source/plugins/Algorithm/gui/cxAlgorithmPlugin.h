/*
 * cxAlgorithmPlugin.h
 *
 *  \date Jun 15, 2011
 *      \author christiana
 */

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

typedef boost::shared_ptr<class AlgorithmPlugin> AlgorithmPluginPtr;


class AlgorithmPlugin : public PluginBase
{
	Q_OBJECT
public:
	AlgorithmPlugin();
	virtual ~AlgorithmPlugin();

	virtual std::vector<PluginWidget> createWidgets() const;

signals:

private slots:

private:
};

/**
 * @}
 */
}

#endif /* CXALGORITHMPLUGIN_H_ */
