/*
 * sscBoolDataAdapterXml.h
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#ifndef SSCBOOLDATAADAPTERXML_H_
#define SSCBOOLDATAADAPTERXML_H_

#include <QDomElement>

#include "sscBoolDataAdapter.h"
#include "sscXmlOptionItem.h"

namespace ssc
{

typedef boost::shared_ptr<class BoolDataAdapterXml> BoolDataAdapterXmlPtr;

class BoolDataAdapterXml : public BoolDataAdapter
{
  Q_OBJECT
public:
  virtual ~BoolDataAdapterXml() {}

  /** Make sure one given option exists witin root.
   * If not present, fill inn the input defaults.
   */
  static BoolDataAdapterXmlPtr initialize(const QString& uid,
      QString name,
      QString help,
      bool value,
      QDomNode root = QDomNode());

public: // basic methods
  virtual QString getValueName() const; ///< name of data entity. Used for display to user.
  virtual bool setValue(bool value); ///< set the data value.
  virtual bool getValue() const; ///< get the data value.

public: // optional methods
  virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.
  QString getUid() const;

signals:
    void valueWasSet(); /// emitted when the value is set using setValue() (similar to changed(), but more constrained)

private:
  QString mName;
  QString mUid;
  QString mHelp;
  bool mValue;
  XmlOptionItem mStore;

};

}



#endif /* SSCBOOLDATAADAPTERXML_H_ */
