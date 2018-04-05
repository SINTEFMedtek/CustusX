/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRAININGWIDGET_H_
#define CXTRAININGWIDGET_H_

#include "cxBaseWidget.h"
#include "boost/shared_ptr.hpp"
#include "org_custusx_training_Export.h"
#include <boost/function.hpp>
#include "cxForwardDeclarations.h"
class ctkPluginContext;
class QPushButton;

namespace cx {

typedef boost::shared_ptr<class HelpEngine> HelpEnginePtr;
class HelpBrowser;


/**
 * Top-level help widget
 *
 * \ingroup org_custusx_training
 *
 * \date 2016-03-14
 * \author Christian Askeland
 * \author Janne Beate Bakeng
 */
class org_custusx_training_EXPORT TrainingWidget : public BaseWidget
{
	Q_OBJECT

public:
	explicit TrainingWidget(RegServicesPtr services, QString objectName, QString windowTitle, QWidget* parent = NULL);
	virtual ~TrainingWidget();

protected:
    void resetSteps();

    typedef boost::function<void(void)> func_t;
    typedef std::vector<func_t> funcs_t;
    void registrateTransition( func_t transition);

protected:
	void makeUnavailable(QString uidPart, bool makeModalityUnavailable = false);
	void makeAvailable(QString uidPart, bool makeModalityUnavailable);
	RegServicesPtr mServices;

	QString getFirstUSVolume();
	MeshPtr getMesh(QString uidPart);
private slots:
    void onImportSimulatedPatient();

private:
    void createActions();
    void createSteps(unsigned numberOfSteps);
    CXToolButton *addToolButtonFor(QHBoxLayout *layout, QAction *action);
    void toWelcomeStep();
    void onStep(int delta);
    void stepTo(int step);
    void transitionToStep(int step);
	void setAvailability(std::map<QString, DataPtr> datas, bool available, QString uidPart, bool makeModalityUnavailable);

    HelpEnginePtr mEngine;
    HelpBrowser* mBrowser;

    funcs_t mTransitions;

    int mCurrentStep;
    QAction* mPreviousAction;
    QAction* mNextAction;
    QAction* mCurrentAction;
    QAction* mImportAction;
	QStringList mSessionIDs;
};

} /* namespace cx */
#endif /* CXTRAININGWIDGET_H_ */
