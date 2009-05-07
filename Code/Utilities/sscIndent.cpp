#include "sscIndent.h"

namespace ssc
{

Indent::Indent() : mDetail(dNORMAL), mIndent(0) {}

Indent::Indent(int ind, DETAIL det) : mDetail(det), mIndent(ind) {}

Indent::~Indent() {}

int Indent::getIndent() const  { return mIndent; }

vtkIndent Indent::getVtkIndent() const { return vtkIndent(mIndent); }

Indent Indent::stepDown() const
{
	return Indent(getIndent()+3, mDetail);
}

bool Indent::includeDetails() const
{
	return mDetail >=dDETAILS;
}

bool Indent::includeNormal() const
{
	return mDetail >=dNORMAL;
}

ostream& operator<<(ostream &os, const Indent &val)
{
	os << std::string(val.getIndent(), ' ').c_str();
	return os;
}


}
