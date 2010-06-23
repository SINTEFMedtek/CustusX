/*
 * cxDataLocations.h
 *
 *  Created on: Jun 22, 2010
 *      Author: christiana
 */
#ifndef CXDATALOCATIONS_H_
#define CXDATALOCATIONS_H_

#include <QString>
#include "boost/shared_ptr.hpp"
typedef boost::shared_ptr<class QSettings> QSettingsPtr;

namespace cx
{

/**Helper functions for accessing file paths and similar stuff.
 *
 */
class DataLocations
{
public:
  static QString getBundlePath(); ///< return the folder where the bundle or executable are located.
  static QString getConfigPath(); ///< return path to root config folder
  static QString getShaderPath(); ///< return the path to installed shaders. empty string if not installed.
  static QString getAppDataPath(); ///< return path to global configuration data.
  static QSettingsPtr getSettings(); ///< return a settings object for global custusX data
};

} // namespace cx

#endif /* CXDATALOCATIONS_H_ */
