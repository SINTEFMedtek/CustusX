#ifndef SSCSNWINIFILE_H_
#define SSCSNWINIFILE_H_

#include <QSettings>

namespace ssc
{

/**\brief Custom ini file for Sonowand.
 *
 * Extension of the QSettings class.
 * 
 * -Always of type inifile.
 * -handles the md5 checksum (optional)
 *
 * \ingroup sscUtility
 */
class SonowandInifile : public QSettings
{
public:
	enum CHECK_TYPE
	{
		CHECK_MD5,
		NO_CHECK
	};

	SonowandInifile(QString filename, CHECK_TYPE checkType=CHECK_MD5);
	~SonowandInifile();
	void sync();
	bool checkOK() const; 
	void clear();
	void remove( const QString& key);
	void setValue( const QString & key, const QVariant& value);

private:
	void writeChecksum();

	CHECK_TYPE mCheckType;
	bool mCheckSuccess;
	bool mModified;
};

class SonowandUTF8Inifile : public SonowandInifile
{
public:
	SonowandUTF8Inifile(QString filename, CHECK_TYPE checkType=NO_CHECK);
};

}

#endif /*PDINIFILE_H_*/
