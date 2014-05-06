/*
 * cxPluginFrameworkUtilities.h
 *
 *  Created on: May 3, 2014
 *      Author: christiana
 */

#ifndef CXPLUGINFRAMEWORKUTILITIES_H_
#define CXPLUGINFRAMEWORKUTILITIES_H_

#include <ctkPlugin.h>

namespace cx
{


QString getStringForctkPluginState(const ctkPlugin::State state);
ctkPlugin::State getctkPluginStateForString(QString text);

} /* namespace cx */
#endif /* CXPLUGINFRAMEWORKUTILITIES_H_ */
