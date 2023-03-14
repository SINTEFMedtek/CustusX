/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMESHESFROMLABELSFILTER_H
#define CXMESHESFROMLABELSFILTER_H

#include "cxFilterImpl.h"
class QColor;

namespace cx
{

/** Create meshes from a labeled volume
 *  Marching cubes surface generation.
 *
 * \ingroup cx
 * \date Mar 08, 2023
 * \author Ole Vegard Solberg
 */
class cxResourceFilter_EXPORT MeshesFromLabelsFilter : public FilterImpl
{
	Q_OBJECT

public:
	MeshesFromLabelsFilter(VisServicesPtr services);
	virtual ~MeshesFromLabelsFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
    static QString getNameSuffix();
	virtual void setActive(bool on);

	bool preProcess();
	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	BoolPropertyPtr getReduceResolutionOption(QDomElement root);
	BoolPropertyPtr getSmoothingOption(QDomElement root);
	BoolPropertyPtr getPreserveTopologyOption(QDomElement root);
	DoublePropertyPtr getSurfaceThresholdOption(QDomElement root);
	DoublePropertyPtr getDecimationOption(QDomElement root);
	ColorPropertyPtr getColorOption(QDomElement root);
	BoolPropertyPtr getGenerateColorOption(QDomElement root);
    DoublePropertyPtr getNumberOfIterationsOption(QDomElement root);
	DoublePropertyPtr getPassBandOption(QDomElement root);
	DoublePropertyPtr getStartLabelOption(QDomElement root);
	DoublePropertyPtr getEndLabelOption(QDomElement root);

	/** This is the core algorithm, call this if you dont need all the filter stuff.
	    Generate a contour from a vtkImageData.
	  */
	static std::vector<vtkPolyDataPtr> execute(vtkImageDataPtr input,
								  int startLabel,
								  int endLabel,
								  double threshold,
								  bool reduceResolution=false,
								  bool smoothing=true,
								  bool preserveTopology=true,
								  double decimation=0.2,
								  double numberOfIterations = 15,
								  double passBand = 0.3);
	/** Generate a mesh from the contour using base to generate name.
	  * Save to dataManager.
	  */
	static std::vector<MeshPtr> postProcess(PatientModelServicePtr patient, std::vector<vtkPolyDataPtr> contours, ImagePtr base, QColor color, bool generateColors);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();
	static QColor generateColor(int colorNum, int colorCount);

private slots:
	/** Set new value+range of the threshold option.
	  */
	void imageChangedSlot(QString uid);
	void thresholdSlot();

private:
	void stopPreview();

	BoolPropertyPtr mReduceResolutionOption;
	DoublePropertyPtr mSurfaceThresholdOption;
	std::vector<vtkPolyDataPtr> mRawResult;
	ImagePtr mPreviewImage;
};
typedef boost::shared_ptr<class MeshesFromLabelsFilter> MeshesFromLabelsFilterPtr;


} // namespace cx

#endif // CXMESHESFROMLABELSFILTER_H
