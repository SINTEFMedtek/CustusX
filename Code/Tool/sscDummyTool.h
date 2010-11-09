#ifndef SSCDUMMYTOOL_H_
#define SSCDUMMYTOOL_H_

#include "sscTool.h"
#include <vector>
#include "sscBoundingBox3D.h"
#include "vtkForwardDeclarations.h"

typedef boost::shared_ptr<class QTimer> QTimerPtr;

/**
 * \class DummyTool
 *
 * \brief
 *
 * \date Oct 30, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
namespace ssc
{
class DummyTool : public Tool
{
	Q_OBJECT
public:
	explicit DummyTool(const QString& uid="dummytool");
	~DummyTool();
	virtual Type getType() const;
	virtual QString getGraphicsFileName() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual void saveTransformsAndTimestamps();
	virtual void setTransformSaveFile(const QString& filename);
	virtual Transform3D get_prMt() const;
	virtual bool getVisible() const;
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual int getIndex() const{return 0;};
	virtual bool isCalibrated() const;
	virtual ProbeSector getProbeSector() const { return ProbeSector(); }	
	virtual double getTimestamp() const { return 0; }

	void startTracking(int interval=33);
	void stopTracking();

	void setToolPositionMovement(const std::vector<Transform3D>& positions);
	void setToolPositionMovementBB(const DoubleBoundingBox3D& bb);
	std::vector<Transform3D> createToolPositionMovementTranslationOnly(const DoubleBoundingBox3D& bb) const;
	std::vector<Transform3D> getToolPositionMovement();
	void set_prMt(const Transform3D& ptMt);
	void setVisible(bool val);

	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);	
	virtual Transform3D getCalibration_sMt() const;

	virtual ssc::Vector3D getReferencePoint() const {return ssc::Vector3D();};

	static vtkPolyDataPtr createPolyData(double h1, double h2, double r1, double r2);	
	
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
	QString mTransformSaveFileName;
	QTimerPtr mTimer;
	double mOffset;
	std::vector<Transform3D> mTransforms;

	static int mTransformCount;
};
typedef boost::shared_ptr<DummyTool> DummyToolPtr;
}//namespace ssc
#endif /* SSCDUMMYTOOL_H_ */
