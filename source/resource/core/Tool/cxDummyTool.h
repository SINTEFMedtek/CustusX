/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDUMMYTOOL_H_
#define CXDUMMYTOOL_H_

#include "cxResourceExport.h"

#include "cxToolImpl.h"
#include <vector>
#include <QThread>
#include <QDateTime>
#include <QTimer>
#include "cxBoundingBox3D.h"
#include "vtkForwardDeclarations.h"
#include "cxProbeSector.h"
#include "cxTime.h"

typedef boost::shared_ptr<class QTimer> QTimerPtr;

namespace cx
{
class ToolManager;

/** Minimal implementation of the Probe interface.
  * Use setData() and setRTSource() to initialize.
  *
  * \ingroup cx_resource_core_tool
  */
class cxResource_EXPORT DummyProbe: public Probe
{
	Q_OBJECT
public:
	virtual ~DummyProbe() {}
	virtual bool isValid() const
	{
		return mProbeDefinition.getType() != ProbeDefinition::tNONE;
	}
	virtual QStringList getAvailableVideoSources() { return QStringList() << "active"; }
	virtual ProbeDefinition getProbeDefinition(QString uid="active") const
	{
		return mProbeDefinition;
	}
	virtual ProbeSectorPtr getSector(QString uid="active")
	{
		ProbeSectorPtr retval(new ProbeSector());
		retval->setData(this->getProbeDefinition());
		return retval;
	}
	virtual VideoSourcePtr getRTSource(QString uid="active") const
	{
		return mVideoSource;
	}

	virtual void setActiveStream(QString uid) {}
	virtual QString getActiveStream() const { return mProbeDefinition.getUid(); }

	virtual void addXml(QDomNode& dataNode) {}
	virtual void parseXml(QDomNode& dataNode) {}

	virtual QStringList getConfigIdList() const { return QStringList(); }
	virtual QString getConfigName(QString uid) { return QString(); }
	virtual QString getConfigId() const { return QString(); }
	virtual QString getConfigurationPath() const { return QString(); }
	virtual void removeCurrentConfig() {}
	virtual void saveCurrentConfig(QString uid, QString name) {}

	virtual void applyNewConfigurationWithId(QString uid) {}
	virtual void setTemporalCalibration(double val) {}
	virtual void setSoundSpeedCompensationFactor(double val) {}
	virtual void setProbeDefinition(ProbeDefinition probeDefinition)
	{
		mProbeDefinition = probeDefinition;
		emit sectorChanged();
	}
	virtual void setRTSource(VideoSourcePtr source)
	{
		mVideoSource = source;
		emit sectorChanged();
	}
	virtual void removeRTSource(VideoSourcePtr source)
	{
		if (source!=mVideoSource)
			return;
		mVideoSource.reset();
		emit sectorChanged();
	}

private:
	ProbeDefinition mProbeDefinition;
	VideoSourcePtr mVideoSource;
};

/**Helper class for emitting signals at a constant rate in a separate thread.
 *
 */
class cxResource_EXPORT DummyToolThread : public QThread
{
	Q_OBJECT
public:
	DummyToolThread(int interval, QObject* parent=NULL) : QThread(parent), mInterval(interval)
	{
		this->setObjectName("org.custusx.resource.core.tracking.dummytool"); // becomes the thread name
	}
protected:
	void run()
	{
		QTimer* timer = new QTimer;
		timer->start(mInterval);
		connect(timer, SIGNAL(timeout()), this, SIGNAL(ping())); // this signal will be executed in the thread of THIS, i.e. the main thread.
//		connect(timer, SIGNAL(timeout()), this, SLOT(pong())); // this signal will be executed in the thread of THIS, i.e. the main thread.
		exec();
		delete timer;
	}
	int mInterval;
private slots:
	void pong()
	{
		std::cout << "Thread Pong " << QDateTime::currentDateTime().toString("mm:ss:zzz").toStdString() << std::endl;
	}
signals:
	void ping();
};

struct cxResource_EXPORT DummyToolTestUtilities
{
	static DummyToolPtr createDummyTool(ProbeDefinition probeDefinition = ProbeDefinition());
	static ProbeDefinition createProbeDefinitionLinear(double depth=40, double width=50, Eigen::Array2i frameSize=Eigen::Array2i(80,40));
	static ProbeDefinition createProbeDefinition(ProbeDefinition::TYPE, double depth=40, double width=50, Eigen::Array2i frameSize=Eigen::Array2i(80,40));
};

/**\brief Implementation of a Tool used for testing.
 *
 * Can move in a specified path.
 *
 * \date Oct 30, 2008
 * \author: Janne Beate Bakeng, SINTEF
 *
 * \ingroup sscTool
 */
class cxResource_EXPORT DummyTool : public ToolImpl
{
	Q_OBJECT
public:
	explicit DummyTool(const QString& uid="dummytool");
	~DummyTool();
	virtual std::set<Type> getTypes() const;
	virtual void setType(Type);

	virtual void setTransformSaveFile(const QString& filename);
	virtual bool getVisible() const;
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual int getIndex() const{return 0;}
	virtual bool isCalibrated() const;
	virtual ProbePtr getProbe() const
	{
		return mProbe;
	}
	void setProbeSector( ProbeDefinition probeDefinition )
	{
		mProbeDefinition = probeDefinition;
		mProbe.reset(new DummyProbe());
		mProbe->setProbeDefinition(mProbeDefinition);
		emit toolProbeSector();
	}
	void setProbeSector(ProbePtr probe)
	{
		mProbe = probe;
		mProbeDefinition = probe->getProbeDefinition();
		emit toolProbeSector();
	}
	virtual double getTimestamp() const { return getMilliSecondsSinceEpoch(); }

	void startTracking(int interval=33);
	void stopTracking();

	void setToolPositionMovement(const std::vector<Transform3D>& positions);
	void setToolPositionMovementBB(const DoubleBoundingBox3D& bb);
	std::vector<Transform3D> createToolPositionMovementTranslationOnly(const DoubleBoundingBox3D& bb) const;
	std::vector<Transform3D> getToolPositionMovement();
	void set_prMt(const Transform3D& ptMt);
	void setVisible(bool val);

//	virtual double getTooltipOffset() const;
//	virtual void setTooltipOffset(double val);
	virtual Transform3D getCalibration_sMt() const;

	virtual std::map<int, Vector3D> getReferencePoints() const {return std::map<int, Vector3D>();};

	static vtkPolyDataPtr createPolyData(double h1, double h2, double r1, double r2);

private slots:
	virtual void sendTransform();

private:
	void stopThread();
	void createPolyData();
	std::vector<Transform3D> createToolPositionMovement(const DoubleBoundingBox3D& bb) const;
	Transform3D* getNextTransform();
	void createLinearMovement(std::vector<Transform3D>* retval, Transform3D* T_in, const Transform3D& R, const Vector3D& a, const Vector3D& b, double step) const;

	bool mVisible;
	QString mTransformSaveFileName;
	QTimerPtr mTimer;
	std::vector<Transform3D> mTransforms;

	static int mTransformCount;
//	Type mType;
	std::set<Type> mTypes;
	ProbeDefinition mProbeDefinition;
	ProbePtr mProbe;
	DummyToolThread* mThread;
};
typedef boost::shared_ptr<DummyTool> DummyToolPtr;
}//namespace cx
#endif /* CXDUMMYTOOL_H_ */
