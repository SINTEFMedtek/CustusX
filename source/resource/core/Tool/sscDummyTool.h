#ifndef SSCDUMMYTOOL_H_
#define SSCDUMMYTOOL_H_

#include "cxToolImpl.h"
#include <vector>
#include <QThread>
#include <QDateTime>
#include <QTimer>
#include "sscBoundingBox3D.h"
#include "vtkForwardDeclarations.h"
#include "sscProbeSector.h"
#include "sscTime.h"

typedef boost::shared_ptr<class QTimer> QTimerPtr;

namespace cx
{
class ToolManager;

/** Minimal implementation of the Probe interface.
  * Use setData() and setRTSource() to initialize.
  *
 * \ingroup cx_resource_core_tool
  */
class DummyProbe: public Probe
{
	Q_OBJECT
public:
	virtual ~DummyProbe() {}
	virtual bool isValid() const
	{
		return mProbeData.getType() != ProbeDefinition::tNONE;
	}
	virtual QStringList getAvailableVideoSources() { return QStringList() << "active"; }
	virtual ProbeDefinition getProbeData(QString uid="active") const
	{
		return mProbeData;
	}
	virtual ProbeSectorPtr getSector(QString uid="active")
	{
		ProbeSectorPtr retval(new ProbeSector());
		retval->setData(this->getProbeData());
		return retval;
	}
	virtual VideoSourcePtr getRTSource(QString uid="active") const
	{
		return mVideoSource;
	}

	virtual void setActiveStream(QString uid) {}
	virtual QString getActiveStream() const { return mProbeData.getUid(); }

	virtual void addXml(QDomNode& dataNode) {}
	virtual void parseXml(QDomNode& dataNode) {}

	virtual QStringList getConfigIdList() const { return QStringList(); }
	virtual QString getConfigName(QString uid) { return QString(); }
	virtual QString getConfigId() const { return QString(); }
	virtual QString getConfigurationPath() const { return QString(); }

	virtual void applyNewConfigurationWithId(QString uid) {}
	virtual void setTemporalCalibration(double val) {}
	virtual void setSoundSpeedCompensationFactor(double val) {}
	virtual void setProbeSector(ProbeDefinition probeSector)
	{
		mProbeData = probeSector;
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
	ProbeDefinition mProbeData;
	VideoSourcePtr mVideoSource;
};

/**Helper class for emitting signals at a constant rate in a separate thread.
 *
 */
class DummyToolThread : public QThread
{
	Q_OBJECT
public:
	DummyToolThread(int interval, QObject* parent=NULL) : QThread(parent), mInterval(interval) {}
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

struct DummyToolTestUtilities
{
	static DummyToolPtr createDummyTool(ProbeDefinition probeData = ProbeDefinition(), TrackingServicePtr manager = TrackingServicePtr());
	static ProbeDefinition createProbeDataLinear(double depth=40, double width=50, Eigen::Array2i frameSize=Eigen::Array2i(80,40));
	static ProbeDefinition createProbeData(ProbeDefinition::TYPE, double depth=40, double width=50, Eigen::Array2i frameSize=Eigen::Array2i(80,40));
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
class DummyTool : public ToolImpl
{
	Q_OBJECT
public:
	explicit DummyTool(TrackingServicePtr manager, const QString& uid="dummytool");
	~DummyTool();
	virtual std::set<Type> getTypes() const;
	virtual void setType(Type);

	virtual QString getGraphicsFileName() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual void saveTransformsAndTimestamps();
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
	virtual ProbeDefinition getProbeSector() const { return mProbeData; }
	void setProbeSector( ProbeDefinition probeData )
	{
		mProbeData = probeData;
		mProbe.reset(new DummyProbe());
		mProbe->setProbeSector(mProbeData);
		emit toolProbeSector();
	}
	void setProbeSector(ProbePtr probe)
	{
		mProbe = probe;
		mProbeData = probe->getProbeData();
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

	vtkPolyDataPtr mPolyData;
	bool mVisible;
	QString mTransformSaveFileName;
	QTimerPtr mTimer;
	std::vector<Transform3D> mTransforms;

	static int mTransformCount;
//	Type mType;
	std::set<Type> mTypes;
	ProbeDefinition mProbeData;
	ProbePtr mProbe;
	DummyToolThread* mThread;
};
typedef boost::shared_ptr<DummyTool> DummyToolPtr;
}//namespace cx
#endif /* SSCDUMMYTOOL_H_ */
