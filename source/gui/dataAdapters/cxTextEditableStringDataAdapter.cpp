#include "cxTextEditableStringDataAdapter.h"

namespace cx {

TextEditableStringDataAdapterXml::TextEditableStringDataAdapterXml() :
		mText("")
{}

QString TextEditableStringDataAdapterXml::getValueName() const
{
  return "Text";
}

bool TextEditableStringDataAdapterXml::setValue(const QString& value)
{
  if(mText.isEmpty() && (mText.compare(value) == 0))
	return false;
  mText = value;
  emit changed();
  return true;
}

QString TextEditableStringDataAdapterXml::getValue() const
{
  if(!mText.isEmpty())
	return mText;
  return "";
}

} /* namespace cx */
