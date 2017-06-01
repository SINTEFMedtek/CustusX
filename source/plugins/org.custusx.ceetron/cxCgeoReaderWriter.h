#ifndef CXCGEOREADERWRITER_H
#define CXCGEOREADERWRITER_H

#include "org_custusx_ceetron_Export.h"
#include "cxFileReaderWriterService.h"

namespace cx
{
/**
 * @brief Write triangular mesh to .cgeo format for Ceetron.
 *
 * \date May 19, 2017
 * \author Erlend F Hofstad
 */

class org_custusx_ceetron_EXPORT CgeoReaderWriter : public FileReaderWriterService
{
public:
	Q_INTERFACES(cx::FileReaderWriterService)

	CgeoReaderWriter();

	bool isNull();
	bool canLoad(const QString &type, const QString &filename);
	DataPtr load(const QString &uid, const QString &filename);
	QString canLoadDataType() const;
	bool readInto(DataPtr data, QString path);
	void save(DataPtr data, const QString &filename);
};

}
#endif // CXCGEOREADERWRITER_H