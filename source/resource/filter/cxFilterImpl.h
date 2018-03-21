/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILTERIMPL_H
#define CXFILTERIMPL_H

#include "cxResourceFilterExport.h"

#include <vector>
#include <QObject>
#include "cxFilter.h"
#include <QDomElement>
#include <boost/shared_ptr.hpp>

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;

/** Helper implementation for Filter.
 *
 * Inherit from this class to get access to useful functionality common
 * to Filter descendants.
 *
 * \ingroup cxResourceAlgorithms
 * \date Nov 21, 2012
 * \author Christian Askeland, SINTEF
 */
class cxResourceFilter_EXPORT FilterImpl : public Filter
{
	Q_OBJECT

public:
	virtual ~FilterImpl() {}

	virtual QString getUid() const;
	virtual void initialize(QDomElement root, QString uid = "");
	virtual std::vector<PropertyPtr> getOptions();
	virtual std::vector<SelectDataStringPropertyBasePtr> getInputTypes();
	virtual std::vector<SelectDataStringPropertyBasePtr> getOutputTypes();
	virtual bool hasPresets() { return false; }
	virtual PresetsPtr getPresets() { return PresetsPtr(); }
	virtual QDomElement generatePresetFromCurrentlySetOptions(QString name) { return QDomElement(); }
	virtual void setActive(bool on);
	virtual bool preProcess();

public slots:
	virtual void requestSetPresetSlot(QString name) {}

protected:
	explicit FilterImpl(VisServicesPtr services);

	/** Helper: Return the index'th input type as an image.
	  * Return zero if not available (as image). */
	ImagePtr getCopiedInputImage(int index = 0);
	/** Helper:
	  * Call if you have an image threshold Property that need update from an image change.
	  */
	void updateThresholdFromImageChange(QString uid, DoublePropertyPtr threshold);
	void updateThresholdPairFromImageChange(QString uid, DoublePairPropertyPtr threshold);

	virtual void createOptions() = 0;
	virtual void createInputTypes() = 0;
	virtual void createOutputTypes() = 0;

	std::vector<SelectDataStringPropertyBasePtr> mInputTypes;
	std::vector<SelectDataStringPropertyBasePtr> mOutputTypes;
	std::vector<PropertyPtr> mOptionsAdapters;
	QDomElement mOptions;

	// data used by execute - copied for thread safety purposes
	std::vector<DataPtr> mCopiedInput;
	QDomElement mCopiedOptions;
	bool mActive;
	VisServicesPtr mServices;
	PatientModelServicePtr patientService();

private:
	QString mUid;

};

} // namespace cx


#endif // CXFILTERIMPL_H
