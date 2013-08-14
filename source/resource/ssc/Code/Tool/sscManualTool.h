// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCMANUALTOOL_H_
#define SSCMANUALTOOL_H_

#include <QMutex>
#include "sscTool.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{

class ToolManager;

/**\brief A manual tool that is unconnected to any hardware.
 *
 * Implementation of a Tool that is controlled programatically.
 * Can be used as a mouse-controlled offline tool.
 *
 * Use this tool by setting the functions setTransform() and setVisible().
 *
 * This class is Threadsafe.
 *
 * \ingroup sscTool
 */
class ManualTool : public ssc::Tool
{
	Q_OBJECT
public:

	explicit ManualTool(ToolManager* manager, const QString& uid, const QString& name ="");
	virtual ~ManualTool();
	virtual std::set<Type> getTypes() const;
	virtual QString getGraphicsFileName() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual void saveTransformsAndTimestamps(){}
	virtual void setTransformSaveFile(const QString& filename){}
	virtual Transform3D get_prMt() const;
	virtual bool getVisible() const;
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual int getIndex() const;
	virtual bool isCalibrated() const;
	virtual ssc::ProbeData getProbeSector() const;
	virtual double getTimestamp() const;
	virtual void set_prMt(const Transform3D& prMt);
	virtual void set_prMt(const Transform3D& prMt, double timestamp);

	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);

	virtual Transform3D getCalibration_sMt() const;
	virtual std::map<int, Vector3D> getReferencePoints() const { return std::map<int, Vector3D>(); }

	// extensions:
	void setVisible(bool vis);

private slots:
	void read3DCrossHairSlot(double toolTipOffset);
private:
	// constant data
	ssc::ProbeData mSector;
	vtkSTLReaderPtr mSTLReader;
	vtkCursor3DPtr mCrossHair;

	// mutex-protected mutable data
	bool mVisible;
	Transform3D m_prMt;
	mutable QMutex mMutex;
	double mTimestamp;
};

typedef boost::shared_ptr<ManualTool> ManualToolPtr;

}// end namespace

#endif /*SSCMANULATOOL_H_*/
