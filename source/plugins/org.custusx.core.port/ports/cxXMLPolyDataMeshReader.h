#ifndef CXXMLPOLYDATAMESHREADER_H
#define CXXMLPOLYDATAMESHREADER_H

#include "cxPortService.h"
#include "org_custusx_core_port_Export.h"

class ctkPluginContext;

namespace cx
{

/**\brief Reader for .vtp files.
 *
 */
class org_custusx_core_port_EXPORT XMLPolyDataMeshReader: public FileReaderWriterService
{
public:
	Q_INTERFACES(cx::FileReaderWriterService)

	XMLPolyDataMeshReader();
	virtual ~XMLPolyDataMeshReader(){}
	virtual bool canLoad(const QString& type, const QString& filename);
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(MeshPtr mesh, QString filename);

	virtual vtkPolyDataPtr loadVtkPolyData(QString filename);
	virtual QString canLoadDataType() const { return "mesh"; }
	virtual DataPtr load(const QString& uid, const QString& filename);

	bool isNull(){ return false;}
};
}

#endif // CXXMLPOLYDATAMESHREADER_H