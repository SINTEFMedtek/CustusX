/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXELASTIXEXECUTER_H_
#define CXELASTIXEXECUTER_H_

#include <QString>
#include "cxForwardDeclarations.h"
#include "cxTransform3D.h"
#include <QObject>
#include <QProcess>
#include <QFile>
#include "cxTimedAlgorithm.h"
#include "cxLogger.h"
#include "cxRegServices.h"
#include "org_custusx_registration_method_commandline_Export.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_registration_method_commandline
 * @{
 */

/**
 * \brief ElastiX command-line wrapper.
 *
 * This class wraps a call to the ElastiX library,
 * http://elastix.isi.uu.nl .
 *
 * Call the run method to execute an elastiX registration,
 * wait for the signal finished(),
 * then get the results using the getters.
 *
 *
 * \date Feb 4, 2012
 * \author Christian Askeland, SINTEF
 */
class org_custusx_registration_method_commandline_EXPORT ElastixExecuter : public TimedBaseAlgorithm
{
	Q_OBJECT
public:
	ElastixExecuter(RegServicesPtr services, QObject* parent=NULL);
	virtual ~ElastixExecuter();

	void setDisplayProcessMessages(bool on);

	bool setInput(QString application,
	         DataPtr fixed,
	         DataPtr moving,
	         QString outdir,
	         QStringList parameterfiles);
	virtual void execute();
	virtual bool isFinished() const;
    virtual bool isRunning() const;

	/** Return the result of the latest registration as a linear transform mMf.
	 *
	 * Read the descriptions in writeInitTransformToElastixfile() and
	 * getAffineResult_mmMff for a full discussion.
	 */
	Transform3D getAffineResult_mMf(bool* ok = 0) ;
	QString getNonlinearResultVolume(bool* ok = 0);

private slots:
	void processStateChanged(QProcess::ProcessState newState);
	void processError(QProcess::ProcessError error);
	void processFinished(int, QProcess::ExitStatus);
	void processReadyRead();

private:
	/** Write the initial (pre-registration) mMf transform to
	 *  disk as required by elastix.
	 */
	QString writeInitTransformToElastixfile(DataPtr fixed, DataPtr moving, QString outdir);
	/** Write the initial (pre-registration) mMf transform to
	 *  disk in a .cal file that contains only the 16 matrix numbers.
	 */
	QString writeInitTransformToCalfile(DataPtr fixed, DataPtr moving, QString outdir);
	/** Find the TransformParameters.i.txt file with the
	 *  highest i. All other transform files can be found from
	 *  this one.
	 */
	QString findMostRecentTransformOutputFile() const;
	/** Return the result of the latest registration as a linear transform mMf.
	 *
	 *  Important: The result is according to the ElastiX spec:
	 * \verbatim
	   In elastix the transformation is defined as a coordinate mapping from
	   the fixed image domain to the moving image domain.
	   \endverbatim
	 *
	 * All transform files are read and concatenated. Those with
	 * unrecognized (i.e. by CustusX) transforms are ignored with
	 * a warning.
	 *
	 * NOTE: This 'inner' function returns the raw result from elastiX,
	 * but CustusX expects that the file transforms of the fixed and moving
	 * images are also contained in the result. Use the getAffineResult_mMf()
	 * for the full result.
	 *
	 */
	Transform3D getAffineResult_mmMff(bool* ok = 0) ;
	/** Return the transform present within the mhd file pointed to by the
	 * input volume.
	 *
	 * This is part of the normal rMd transform within Data, but required
	 * because elastiX reads and uses it.
	 */
	Transform3D getFileTransform_ddMd(DataPtr volume);

	QString mLastOutdir;
	QProcess* mProcess;
	DataPtr mFixed;
	DataPtr mMoving;
	RegServicesPtr mServices;
};

/**Reader class for an Elastix-style parameter file.
 *
 */
class ElastixParameterFile
{
public:
	ElastixParameterFile(QString filename);

	bool isValid() const;
	QString readParameterString(QString key) ;
	bool readParameterBool(QString key) ;
	int readParameterInt(QString key) ;
	std::vector<double> readParameterDoubleVector(QString key) ;
//	void writeParameter(QString key, QStringList value);
	Transform3D readEulerTransform() ;
	Transform3D readAffineTransform();

private:
	QString readParameterRawValue(QString key);
	QFile mFile;
	QString mText;
};

/**Class encapsulating the math conversions
 * between the ElastiX "EulerTransform" representation
 * and the Transform3D representation.
 */
class ElastixEulerTransform
{
public:
	Vector3D mAngles_xyz;
	Vector3D mTranslation;
	Vector3D mCenterOfRotation;

	static ElastixEulerTransform create(Vector3D angles_xyz, Vector3D translation, Vector3D centerOfRotation)
	{
		ElastixEulerTransform retval;
		retval.mAngles_xyz = angles_xyz;
		retval.mTranslation = translation;
		retval.mCenterOfRotation = centerOfRotation;
		return retval;
	}
	static ElastixEulerTransform create(Transform3D M, Vector3D centerOfRotation)
	{
		ElastixEulerTransform retval;
		retval.mAngles_xyz = M.matrix().block<3, 3> (0, 0).eulerAngles(0, 1, 2);
		retval.mCenterOfRotation = centerOfRotation;

		Transform3D R = retval.getRotationMatrix();
		Transform3D C = createTransformTranslate(retval.mCenterOfRotation);
		// solve M = T*Tc*R*Tc.inv() with respect to T:
		Transform3D T = M*C*R.inv()*C.inv();

		retval.mTranslation = T.matrix().block<3, 1> (0, 3);
//		retval.mCenterOfRotation = Vector3D(0,0,0);
		return retval;
	}
	Transform3D toMatrix() const
	{
		Transform3D T = createTransformTranslate(mTranslation);
		Transform3D C = createTransformTranslate(mCenterOfRotation);
		Transform3D R = this->getRotationMatrix();
		Transform3D Q = T*C*R*C.inv();
		return Q;
	}
	static void test()
	{
		std::cout << "==========TEST==============" << std::endl;
		Transform3D M = createTransformRotateX(M_PI/3)*createTransformRotateY(M_PI/4)*createTransformTranslate(Vector3D(0,2,3));

		ElastixEulerTransform E = create(M, Vector3D(30,40,50));
		Transform3D Q = E.toMatrix();

		std::cout << "M\n" << M << std::endl;
		std::cout << "Q\n" << Q << std::endl;
		Transform3D diff = Q*M.inv();
		std::cout << "Q*M.inv\n" << diff << std::endl;
		if (!similar(Transform3D::Identity(), diff))
			reportError("assertion failure in ElastixEulerTransform");
	}
private:
	ElastixEulerTransform() {}

	Transform3D getRotationMatrix() const
	{
		Eigen::Matrix3d m;
		m =	Eigen::AngleAxisd(mAngles_xyz[0], Eigen::Vector3d::UnitX())
		  * Eigen::AngleAxisd(mAngles_xyz[1], Eigen::Vector3d::UnitY())
		  * Eigen::AngleAxisd(mAngles_xyz[2], Eigen::Vector3d::UnitZ());

		Transform3D R = Transform3D::Identity();
		R.matrix().block<3, 3> (0, 0) = m;
		return R;
	}
};


/**
 * @}
 */
} /* namespace cx */
#endif /* CXELASTIXEXECUTER_H_ */
