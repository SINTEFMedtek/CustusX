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

#include "sscProbeSector.h"

#include "vtkImageData.h"
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkPolyLine.h>
#include <vtkClipPolyData.h>
#include <vtkBox.h>
#include <vtkPlane.h>
#include <vtkPlanes.h>
#include <vtkCutter.h>
#include <vtkAppendPolyData.h>
//#include <vtkPolyPlane.h>
#include "sscBoundingBox3D.h"
#include "sscVolumeHelpers.h"
#include "sscUtilHelpers.h"

typedef vtkSmartPointer<class vtkPlanes> vtkPlanesPtr;
typedef vtkSmartPointer<class vtkPlane> vtkPlanePtr;
typedef vtkSmartPointer<class vtkBox> vtkBoxPtr;
typedef vtkSmartPointer<class vtkCutter> vtkCutterPtr;
typedef vtkSmartPointer<class vtkAppendPolyData> vtkAppendPolyDataPtr;
typedef vtkSmartPointer<class vtkFloatArray> vtkFloatArrayPtr;

namespace ssc
{

ProbeSector::ProbeSector()
{
	mPolyData = vtkPolyDataPtr::New();
}

void ProbeSector::setData(ProbeData data)
{
	mData = data;
	//  this->test();
}

/**Function object for evaluating whether a pixel is inside the
 * us mask.
 */
class InsideMaskFunctor
{
public:
	InsideMaskFunctor(ProbeData data, Transform3D uMv) :
		mData(data), m_vMu(uMv.inv())
	{
		mCachedCenter_v = m_vMu.coord(mData.getImage().getOrigin_u());
		mClipRect_v = transform(m_vMu, mData.getImage().getClipRect_u());
		mClipRect_v[4] = -1;
		mClipRect_v[5] = 1;
	}
	bool operator ()(int x, int y) const
	{
		Vector3D p_v = multiply_elems(Vector3D(x, y, 0), mData.getImage().mSpacing);

		return this->insideClipRect(p_v) && this->insideSector(p_v);
	}

private:
	/**return true if p_v, given in the upper-left space v,
	 * is inside the us beam sector
	 *
	 * Prerequisite: mCachedCenter_v is updated!
	 */
	bool insideClipRect(const Vector3D& p_v) const
	{
		return mClipRect_v.contains(p_v);
	}

	/**return true if p_v, given in the upper-left space v,
	 * is inside the us beam sector
	 *
	 * Prerequisite: mCachedCenter_v is updated!
	 */
	bool insideSector(const Vector3D& p_v) const
	{
		Vector3D d = p_v - mCachedCenter_v;

		if (mData.getType() == ProbeData::tSECTOR)
		{
			double angle = atan2(d[1], d[0]);
			angle -= M_PI_2; // center angle on us probe axis at 90*.
			if (angle < -M_PI)
				angle += 2.0 * M_PI;

			if (fabs(angle) > mData.getWidth() / 2.0)
				return false;
			if (d.length() < mData.getDepthStart())
				return false;
			if (d.length() > mData.getDepthEnd())
				return false;
			return true;
		}
		else // tLINEAR
		{
			if (fabs(d[0]) > mData.getWidth() / 2.0)
				return false;
			if (d[1] < mData.getDepthStart())
				return false;
			if (d[1] > mData.getDepthEnd())
				return false;
			return true;
		}
	}

	ProbeData mData;
	Transform3D m_vMu;
	Vector3D mCachedCenter_v; ///< center of beam sector for sector probes.
	ssc::DoubleBoundingBox3D mClipRect_v;
};

/** Return a 2D mask image identifying the US beam inside the image
 *  data stream.
 */
vtkImageDataPtr ProbeSector::getMask()
{
	if (mData.getType()==ProbeData::tNONE)
		return vtkImageDataPtr();
	InsideMaskFunctor checkInside(mData, this->get_uMv());
	vtkImageDataPtr retval;
	retval = generateVtkImageData(Eigen::Array3i(mData.getImage().mSize.width(), mData.getImage().mSize.height(), 1),
		mData.getImage().mSpacing, 0);

	int* dim(retval->GetDimensions());
	unsigned char* dataPtr = static_cast<unsigned char*> (retval->GetScalarPointer());
	for (int x = 0; x < dim[0]; x++)
		for (int y = 0; y < dim[1]; y++)
		{
			dataPtr[x + y * dim[0]] = checkInside(x, y) ? 1 : 0;
		}

	return retval;
}

void ProbeSector::test()
{
	Transform3D tMu = this->get_tMu();
	Vector3D e_x(1, 0, 0);
	Vector3D e_y(0, 1, 0);
	Vector3D e_z(0, 0, 1);

	// zero = tMu * mOrigin_u
	std::cout << "zero = tMu * mOrigin_u, zero: " << tMu.coord(mData.getImage().getOrigin_u()) << ", mOrigin_u: "
		<< mData.getImage().getOrigin_u() << std::endl;

	// e_z = tMu * -e_y
	std::cout << "e_z = tMu * -e_y " << tMu.vector(-e_y) << std::endl;

	// e_y = tMu * -e_x
	std::cout << "e_y = tMu * -e_x " << tMu.vector(-e_x) << std::endl;

	// tMu * e_x
	std::cout << "tMu * e_x = <0,-1,0>" << tMu.vector(e_x) << std::endl;
	// tMu * e_y
	std::cout << "tMu * e_y = <0,0,-1> " << tMu.vector(e_y) << std::endl;

}

Transform3D ProbeSector::get_tMu() const
{
	Transform3D Rx = ssc::createTransformRotateX(-M_PI / 2.0);
	Transform3D Rz = ssc::createTransformRotateY(M_PI / 2.0);
	ssc::Transform3D R = (Rx * Rz);
	ssc::Transform3D T = ssc::createTransformTranslate(-mData.getImage().getOrigin_u());

	ssc::Transform3D tMu = R * T;
	return tMu;
}

Transform3D ProbeSector::get_uMv() const
{
	// use H-1 because we count between pixel centers.
	double H = (mData.getImage().mSize.height() - 1) * mData.getImage().mSpacing[1];
	return createTransformRotateX(M_PI) * createTransformTranslate(Vector3D(0, -H, 0));
}

vtkPolyDataPtr ProbeSector::getSector()
{
	this->updateSector();
	return mPolyData;
}

/**Return true is cliprect has any effect on the sector,
 * i.e if the intersection between sector and cliprect is
 * different from sector.
 */
bool ProbeSector::clipRectIntersectsSector() const
{
	DoubleBoundingBox3D s(mPolyData->GetPoints()->GetBounds());
	DoubleBoundingBox3D c = mData.getImage().getClipRect_u();

	bool outside = ( (c[0] < s[0]) || similar(c[0],s[0], 0.01) )
			&& ( (s[1] < c[1]) || similar(s[1],c[1], 0.01) )
			&& ( (c[2] < s[2]) || similar(c[2],s[2], 0.01) )
			&& ( (s[3] < c[3]) || similar(s[3],c[3], 0.01) );
	return !outside;
}

vtkPolyDataPtr ProbeSector::getSectorLinesOnly()
{
	this->updateSector();
	if (mData.getType() == ProbeData::tNONE)
		return mPolyData;

	vtkPolyDataPtr output = vtkPolyDataPtr::New();

	output->SetPoints(mPolyData->GetPoints());
	output->SetLines(mPolyData->GetLines());
	//  output->SetStrips(mPolyData->GetStrips());
	//  return output;

	// also display the cliprect
	vtkAppendPolyDataPtr retval = vtkAppendPolyDataPtr::New();
	retval->AddInput(output);

	if (this->clipRectIntersectsSector())
		retval->AddInput(this->getClipRectPolyData());

	retval->Update();
	return retval->GetOutput();
}

vtkPolyDataPtr ProbeSector::getSectorSectorOnlyLinesOnly()
{
	this->updateSector();
	if (mData.getType() == ProbeData::tNONE)
		return mPolyData;

	vtkPolyDataPtr output = vtkPolyDataPtr::New();
	output->SetPoints(mPolyData->GetPoints());
	output->SetLines(mPolyData->GetLines());

	output->Update();
	return output;
}

vtkPolyDataPtr ProbeSector::getClipRectLinesOnly()
{
	return this->getClipRectPolyData();
}

/**generate a polydata containing only a polygon representing the sector cliprect.
 *
 */
vtkPolyDataPtr ProbeSector::getClipRectPolyData()
{
	vtkPointsPtr points = vtkPointsPtr::New();
	vtkCellArrayPtr sides = vtkCellArrayPtr::New();

	//points->Allocate(N+M);
	vtkIdType cells[5] =
	{ 0, 1, 2, 3, 0 };
	sides->InsertNextCell(5, cells);

	DoubleBoundingBox3D bb = mData.getImage().getClipRect_u();
	points->InsertNextPoint(bb.corner(0, 0, 0).begin());
	points->InsertNextPoint(bb.corner(1, 0, 0).begin());
	points->InsertNextPoint(bb.corner(1, 1, 0).begin());
	points->InsertNextPoint(bb.corner(0, 1, 0).begin());

	vtkPolyDataPtr polydata = vtkPolyDataPtr::New();
	polydata->SetPoints(points);
	polydata->SetLines(sides);

	//  polydata = this->generateClipper(polydata);
	return polydata;
}

/**generate a polydata showing the origin as a small line.
 *
 */
vtkPolyDataPtr ProbeSector::getOriginPolyData()
{
	vtkPointsPtr points = vtkPointsPtr::New();
	vtkCellArrayPtr sides = vtkCellArrayPtr::New();

	//points->Allocate(N+M);
	vtkIdType cells[4] =
	{ 1, 0, 2, 3 };
	sides->InsertNextCell(4, cells);

	ssc::Vector3D o_u = mData.getImage().getOrigin_u();
	double length = (mData.getDepthStart() - mData.getDepthEnd())/15;
	length = ssc::constrainValue(length, 2, 10);
//	length = 5; //mm
	ssc::Vector3D tip = o_u + ssc::Vector3D(0, -length, 0);
	ssc::Vector3D left = o_u + ssc::Vector3D(-length/3, 0, 0);
	ssc::Vector3D right = o_u + ssc::Vector3D(length/3, 0, 0);

	points->InsertNextPoint(o_u.begin());
	points->InsertNextPoint(tip.begin());
	points->InsertNextPoint(left.begin());
	points->InsertNextPoint(right.begin());

	vtkPolyDataPtr polydata = vtkPolyDataPtr::New();
	polydata->SetPoints(points);
	polydata->SetLines(sides);

	//  polydata = this->generateClipper(polydata);
	return polydata;
}

void ProbeSector::updateSector()
{
	if (mData.getType() == ProbeData::tNONE)
	{
		mPolyData = vtkPolyDataPtr::New();
		return;
	}

	ssc::Vector3D bounds = ssc::Vector3D(mData.getImage().mSize.width() - 1, mData.getImage().mSize.height() - 1, 1);
	bounds = multiply_elems(bounds, mData.getImage().mSpacing);

	vtkFloatArrayPtr newTCoords = vtkFloatArrayPtr::New();
	newTCoords->SetNumberOfComponents(2);

	ssc::Vector3D p(0, 0, 0); // tool position in local space
	// first define the shape of the probe in a xy-plane.
	// after that, transform into yz-plane because thats the tool plane (probe point towards positive z)
	// then transform to global space.
	Transform3D tMl = createTransformIJC(Vector3D(0, 1, 0), Vector3D(0, 0, 1), Vector3D(0, 0, 0));
	Transform3D texMu = createTransformNormalize(DoubleBoundingBox3D(0, bounds[0], 0, bounds[1], 0, 1),
		DoubleBoundingBox3D(0, 1, 0, 1, 0, 1));
	Transform3D uMt = this->get_tMu().inv();
	Transform3D texMl = texMu * uMt * tMl;
	Transform3D uMl = uMt * tMl;

	//Transform3D M = tMl;
	Vector3D e_x = unitVector(0);
	Vector3D e_y = unitVector(M_PI_2);
	Vector3D e_z(0, 0, 1);

	vtkPointsPtr points = vtkPointsPtr::New();
	vtkCellArrayPtr sides = vtkCellArrayPtr::New();
	vtkCellArrayPtr strips = vtkCellArrayPtr::New();
	vtkCellArrayPtr polys = vtkCellArrayPtr::New();

	DoubleBoundingBox3D bb_u;

	if (mData.getType() == ProbeData::tLINEAR)
	{
		Vector3D cr = mData.getDepthStart() * e_y + mData.getWidth() / 2 * e_x;
		Vector3D cl = mData.getDepthStart() * e_y - mData.getWidth() / 2 * e_x;
		Vector3D pr = mData.getDepthEnd() * e_y + mData.getWidth() / 2 * e_x;
		Vector3D pl = mData.getDepthEnd() * e_y - mData.getWidth() / 2 * e_x;

		points->Allocate(4);
		points->InsertNextPoint(uMl.coord(cl).begin());
		points->InsertNextPoint(uMl.coord(cr).begin());
		points->InsertNextPoint(uMl.coord(pr).begin());
		points->InsertNextPoint(uMl.coord(pl).begin());

		newTCoords->Allocate(4);
		newTCoords->InsertNextTuple(texMl.coord(cl).begin());
		newTCoords->InsertNextTuple(texMl.coord(cr).begin());
		newTCoords->InsertNextTuple(texMl.coord(pr).begin());
		newTCoords->InsertNextTuple(texMl.coord(pl).begin());

		vtkIdType cells[5] =
		{ 0, 1, 2, 3, 0 };
		sides->InsertNextCell(5, cells);
		polys->InsertNextCell(5, cells);
		vtkIdType s_cells[5] =
		{ 0, 3, 1, 2 };
		strips->InsertNextCell(4, s_cells);
	}
	else if (mData.getType() == ProbeData::tSECTOR)
	{
		Vector3D c(0, 0, 0); // arc center point
		c += mData.getCenterOffset() * e_y;  // arc center point

		//    std::cout << "c_local " << c << std::endl;
		//    std::cout << "c_u " << uMl.coord(c) << std::endl;
		//    Vector3D c_e = c + mData.mDepthStart * unitVector(M_PI_2);
		//    std::cout << "c_e_local " << c_e << std::endl;
		//    std::cout << "c_e_u " << uMl.coord(c_e) << std::endl;

		//    int arcRes = 20;//Number of points in arc
		int arcRes = 20;//Number of points in arc
		double angleIncrement = mData.getWidth() / arcRes;
		double startAngle = M_PI_2 - mData.getWidth() / 2.0;
		double stopAngle = M_PI_2 + mData.getWidth() / 2.0;
		int N = 2 * (arcRes + 1); // total number of points

		points->Allocate(N);
		newTCoords->Allocate(2 * N);

		for (int i = 0; i <= arcRes; i++)
		{
			double theta = startAngle + i * angleIncrement;
			Vector3D startTheta;
			if (mData.getCenterOffset() == 0)
			{
				startTheta = c + (mData.getDepthStart()-mData.getCenterOffset()) * unitVector(theta);
			}
			else
			{
				startTheta = c + (mData.getDepthStart()-mData.getCenterOffset()) * unitVector(theta) / sin(theta);
			}
			newTCoords->InsertNextTuple(texMl.coord(startTheta).begin());
			points->InsertNextPoint(uMl.coord(startTheta).begin());
			//std::cout << "p_arc " << uMl.coord(startTheta) << std::endl;
		}
		for (int i = 0; i <= arcRes; i++)
		{
			double theta = stopAngle - i * angleIncrement;
			Vector3D endTheta;
			double offset = 0;
			
			if (mData.getCenterOffset() != 0)
			{
				offset = (mData.getDepthStart()-mData.getCenterOffset()) / sin(theta) - (mData.getDepthStart()-mData.getCenterOffset());
			}
			endTheta = c + (mData.getDepthEnd()-mData.getCenterOffset()+offset) * unitVector(theta);
			newTCoords->InsertNextTuple(texMl.coord(endTheta).begin());
			points->InsertNextPoint(uMl.coord(endTheta).begin());
			//std::cout << "p_arc " << uMl.coord(endTheta) << std::endl;
		}

		sides->InsertNextCell(N + 1);
		for (int i = 0; i < N; i++)
			sides->InsertCellPoint(i);
		sides->InsertCellPoint(0);

		polys->InsertNextCell(N + 1);
		for (int i = 0; i < arcRes * 2 + 2; i++)
			polys->InsertCellPoint(i);
		polys->InsertCellPoint(0);

		strips->InsertNextCell(N);
		for (int i = 0; i <= arcRes; ++i)
		{
			strips->InsertCellPoint(i);
			strips->InsertCellPoint(N - 1 - i);
		}
	}

	vtkPolyDataPtr polydata = vtkPolyDataPtr::New();
	polydata->SetPoints(points);
	polydata->SetStrips(strips);
	polydata->GetPointData()->SetTCoords(newTCoords);
	polydata->SetLines(sides);
	//  polydata->SetPolys(polys);
	mPolyData = polydata;
	//  mPolyData = this->generateClipper(polydata);
}

}
