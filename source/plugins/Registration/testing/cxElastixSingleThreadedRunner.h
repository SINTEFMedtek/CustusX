/*
 * cxElastixSingleThreadedRunner.h
 *
 *  Created on: Oct 8, 2012
 *      Author: christiana
 */

#ifndef CXELASTIXSINGLETHREADEDRUNNER_H_
#define CXELASTIXSINGLETHREADEDRUNNER_H_

#include <QObject>
#include "sscTransform3D.h"
#include "cxForwardDeclarations.h"

namespace cx
{

typedef boost::shared_ptr<class ElastixExecuter> ElastixExecuterPtr;


/**Helper class for running ElastixExecuter in a test.
 *
 */
class ElastixSingleThreadedRunner : public QObject
{
	Q_OBJECT
public:
	bool registerLinear(
        ssc::DataPtr fixed,
        ssc::DataPtr moving,
        QString preset,
        ssc::Transform3D* result);

	ElastixSingleThreadedRunner();
	virtual ~ElastixSingleThreadedRunner();

private slots:
	void preprocessExecuter();
	void executionFinishedSlot();
private:
	ElastixExecuterPtr mExecuter;
	bool mCompleted;
	ssc::Transform3D m_mMf;

};

}

#endif /* CXELASTIXSINGLETHREADEDRUNNER_H_ */
