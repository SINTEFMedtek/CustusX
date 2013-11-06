#ifndef CX_DILATION_FILTER_H
#define CX_DILATION_FILTER_H

#include "cxFilterImpl.h"

namespace cx {
class DilationFilter : public FilterImpl
{
	Q_OBJECT

public:
	virtual ~DilationFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	bool preProcess();
	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	DoubleDataAdapterXmlPtr getDilationAmountOption(QDomElement root);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private:
	DoubleDataAdapterXmlPtr mDilationAmountOption;
	vtkImageDataPtr mRawResult;
	vtkPolyDataPtr mRawContour;
};
typedef boost::shared_ptr<class DilationFilter> DilationFilterPtr;

} // namespace cx

#endif
