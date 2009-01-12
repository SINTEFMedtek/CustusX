#ifndef SSCDUMMYTOOL_H_
#define SSCDUMMYTOOL_H_

#include "sscTool.h"
#include <vector>

typedef vtkSmartPointer<class vtkConeSource> vtkConeSourcePtr;
typedef boost::shared_ptr<class QTimer> QTimerPtr;

/**
 * sscDummyTool.h
 *
 * \brief
 *
 * \date Oct 30, 2008
 * \author: jbake
 */
namespace ssc
{
class DummyTool : public Tool
{
	Q_OBJECT
public:
	DummyTool();
	~DummyTool();
	virtual Type getType() const;
	virtual std::string getGraphicsFileName() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual void saveTransformsAndTimestamps();
	virtual void setTransformSaveFile(const std::string& filename);
	virtual Transform3D get_prMt() const;
	virtual bool getVisible() const;
	virtual std::string getUid() const;
	virtual std::string getName() const;
	virtual int getIndex() const{return 0;};
	virtual bool isCalibrated() const;
	void startTracking();
	void stopTracking();

private slots:
	virtual void sendTransform();

private:
	void createPolyData();
	std::vector<Transform3D> createToolPositionMovement() const;
	Transform3D* getNextTransform();

	vtkPolyDataPtr mPolyData;

	bool mVisible;
	std::string mTransformSaveFileName;
	QTimerPtr mTimer;
	std::vector<Transform3D> mTransforms;

	static int mTransformCount;
};
typedef boost::shared_ptr<DummyTool> DummyToolPtr;
}//namespace ssc
#endif /* SSCDUMMYTOOL_H_ */
