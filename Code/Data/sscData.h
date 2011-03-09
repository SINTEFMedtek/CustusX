#ifndef SSCDATA_H_
#define SSCDATA_H_

#include <string>
#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <QObject>
#include "vtkForwardDeclarations.h"
#include "sscTransform3D.h"

#include <QDateTime>
class QDomNode;

namespace ssc
{
typedef boost::weak_ptr<class Rep> RepWeakPtr;
typedef boost::shared_ptr<class RegistrationHistory> RegistrationHistoryPtr;

/**Denotes the various states
 * of registration an object is in.
 */
enum REGISTRATION_STATUS
{
	rsNOT_REGISTRATED,
	rsIMAGE_REGISTRATED,
	rsPATIENT_REGISTRATED
};

/**Superclass for all data objects.
 * Example suclassess: Image and Surface.
 */
class Data : public QObject
{
  Q_OBJECT
public:
	Data(); //TODO remove?
	Data(const QString& uid, const QString& name="");
	virtual ~Data();

	void setUid(const QString& uid);
	void setName(const QString& name);
	void setFilePath(const QString& filePath);///< Set current file path to file
	void setAcquisitionTime(QDateTime time);
	void setRegistrationStatus(REGISTRATION_STATUS regStat);
	//virtual void set_rMd(Transform3D rMd); ///< set the transformation from data to reference space
  virtual RegistrationHistoryPtr get_rMd_History();

	virtual QString getUid() const; ///< \return unique id for this instance
	virtual QString getName() const; /// \return a descriptive name for this instance
	virtual QString getFilePath() const; ///< \return the corresponding file path
	virtual REGISTRATION_STATUS getRegistrationStatus() const; ///< \return what kind of registration that has been performed on this data object.
	virtual Transform3D get_rMd() const; ///< \return the transform M_rd from the data object's space (d) to the reference space (r).
  virtual void setShading(bool on);
  virtual bool getShading() const;
  virtual QDateTime getAcquisitionTime() const;

  virtual QString getParentFrame();
//  virtual void setParentFrame(QString uid);

	void connectToRep(const RepWeakPtr& rep); ///< called by Rep when connecting to an Image
	void disconnectFromRep(const RepWeakPtr& rep); ///< called by Rep when disconnecting from an Image

  virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
  virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

signals:
  void transformChanged(); ///< emitted when transform is changed

protected slots:
  virtual void transformChangedSlot() {}

protected:
	QString mUid;
	QString mName;
	QString mFilePath;
	QDateTime mAcquisitionTime;
//	QString mParentFrame;

	REGISTRATION_STATUS mRegistrationStatus;
	//Transform3D m_rMd; ///< the transform from data to reference space
	RegistrationHistoryPtr m_rMd_History;

	std::set<RepWeakPtr> mReps; ///< links to Rep users.

};
  
typedef boost::shared_ptr<Data> DataPtr;

} // end namespace ssc

#endif /*SSCDATA_H_*/
