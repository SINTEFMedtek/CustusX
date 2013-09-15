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
 * \brief cx implementation of additional functionality for the DataManager
 * \ingroup cxServicePatient
 *
 * \date Mar 23, 2009
 * \author Janne Beate Bakeng, SINTEF
 */
class cxDataManager: public DataManagerImpl
{
Q_OBJECT
public:
	static cxDataManager* getInstance();
	virtual ~cxDataManager();

	static void initialize();
	bool getDebugMode() const;
	virtual PresetTransferFunctions3DPtr getPresetTransferFunctions3D() const;

signals:
	void debugModeChanged(bool on);

public slots:
	void setDebugMode(bool on);

protected:
	cxDataManager(); ///< DataManager is a Singleton. Use getInstance instead

	bool mDebugMode; ///< if set: allow lots of weird debug stuff.

private:
	cxDataManager(cxDataManager const&);
	cxDataManager& operator=(cxDataManager const&);

	mutable PresetTransferFunctions3DPtr mPresetTransferFunctions3D;
};

/**
 * @}
 */
} //namespace cx

#endif /* CXDATAMANAGER_H_ */
