#ifndef TOOLTOOLIMPL_H_
#define TOOLTOOLIMPL_H_

#include "sscTool.h"
#include "Utilities.h"
#include "toolThreadSafeData.h"

// --------------------------------------------------------
namespace tool
{
// --------------------------------------------------------

class ToolImpl : public ssc::Tool
{
public:
	ToolImpl(std::string uid, ThreadSafeDataPtr data);
	virtual ~ToolImpl();
	virtual Type getType() const;
	virtual std::string getGraphicsFileName() const; 
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual void saveTransformsAndTimestamps();
	virtual void setTransformSaveFile(const std::string& filename); 
	virtual Transform3D getTransform() const;
	virtual bool getVisible() const;
	virtual std::string getUid() const; 
	virtual std::string getName() const;
	
	void invokeTransformEvent();
	void invokeVisibleEvent();
	
private:
	std::string mUid;
	ThreadSafeDataPtr mData;
};

typedef boost::shared_ptr<ToolImpl> ToolImplPtr;

// --------------------------------------------------------
} // namespace tool
// --------------------------------------------------------

#endif /*TOOLTOOLIMPL_H_*/
