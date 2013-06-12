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


#ifndef CXELASTIXEXECUTER_H_
#define CXELASTIXEXECUTER_H_

#include <QString>
#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"
#include <QObject>
#include <QProcess>
#include <QFile>
#include "cxTimedAlgorithm.h"
#include "sscMessageManager.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginRegistration
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
class ElastixExecuter : public TimedBaseAlgorithm
{
	Q_OBJECT
public:
	ElastixExecuter(QObject* parent=NULL);
	virtual ~ElastixExecuter();

	void setDisplayProcessMessages(bool on);

	bool setInput(QString application,
	         ssc::DataPtr fixed,
	         ssc::DataPtr moving,
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
	ssc::Transform3D getAffineResult_mMf(bool* ok = 0) ;
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
	QString writeInitTransformToElastixfile(ssc::DataPtr fixed, ssc::DataPtr moving, QString outdir);
	/** Write the initial (pre-registration) mMf transform to
	 *  disk in a .cal file that contains only the 16 matrix numbers.
	 */
	QString writeInitTransformToCalfile(ssc::DataPtr fixed, ssc::DataPtr moving, QString outdir);
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
	ssc::Transform3D getAffineResult_mmMff(bool* ok = 0) ;
	/** Return the transform present within the mhd file pointed to by the
	 * input volume.
	 *
	 * This is part of the normal rMd transform within ssc::Data, but required
	 * because elastiX reads and uses it.
	 */
	ssc::Transform3D getFileTransform_ddMd(ssc::DataPtr volume);

	QString mLastOutdir;
	QProcess* mProcess;
	ssc::DataPtr mFixed;
	ssc::DataPtr mMoving;
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
	ssc::Transform3D readEulerTransform() ;
	ssc::Transform3D readAffineTransform();

private:
	QString readParameterRawValue(QString key);
	QFile mFile;
	QString mText;
};

/**Class encapsulating the math conversions
 * between the ElastiX "EulerTransform" representation
 * and the ssc::Transform3D representation.
 */
class ElastixEulerTransform
{
public:
	ssc::Vector3D mAngles_xyz;
	ssc::Vector3D mTranslation;
	ssc::Vector3D mCenterOfRotation;

	static ElastixEulerTransform create(ssc::Vector3D angles_xyz, ssc::Vector3D translation, ssc::Vector3D centerOfRotation)
	{
		ElastixEulerTransform retval;
		retval.mAngles_xyz = angles_xyz;
		retval.mTranslation = translation;
		retval.mCenterOfRotation = centerOfRotation;
		return retval;
	}
	static ElastixEulerTransform create(ssc::Transform3D M, ssc::Vector3D centerOfRotation)
	{
		ElastixEulerTransform retval;
		retval.mAngles_xyz = M.matrix().block<3, 3> (0, 0).eulerAngles(0, 1, 2);
		retval.mCenterOfRotation = centerOfRotation;

		ssc::Transform3D R = retval.getRotationMatrix();
		ssc::Transform3D C = ssc::createTransformTranslate(retval.mCenterOfRotation);
		// solve M = T*Tc*R*Tc.inv() with respect to T:
		ssc::Transform3D T = M*C*R.inv()*C.inv();

		retval.mTranslation = T.matrix().block<3, 1> (0, 3);
//		retval.mCenterOfRotation = ssc::Vector3D(0,0,0);
		return retval;
	}
	ssc::Transform3D toMatrix() const
	{
		ssc::Transform3D T = ssc::createTransformTranslate(mTranslation);
		ssc::Transform3D C = ssc::createTransformTranslate(mCenterOfRotation);
		ssc::Transform3D R = this->getRotationMatrix();
		ssc::Transform3D Q = T*C*R*C.inv();
		return Q;
	}
	static void test()
	{
		std::cout << "==========TEST==============" << std::endl;
		ssc::Transform3D M = ssc::createTransformRotateX(M_PI/3)*ssc::createTransformRotateY(M_PI/4)*ssc::createTransformTranslate(ssc::Vector3D(0,2,3));

		ElastixEulerTransform E = ElastixEulerTransform::create(M, ssc::Vector3D(30,40,50));
		ssc::Transform3D Q = E.toMatrix();

		std::cout << "M\n" << M << std::endl;
		std::cout << "Q\n" << Q << std::endl;
		ssc::Transform3D diff = Q*M.inv();
		std::cout << "Q*M.inv\n" << diff << std::endl;
		if (!ssc::similar(ssc::Transform3D::Identity(), diff))
			ssc::messageManager()->sendError("assertion failure in ElastixEulerTransform");
	}
private:
	ElastixEulerTransform() {}

	ssc::Transform3D getRotationMatrix() const
	{
		Eigen::Matrix3d m;
		m =	Eigen::AngleAxisd(mAngles_xyz[0], Eigen::Vector3d::UnitX())
		  * Eigen::AngleAxisd(mAngles_xyz[1], Eigen::Vector3d::UnitY())
		  * Eigen::AngleAxisd(mAngles_xyz[2], Eigen::Vector3d::UnitZ());

		ssc::Transform3D R = ssc::Transform3D::Identity();
		R.matrix().block<3, 3> (0, 0) = m;
		return R;
	}
};


/**
 * @}
 */
} /* namespace cx */
#endif /* CXELASTIXEXECUTER_H_ */
