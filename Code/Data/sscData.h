#ifndef SSCDATA_H_
#define SSCDATA_H_

#include <string>
#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <QObject>
#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkMatrix4x4> vtkMatrix4x4Ptr;
#include "sscTransform3D.h"
#include "sscRegistrationTransform.h"

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
	Data(const std::string& uid, const std::string& name="");
	virtual ~Data();

	void setUid(const std::string& uid);
	void setName(const std::string& name);
	void setFilePath(const std::string& filePath);///< Set current file path to file
	void setRegistrationStatus(REGISTRATION_STATUS regStat);
	//virtual void set_rMd(Transform3D rMd); ///< set the transformation from data to reference space
  virtual RegistrationHistoryPtr get_rMd_History();

	virtual std::string getUid() const; ///< \return unique id for this instance
	virtual std::string getName() const; /// \return a descriptive name for this instance
	virtual std::string getFilePath() const; ///< \return the corresponding file path
	virtual REGISTRATION_STATUS getRegistrationStatus() const; ///< \return what kind of registration that has been performed on this data object.
	virtual Transform3D get_rMd() const; ///< \return the transform M_rd from the data object's space (d) to the reference space (r).
  virtual void setShading(bool on);
  virtual bool getShading() const;

  virtual std::string getParentFrame();
  virtual void setParentFrame(std::string uid);

	void connectToRep(const RepWeakPtr& rep); ///< called by Rep when connecting to an Image
	void disconnectFromRep(const RepWeakPtr& rep); ///< called by Rep when disconnecting from an Image

  virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
  virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

signals:
  void transformChanged(); ///< emitted when transform is changed

protected slots:
  virtual void transformChangedSlot() {}

protected:
	std::string mUid;
	std::string mName;
	std::string mFilePath;
	std::string mParentFrame;

	REGISTRATION_STATUS mRegistrationStatus;
	//Transform3D m_rMd; ///< the transform from data to reference space
	RegistrationHistoryPtr m_rMd_History;

	std::set<RepWeakPtr> mReps; ///< links to Rep users.

};
  
typedef boost::shared_ptr<Data> DataPtr;

} // end namespace ssc

#endif /*SSCDATA_H_*/
