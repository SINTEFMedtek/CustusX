#ifndef SSCDISPLAYTEXTREP_H_
#define SSCDISPLAYTEXTREP_H_

#include <vector>
#include "sscRepImpl.h"
#include "sscVector3D.h"

namespace ssc
{
typedef boost::shared_ptr<class TextDisplay> TextDisplayPtr;

typedef boost::shared_ptr<class DisplayTextRep> DisplayTextRepPtr;

class DisplayTextRep : public ssc::RepImpl
{
public:
	static DisplayTextRepPtr New(const QString& uid, const QString& name);
	virtual QString getType() const{ return "vm::DisplayTextRep";};
	void addText(const Vector3D& color, const QString& text, const Vector3D& pos);
	void setText(unsigned i, const QString& text);
	void setColor(const Vector3D& color );
	void setFontSize(int size);
	virtual ~DisplayTextRep();
protected:
	DisplayTextRep(const QString& uid, const QString& name);
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);
	
	std::vector<TextDisplayPtr> mDisplayText;

	TextDisplayPtr mPictString;
};



}//end namespace

#endif /*SSCDISPLAYTEXTREP_H_*/
