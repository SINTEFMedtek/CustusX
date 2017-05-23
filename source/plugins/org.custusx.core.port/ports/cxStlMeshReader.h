#ifndef CXSTLMESHREADER_H
#define CXSTLMESHREADER_H

#include "cxPortService.h"

class ctkPluginContext;
namespace cx
{
/**\brief Reader for STL files.
 *
 */
class StlMeshReader: public PortService
{
public:
	Q_INTERFACES(cx::PortService)

	StlMeshReader(ctkPluginContext *context);
	virtual ~StlMeshReader(){}
	virtual bool canLoad(const QString& type, const QString& filename);
	virtual bool readInto(DataPtr data, QString path);
	bool readInto(MeshPtr mesh, QString filename);
	virtual vtkPolyDataPtr loadVtkPolyData(QString filename);
	virtual QString canLoadDataType() const { return "mesh"; }
	virtual DataPtr load(const QString& uid, const QString& filename);

	bool isNull(){return false;}
};
}

#endif // CXSTLMESHREADER_H