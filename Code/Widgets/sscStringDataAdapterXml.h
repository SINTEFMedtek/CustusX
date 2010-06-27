/*
 * sscStringDataAdapterXml.h
 *
 *  Created on: Jun 27, 2010
 *      Author: christiana
 */

#ifndef SSCSTRINGDATAADAPTERXML_H_
#define SSCSTRINGDATAADAPTERXML_H_

#include <QDomElement>
#include <QStringList>
#include "sscStringDataAdapter.h"

namespace ssc
{

typedef boost::shared_ptr<class StringDataAdapterXml> StringDataAdapterXmlPtr;

/** Represents one option of the string type.
 *  The data are stored within a xml document.
 *
 *  The option node has this layout:
 *   <option id="Processor" value="3.14"/>
 */
class StringDataAdapterXml : public StringDataAdapter
{
	Q_OBJECT
public:
  /** find and return the setting with id==uid among the children of root.
   */
//  static StringOptionItem fromName(const QString& uid, QDomNode root);

  /** Make sure one given option exists witin root.
   * If not present, fill inn the input defaults.
   */
  static StringDataAdapterXmlPtr initialize(const QString& uid,
      QString name,
      QString help,
      QString value,
      QStringList range,
      QDomNode root);

public: // inherited interface
  virtual QString getValueName() const;///< name of data entity. Used for display to user.
  virtual bool setValue(const QString& value); ///< set the data value.
  virtual QString getValue() const; ///< get the data value.
  virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.
  virtual QStringList getValueRange() const; /// range of value. Use if data is constrained to a set.

public:
  QString getUid() const;

signals:
    void valueWasSet(); /// emitted when the value is set using setValue() (similar to changed(), but more constrained)

private:
  QString mName;
  QString mUid;
  QString mHelp;
  QString mValue;
  QStringList mRange;
  QDomElement mRoot;

  QDomElement findElemFromUid(const QString& uid, QDomNode root);
  void writeValue(const QString& val);
};

// --------------------------------------------------------
// --------------------------------------------------------


} // namespace ssc

#endif /* SSCSTRINGDATAADAPTERXML_H_ */
