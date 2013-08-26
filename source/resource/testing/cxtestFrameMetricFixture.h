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

	cx::FrameMetricPtr mOriginalMetric;
	cx::FrameMetricPtr mModifiedMetric;
	ssc::Transform3D mTestTransform;
	QDomNode mXmlNode;
	QString mTestFile;
};

} //namespace cxtest
#endif // CXTESTFRAMEMETRICFIXTURE_H_
