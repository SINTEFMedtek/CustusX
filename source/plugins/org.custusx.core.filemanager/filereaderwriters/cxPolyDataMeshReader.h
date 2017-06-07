#ifndef CXPOLYDATAMESHREADER_H
#define CXPOLYDATAMESHREADER_H

#include "cxFileReaderWriterService.h"
#include "org_custusx_core_filemanager_Export.h"

class ctkPluginContext;

namespace cx {

/**\brief Reader for .vtk files.
 *
 */
class org_custusx_core_filemanager_EXPORT PolyDataMeshReader: public FileReaderWriterImplService
{
public:
	Q_INTERFACES(cx::FileReaderWriterService)

	PolyDataMeshReader();
	virtual ~PolyDataMeshReader(){}

	bool isNull(){ return false; }

	virtual bool canRead(const QString& type, const QString& filename);
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(MeshPtr mesh, QString filename);

	virtual vtkPolyDataPtr loadVtkPolyData(QString filename);
	virtual QString canReadDataType() const { return "mesh"; }
	virtual DataPtr read(const QString& uid, const QString& filename);

	virtual void write(DataPtr data, const QString& filename);
	QString canWriteDataType() const;
	bool canWrite(const QString &type, const QString &filename) const;
};

}

#endif // CXPOLYDATAMESHREADER_H