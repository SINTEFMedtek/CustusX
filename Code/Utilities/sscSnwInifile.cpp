#include "sscSnwInifile.h"
#include <QFileInfo>
#include <QStringList>
#include <iostream>
#include "sscMD5Check.h"

namespace ssc
{

/**
 * \brief Open \p filename as a ini file and check checksums if required
 * if \p checkType is set to CHECK_MD5 and the file exist and a corresponding .md5 file exist then the md5sum is verified.
 */
SonowandInifile::SonowandInifile(QString filename, CHECK_TYPE checkType) : 
	QSettings(filename, QSettings::IniFormat), 
	mCheckType(checkType),
	mCheckSuccess(true),
	mModified(false)
{
	if (mCheckType == CHECK_MD5)
	{
		QString md5file = filename + ".md5";
		mCheckSuccess = !QFile::exists(filename) || !QFile::exists(md5file) || CheckMD5(fileName().toAscii().constData());
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
	if (mCheckType == CHECK_MD5 && mModified && mCheckSuccess)
	{
		GenerateMD5(fileName().toAscii().constData());
	}
}

SonowandUTF8Inifile::SonowandUTF8Inifile(QString filename, CHECK_TYPE checkType) : 
	SonowandInifile(filename, checkType)
{
#if QT_VERSION >= 0x040500
	setIniCodec("UTF-8");
#endif
}

} // namespace pd
