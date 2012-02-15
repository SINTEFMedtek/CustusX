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


#include "cxElastixExecuter.h"

#include <QFile>
#include "sscMessageManager.h"
#include "cxPatientService.h"
#include "cxPatientData.h"

namespace cx
{

ElastixExecuter::ElastixExecuter()
{
	// TODO Auto-generated constructor stub

}

ElastixExecuter::~ElastixExecuter()
{
	// TODO Auto-generated destructor stub
}

void ElastixExecuter::run(QString application,
				ssc::ImagePtr fixed,
				ssc::ImagePtr moving,
				QString outdir,
				QStringList parameterfiles)
{
	QFile initTransformFile(outdir+"/t0.txt");


	QStringList cmd;
	cmd << application;
	cmd << "-f" << patientService()->getPatientData()->getActivePatientFolder()+"/"+fixed->getFilePath();
	cmd << "-m" << patientService()->getPatientData()->getActivePatientFolder()+"/"+moving->getFilePath();
	cmd << "-out" << outdir;
	cmd << "-t0" << initTransformFile.fileName();
	for (int i=0; i<parameterfiles.size(); ++i)
		cmd << "-p" << parameterfiles[i];

	QString commandLine = cmd.join(" ");
	ssc::messageManager()->sendInfo(QString("Executing registration with command line: [%1]").arg(commandLine));


}

ssc::Transform3D ElastixExecuter::getAffineResult() const
{
	return ssc::Transform3D::Identity();
}



} /* namespace cx */
