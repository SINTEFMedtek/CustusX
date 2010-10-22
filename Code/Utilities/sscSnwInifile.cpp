#include "sscSnwInifile.h"
#include <QFileInfo>
#include <QStringList>
#include <iostream>
#include "sscMD5Check.h"

namespace ssc
{

SonowandInifile::SonowandInifile(QString filename, CHECK_TYPE checkType) : 
	QSettings(filename, QSettings::IniFormat), 
	mCheckType(checkType),
	mCheckSuccess(true),
	mModified(false)
{
	QString md5file = filename + ".md5";
	
	if (mCheckType==CHECK_MD5 && QFileInfo(filename).exists() && QFileInfo(md5file).exists())
	{
		mCheckSuccess = CheckMD5(fileName().toAscii().constData());
	}
}

SonowandInifile::~SonowandInifile()
{
	sync();
}

void SonowandInifile::sync()
{
	QSettings::sync();
	writeChecksum();
}

bool SonowandInifile::checkOK() const 
{ 
	return mCheckSuccess; 
}

void SonowandInifile::clear()
{
	QSettings::clear();
	mModified = true;
}

void SonowandInifile::remove( const QString& key)
{
	QSettings::remove(key);
	mModified = true;
}

void SonowandInifile::setValue( const QString & key, const QVariant& value)	
{
	QSettings::setValue(key, value);
	mModified = true;
}

void SonowandInifile::writeChecksum()
{
	if (mCheckType==CHECK_MD5 && mModified && mCheckSuccess)
		GenerateMD5(fileName().toAscii().constData());
}

SonowandUTF8Inifile::SonowandUTF8Inifile(QString filename, CHECK_TYPE checkType) : 
	SonowandInifile(filename, checkType)
{
#if QT_VERSION >= 0x040500
	setIniCodec("UTF-8");
#endif
}

} // namespace pd
