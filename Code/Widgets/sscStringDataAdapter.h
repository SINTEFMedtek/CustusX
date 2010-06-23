/*
 * sscStringDataAdapter.h
 *
 *  Created on: Jun 23, 2010
 *      Author: christiana
 */
#ifndef SSCSTRINGDATAADAPTER_H_
#define SSCSTRINGDATAADAPTER_H_

#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QComboBox>


namespace ssc
{

/** Abstract interface for interaction with internal string-valued data
 *
 * The class provides a bridge between general user interface code and specific
 * data structures. An implementation connects to a single data value.
 *
 *
 * Minimal implementation:
 *      virtual QString getValueName() const;
 *      virtual bool setValue(QString value);
 *      virtual QString getValue() const;
 *      void changed();
 * By implementing these methods you can set and get values, and the data has a name.
 * The changed() signal is used to make sure the user interface is updated even when
 * data is changed by some other source.
 *
 *
 * For more control use the methods:
 *      virtual QStringList getValueRange() const;
 *
 *
 * If there is a difference between the internal data representation and
 * how you want to present them, use:
 *      virtual QString convertInternal2Display(QString internal);
 *
 * When testing, or during development, you can use the StringDataAdapterNull
 * as a dummy implementation.
 *
 */
class StringDataAdapter : public QObject
{
  Q_OBJECT
public:
  virtual ~StringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const = 0;///< name of data entity. Used for display to user.
  virtual bool setValue(const QString& value) = 0; ///< set the data value.
  virtual QString getValue() const = 0; ///< get the data value.

public: // optional methods
  virtual QString getHelp() const { return QString(); } ///< return a descriptive help string for the data, used for example as a tool tip.
  virtual QStringList getValueRange() const { return QStringList(); } /// range of value. Use if data is constrained to a set.
  virtual QString convertInternal2Display(QString internal) { return internal; } ///< conversion from internal value to display value

  //  virtual void connectValueSignals(bool on) = 0; ///< set object to emit changed() when applicable

signals:
  void changed(); ///< emit when the underlying data value is changed: The user interface will be updated.
};
typedef boost::shared_ptr<StringDataAdapter> StringDataAdapterPtr;

/** dummy implementation */
class StringDataAdapterNull : public StringDataAdapter
{
  Q_OBJECT
public:
  virtual ~StringDataAdapterNull() {}
  virtual QString getValueName() const { return "dummy"; }
  virtual bool setValue(const QString& value) { return false; }
  virtual QString getValue() const { return ""; }
//  virtual void connectValueSignals(bool on) {}
};


} // namespace ssc

#endif /* SSCSTRINGDATAADAPTER_H_ */
