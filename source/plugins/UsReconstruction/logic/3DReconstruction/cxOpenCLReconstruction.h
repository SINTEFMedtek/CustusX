#ifndef CXOPENCLRECONSTRUCTION_H_
#define CXOPENCLRECONSTRUCTION_H_

#include "sscReconstructAlgorithm.h"

#include <QDomElement>

namespace cx
{
/**
 * \brief 
 *
 * \date Jan 22, 2014
 * \author Janne Beate Bakeng, SINTEF
 */

class OpenCLReconstruction : public ReconstructAlgorithm
{
public:
	virtual QString getName() const;
	virtual std::vector<DataAdapterPtr> getSettings(QDomElement root);
	virtual bool reconstruct(ProcessedUSInputDataPtr input, vtkImageDataPtr output, QDomElement settings);
};


} /* namespace cx */

#endif /* CXOPENCLRECONSTRUCTION_H_ */
