#ifndef CXMNIREADERWRITER_H
#define CXMNIREADERWRITER_H

#include "org_custusx_core_filemanager_Export.h"
#include "cxFileReaderWriterService.h"
#include "cxForwardDeclarations.h"
class QColor;
class ctkPluginContext;

namespace cx
{

class org_custusx_core_filemanager_EXPORT MNIReaderWriter : public FileReaderWriterImplService

{
public:
	MNIReaderWriter(ctkPluginContext *context);

	bool isNull();
	QString canReadDataType() const;
	bool canRead(const QString &type, const QString &filename);
	DataPtr read(const QString &uid, const QString &filename); ///< WARNING: will produce a popup dialog
	bool readInto(DataPtr data, QString path);

	QString canWriteDataType() const;
	bool canWrite(const QString &type, const QString &filename) const;
	void write(DataPtr data, const QString &filename);

private:
	QColor getRandomColor();
	std::vector<QString> dialogForSelectingVolumesForImportedMNITagFile( int number_of_volumes, QString description);
	DataPtr createData(QString type, QString uid, QString name);

	PatientModelServicePtr mPatientModelService;
	ViewServicePtr mViewService;
};
}

#endif // CXMNIREADERWRITER_H