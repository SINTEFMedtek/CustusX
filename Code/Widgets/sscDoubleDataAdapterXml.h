/*
 * sscDoubleDataAdapterXml.h
 *
 *  Created on: Jun 27, 2010
 *      Author: christiana
 */

#ifndef SSCDOUBLEDATAADAPTERXML_H_
#define SSCDOUBLEDATAADAPTERXML_H_

#include <QDomElement>
#include <QStringList>
#include "sscDoubleRange.h"
#include "sscDoubleDataAdapter.h"


namespace ssc
{

typedef boost::shared_ptr<class DoubleDataAdapterXml> DoubleDataAdapterXmlPtr;

/** Represents one option of the double type.
 *  The data are stored within a xml document.
 *
 *  The option node has this layout:
 *   <option id="Processor" value="3.14"/>
 */
class DoubleDataAdapterXml : public DoubleDataAdapter
{
	Q_OBJECT
public:
  /** Make sure one given option exists witin root.
   * If not present, fill inn the input defaults.
   */
  static DoubleDataAdapterXmlPtr initialize(const QString& uid,
      QString name,
      QString help,
      double value,
      DoubleRange range,
      int decimals,
      QDomNode root);

public: // inherited interface
  virtual QString getValueName() const;///< name of data entity. Used for display to user.
  virtual bool setValue(double value); ///< set the data value.
  virtual double getValue() const; ///< get the data value.
  virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.
  virtual DoubleRange getValueRange() const; /// range of value. Use if data is constrained to a set.
  virtual int getValueDecimals() const; ///< number of relevant decimals in value

public:
  QString getUid() const;

signals:
    void valueWasSet(); /// emitted when the value is set using setValue() (similar to changed(), but more constrained)

private:
  QString mName;
  QString mUid;
  QString mHelp;
  double mValue;
  double mDecimals;
  DoubleRange mRange;
  QDomElement mRoot;

  QDomElement findElemFromUid(const QString& uid, QDomNode root);
  void writeValue(const QString& val);
};


} // namespace ssc

#endif /* SSCDOUBLEDATAADAPTERXML_H_ */
