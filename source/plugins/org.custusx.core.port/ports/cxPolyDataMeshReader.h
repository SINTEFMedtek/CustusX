#ifndef CXPOLYDATAMESHREADER_H
#define CXPOLYDATAMESHREADER_H

#include "cxPortService.h"

class ctkPluginContext;

namespace cx {

/**\brief Reader for .vtk files.
 *
 */
class PolyDataMeshReader: public PortService
{
public:
	Q_INTERFACES(cx::PortService)

	PolyDataMeshReader(ctkPluginContext *context);
	virtual ~PolyDataMeshReader(){}
	virtual bool canLoad(const QString& type, const QString& filename);
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(MeshPtr mesh, QString filename);

	virtual vtkPolyDataPtr loadVtkPolyData(QString filename);
	virtual QString canLoadDataType() const { return "mesh"; }
	virtual DataPtr load(const QString& uid, const QString& filename);

	bool isNull(){ return false; }
};

}

#endif // CXPOLYDATAMESHREADER_H