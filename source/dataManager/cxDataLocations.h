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

class DataLocations
{
public:
  /** return path to the root config folder
   */
  static QString getConfigPath() const;
  QSettingsPtr getSettings() const;
//  {
//
//  }

};

} // namespace cx

#endif /* CXDATALOCATIONS_H_ */
