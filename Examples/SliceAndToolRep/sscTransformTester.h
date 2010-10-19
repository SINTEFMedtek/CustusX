/*
 * sscTransformTester.h
 *
 *  Created on: Sep 9, 2010
 *      Author: christiana
 */

#ifndef SSCTRANSFORMTESTER_H_
#define SSCTRANSFORMTESTER_H_

#include "sscTransform3D.h"
#include <iostream>

struct TransformTest
{
	ssc::Transform3D readTransform(QString filename);
	ssc::Transform3D rotateZ180(ssc::Transform3D M0);
	std::ostream& put(std::ostream& s, ssc::Transform3D M, char newline) const;
	void test();
};

#endif /* SSCTRANSFORMTESTER_H_ */
