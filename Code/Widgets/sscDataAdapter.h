/*
 * sscDataAdapter.h
 *
 *  Created on: Jun 27, 2010
 *      Author: christiana
 */
#ifndef SSCDATAADAPTER_H_
#define SSCDATAADAPTER_H_

#include <boost/shared_ptr.hpp>
#include <QString>
#include <QObject>

class DataAdapter : public QObject
{
 Q_OBJECT
public:
 virtual ~DataAdapter() {}

public: // basic methods
 virtual QString getValueName() const = 0; ///< name of data entity. Used for display to user.
public: // optional methods

 signals:
 void changed(); ///< emit when the underlying data value is changed: The user interface will be updated.
};
typedef boost::shared_ptr<DataAdapter> DataAdapterPtr;


#endif /* SSCDATAADAPTER_H_ */
