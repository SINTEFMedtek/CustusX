#ifndef CXTEXTEDITABLESTRINGDATAADAPTER_H_
#define CXTEXTEDITABLESTRINGDATAADAPTER_H_

#include "sscStringDataAdapter.h"
#include "sscXmlOptionItem.h"

namespace cx {

/*
 * \class TextEditableStringDataAdapter
 *
 * \brief Adapter for editable text
 *
 * \date Feb 1, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
typedef boost::shared_ptr<class TextEditableStringDataAdapterXml> TextEditableStringDataAdapterXmlPtr;
class TextEditableStringDataAdapterXml : public ssc::EditableStringDataAdapter
{
  Q_OBJECT

public:
	/** Make sure one given option exists witin root.
	 * If not present, fill inn the input defaults.
	 */
	static TextEditableStringDataAdapterXmlPtr initialize(const QString& uid, QString name, QString help, QString value,
		QStringList range, QDomNode root);

public:
  static TextEditableStringDataAdapterXmlPtr New() { return TextEditableStringDataAdapterXmlPtr(new TextEditableStringDataAdapterXml()); }
  TextEditableStringDataAdapterXml();
  virtual ~TextEditableStringDataAdapterXml() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

private:
  QString mText;
  XmlOptionItem mStore;
};
} /* namespace cx */
#endif /* CXTEXTEDITABLESTRINGDATAADAPTER_H_ */
