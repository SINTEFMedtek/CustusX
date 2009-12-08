#ifndef SSCDISPLAYTEXTREP_H_
#define SSCDISPLAYTEXTREP_H_

#include <vector>
#include "sscRepImpl.h"
#include "sscVtkHelperClasses.h"

namespace ssc
{

typedef boost::shared_ptr<class DisplayTextRep> DisplayTextRepPtr;

class DisplayTextRep : public ssc::RepImpl
{
public:
	static DisplayTextRepPtr New(const std::string& uid, const std::string& name);
	virtual std::string getType() const{ return "vm::DisplayTextRep";};
	void addText(const Vector3D& color, const std::string& text, const Vector3D& pos);
	void setText(unsigned i, const std::string& text);
	void setColor(const Vector3D& color );
	virtual ~DisplayTextRep();
protected:
	DisplayTextRep(const std::string& uid, const std::string& name);
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);
	
	std::vector<TextDisplayPtr> mDisplayText;

	TextDisplayPtr mPictString;
};



}//end namespace

#endif /*SSCDISPLAYTEXTREP_H_*/
