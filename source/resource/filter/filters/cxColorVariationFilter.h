/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCOLORVARIATIONFILTER_H
#define CXCOLORVARIATIONFILTER_H

#include <random>
#include "cxFilterImpl.h"

namespace cx
{

/** Filter to apply variation in colors to a mesh
 *
 * \ingroup cxResourceAlgorithms
 * \date Aug 26, 2021
 * \author Erlend F. Hofstad
 */
class cxResourceFilter_EXPORT ColorVariationFilter : public FilterImpl
{
	Q_OBJECT

public:
	ColorVariationFilter(VisServicesPtr services);
	virtual ~ColorVariationFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
	static QString getNameSuffixColorVariation();
	MeshPtr getOutputMesh();

	virtual bool execute();
	MeshPtr execute(MeshPtr inputMesh, double globaleVariance, double localeVariance, int smoothingIterations);
	virtual bool postProcess();

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private:
	DoublePropertyPtr getGlobalVarianceOption(QDomElement root);
	DoublePropertyPtr getLocalVarianceOption(QDomElement root);
	DoublePropertyPtr getSmoothingOption(QDomElement root);

	void sortPolyData(vtkPolyDataPtr polyData);
	vtkUnsignedCharArrayPtr colorPolyData(MeshPtr mesh);
	void applyColorToNeighbourPolys(int startIndex, double R, double G, double B);
	std::vector<vtkIdType> applyColorAndFindNeighbours(int pointIndex, double R, double G, double B);
	void generateColorDistribution();
	std::vector<double> generateColor(double R, double G, double B);
	void smoothColorsInMesh(int iterations = 1);

	MeshPtr mOutputMesh;
	vtkUnsignedCharArrayPtr mColors;
	std::vector<std::vector<vtkIdType>> mPolyToPointsArray;
	std::vector<std::vector<vtkIdType>> mPointToPolysArray;
	std::vector<bool> mAssignedColorValues;
	double mGlobalVariance;
	double mLocalVariance;
	double mR_mean;
	double mG_mean;
	double mB_mean;
	std::normal_distribution<> mR_dist;
	std::normal_distribution<> mG_dist;
	std::normal_distribution<> mB_dist;
	std::random_device m_rd{};
	std::mt19937 m_gen{m_rd()};
};

typedef boost::shared_ptr<ColorVariationFilter> ColorVariationFilterPtr;

} // namespace cx


#endif // CXCOLORVARIATIONFILTER_H
