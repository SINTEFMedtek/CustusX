// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXTESTFRAMEMETRICFIXTURE_H_
#define CXTESTFRAMEMETRICFIXTURE_H_

#include "cxFrameMetric.h"
#include <QDomNode>

namespace cxtest {

/*
 * cxtestFrameMetricFixture.h
 *
 * \date Aug 20, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class FrameMetricFixture {
public:
	FrameMetricFixture();
	~FrameMetricFixture();
	bool createAndSetTestTransform();
	bool isEqualTransform(ssc::Transform3D transform);
	QDomNode createDummyXmlNode();
	cx::FrameMetricPtr createFromXml(QDomNode& xmlNode);
	QString expectedStringAfterConversion();
	void setPatientRegistration();
	void changeSpaceToPatientReference();
	void setSpaceToOrigial();

	QString mTransformString;
	cx::FrameMetricPtr mOriginalMetric;
	cx::FrameMetricPtr mModifiedMetric;
	ssc::Transform3D mTestTransform;
	QDomNode mXmlNode;
	QString mTestFile;
};

} //namespace cxtest
#endif // CXTESTFRAMEMETRICFIXTURE_H_
