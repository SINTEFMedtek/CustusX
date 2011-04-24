#define _USE_MATH_DEFINES

/*
 * sscTransformTester.cpp
 *
 *  Created on: Sep 9, 2010
 *      Author: christiana
 */

#include "sscTransformTester.h"
#include <fstream>
#include "sscTransform3D.h"
#include "sscTypeConversions.h"
#include "math.h"
#include "vtkMatrix4x4.h"

ssc::Transform3D TransformTest::readTransform(QString filename)
{
  /* File must be in the form
   * rot_00 rot_01 rot_02 trans_0
   * rot_10 rot_11 rot_12 trans_1
   * rot_20 rot_21 rot_22 trans_2
   */
  QString fullstring;
  std::ifstream inputStream;
  inputStream.open(cstring_cast(filename));
  if(inputStream.is_open())
  {
	std::string line;
	int lineNumber = 0;
	while(!inputStream.eof() && lineNumber<3)
	{
	  getline(inputStream, line);
	  fullstring += qstring_cast(line);
	}
  }
  return ssc::Transform3D::fromString(qstring_cast(fullstring));
}
ssc::Transform3D TransformTest::rotateZ180(ssc::Transform3D M0)
{
	ssc::Vector3D c = M0.coord(ssc::Vector3D(0,0,0));
	ssc::Transform3D Tc = ssc::createTransformTranslate(c);
	//std::cout << "\nTc\n" << Tc << "\n" << std::endl;
	//std::cout << "\nTc.inv()\n" << Tc.inv() << "\n" << std::endl;
	ssc::Transform3D Rz180 = ssc::createTransformRotateZ(M_PI);
	//std::cout << "\nRz180\n" << Rz180 << "\n" << std::endl;
	//std::cout << "\nRz180*Tc.inv()\n" << Rz180*Tc.inv() << "\n" << std::endl;
	ssc::Transform3D retval = Tc * Rz180 * M0 * Tc.inv();
	return retval;
}
std::ostream& TransformTest::put(std::ostream& s, ssc::Transform3D M, char newline) const
{
	QString ind(1, ' ');

	std::ostringstream ss; // avoid changing state of input stream
	ss << setprecision(6) << std::fixed;

	for (unsigned i=0; i<4; ++i)
	{
		ss << ind;
		for (unsigned j=0; j<4; ++j)
		{
			ss << setw(10) << M(i,j) << " ";
		}
		if (i!=3)
		{
			ss << newline;
		}
	}

	s << ss.str();

	return s;
}

void TransformTest::test()
{
	QString path("/Users/christiana/christiana/workspace/");
	ssc::Transform3D M0 = readTransform(path+"Vivid7_M12L_8700449_water.cal");
	ssc::Transform3D M1 = readTransform(path+"Vivid7_M12L_8700449.cal");

	std::cout << "M0 \n" << M0 << std::endl;
	std::cout << "M0'\n";
	put(std::cout, rotateZ180(M0), '\n');
	std::cout << std::endl;
	std::cout << "M0 \n" << M1 << std::endl;
	std::cout << "M1'\n";
	put(std::cout, rotateZ180(M1), '\n');
	std::cout << std::endl;
}
