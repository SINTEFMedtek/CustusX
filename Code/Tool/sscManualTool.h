#ifndef SSCMANUALTOOL_H_
#define SSCMANUALTOOL_H_

#include <QMutex>
#include "sscTool.h"

namespace ssc
{

/**A manual tool that is unconnected to any hardware.
 * Use this tool by setting the functions setTransform() and setVisible().
 * 
 * This class is Threadsafe.
 */
class ManualTool : public ssc::Tool
{
	Q_OBJECT
public:
	
	explicit ManualTool(const std::string& uid);
	virtual ~ManualTool();
	virtual Type getType() const;
	virtual std::string getGraphicsFileName() const; 
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual void saveTransformsAndTimestamps(){};
	virtual void setTransformSaveFile(const std::string& filename){}; 
	virtual Transform3D get_prMt() const;
	virtual bool getVisible() const;
	virtual std::string getUid() const; 
	virtual std::string getName() const;
	virtual int getIndex() const;
	virtual bool isCalibrated() const;
	virtual ssc::ProbeSector getProbeSector() const;	
	virtual double getTimestamp() const;
	
	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);	

	// extensions:
	void set_prMt(const Transform3D& prMt);
	void setVisible(bool vis);
	void setType(const Type& type);

private:
	// constant data
	const std::string mUid;
	const std::string mName;
	vtkPolyDataPtr mPolyData;
	double mOffset;

	// mutex-protected mutable data
	Type mType;
	bool mVisible; 	
	Transform3D m_prMt;
	mutable QMutex mMutex;

//	void createPolyData();
};

typedef boost::shared_ptr<ManualTool> ManualToolPtr;

}// end namespace

#endif /*SSCMANULATOOL_H_*/
