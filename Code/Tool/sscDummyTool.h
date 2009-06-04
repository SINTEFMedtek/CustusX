#ifndef SSCDUMMYTOOL_H_
#define SSCDUMMYTOOL_H_

#include "sscTool.h"
#include <vector>
#include "sscBoundingBox3D.h"

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
	explicit DummyTool(const std::string& uid="dummytool");
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

	void setToolPositionMovement(const std::vector<Transform3D>& positions);
	void setToolPositionMovementBB(const DoubleBoundingBox3D& bb);
	void set_prMt(const Transform3D& ptMt);
	void setVisible(bool val);

private slots:
	virtual void sendTransform();

private:
	void createPolyData();
	std::vector<Transform3D> createToolPositionMovement(const DoubleBoundingBox3D& bb) const;
	Transform3D* getNextTransform();
	void createLinearMovement(std::vector<Transform3D>* retval, Transform3D* T_in, const Transform3D& R, const Vector3D& a, const Vector3D& b, double step) const;

	vtkPolyDataPtr mPolyData;

	bool mVisible;
	Transform3D m_prMt;
	std::string mTransformSaveFileName;
	QTimerPtr mTimer;
	std::vector<Transform3D> mTransforms;

	static int mTransformCount;
};
typedef boost::shared_ptr<DummyTool> DummyToolPtr;
}//namespace ssc
#endif /* SSCDUMMYTOOL_H_ */
