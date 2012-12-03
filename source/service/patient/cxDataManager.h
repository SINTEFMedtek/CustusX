#ifndef CXDATAMANAGER_H_
#define CXDATAMANAGER_H_

#include <sscDataManagerImpl.h>
#include <QDomNode>

class QDomDocument;

namespace cx
{
/**
 * \file
 * \addtogroup cxServicePatient
 * @{
 */

/**
 * \brief cx implementation of additional functionality for the ssc::DataManager
 * \ingroup cxServicePatient
 *
 * \date Mar 23, 2009
 * \author Janne Beate Bakeng, SINTEF
 */
class DataManager: public ssc::DataManagerImpl
{
Q_OBJECT
public:
	static DataManager* getInstance();
	virtual ~DataManager();

	static void initialize();
	bool getDebugMode() const;
	virtual ssc::PresetTransferFunctions3DPtr getPresetTransferFunctions3D() const;

signals:
	void debugModeChanged(bool on);

public slots:
	void setDebugMode(bool on);

protected:
	DataManager(); ///< DataManager is a Singleton. Use getInstance instead

	bool mDebugMode; ///< if set: allow lots of weird debug stuff.

private:
	DataManager(DataManager const&);
	DataManager& operator=(DataManager const&);

	mutable ssc::PresetTransferFunctions3DPtr mPresetTransferFunctions3D;
};

/**
 * @}
 */
} //namespace cx

#endif /* CXDATAMANAGER_H_ */
