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

/**Rewrite to be a descendant of PluginBase, with generic API for creating plugins...
 *
 */
class AlgorithmPlugin : public PluginBase
{
	Q_OBJECT
public:
	AlgorithmPlugin();
	virtual ~AlgorithmPlugin();

//  AcquisitionDataPtr getAcquisitionData() { return mAcquisitionData; }
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
