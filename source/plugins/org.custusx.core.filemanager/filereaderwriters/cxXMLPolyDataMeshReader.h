#ifndef CXXMLPOLYDATAMESHREADER_H
#define CXXMLPOLYDATAMESHREADER_H

#include "cxFileReaderWriterService.h"
#include "org_custusx_core_filemanager_Export.h"

class ctkPluginContext;

namespace cx
{

/**\brief Reader for .vtp files.
 *
 */
class org_custusx_core_filemanager_EXPORT XMLPolyDataMeshReader: public FileReaderWriterImplService
{
public:
	Q_INTERFACES(cx::FileReaderWriterService)

	XMLPolyDataMeshReader(ctkPluginContext *context);
	virtual ~XMLPolyDataMeshReader(){}

	bool isNull(){ return false;}

	virtual bool canRead(const QString& type, const QString& filename);
	virtual DataPtr read(const QString& uid, const QString& filename);
	virtual QString canReadDataType() const { return "mesh"; }
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(MeshPtr mesh, QString filename);
	std::vector<DataPtr> read(const QString &filename);

	void write(DataPtr data, const QString &filename) {}
	QString canWriteDataType() const;
	bool canWrite(const QString &type, const QString &filename) const;

	virtual vtkPolyDataPtr loadVtkPolyData(QString filename);


};

}

#endif // CXXMLPOLYDATAMESHREADER_H