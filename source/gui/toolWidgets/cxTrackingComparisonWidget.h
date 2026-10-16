/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKINGCOMPARISONWIDGET_H_
#define CXTRACKINGCOMPARISONWIDGET_H_

#include "cxGuiExport.h"

#include <map>
#include "cxBaseWidget.h"
#include "cxTrackingSystemComparison.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;
class QTableWidget;

namespace cx
{

/** \brief Runs the same tools with several tracking systems in turn (IGSTK, NDI, OpenIGTLink) and records the positions.
 *
 * \ingroup cx_gui
 */
class cxGui_EXPORT TrackingComparisonWidget : public BaseWidget
{
	Q_OBJECT
public:
	TrackingComparisonWidget(TrackingServicePtr trackingService, QWidget* parent = NULL);
	virtual ~TrackingComparisonWidget();

	static std::vector<TrackingComparisonRun> createRuns(QStringList implementations, QString openIGTLinkCommand, QString openIGTLinkArguments);
	static QStringList findToolConfigurations(QStringList folders); ///< The .xml files in the folders, each file name once, sorted
	static QString getTrackingSystemUid(QString implementation); ///< The uid of the TrackingSystemService for a tracking system implementation

private:
	void startOrCancel();
	void browseToolConfiguration();
	void addToolConfiguration(QString path, QString name);
	QWidget* createToolConfigurationSelector();
	QWidget* createTrackingSystemSelector(TrackingServicePtr trackingService);
	static QStringList getToolConfigurationFolders();
	void onFinished(bool success);
	void showResults();
	static void addLine(class QGridLayout* layout, QString label, QWidget* widget);
	QStringList getSelectedImplementations() const;
	void saveSettings();

	TrackingSystemComparison mComparison;
	QComboBox* mToolConfigFile;
	std::map<QString, QCheckBox*> mImplementations;
	QLineEdit* mOpenIGTLinkCommand;
	QLineEdit* mOpenIGTLinkArguments;
	QCheckBox* mOpenIGTLinkApplyReference;
	QSpinBox* mDuration;
	QLineEdit* mOutputFolder;
	QPushButton* mStartButton;
	QPlainTextEdit* mLog;
	QTableWidget* mResultTable;
};

} // namespace cx

#endif /* CXTRACKINGCOMPARISONWIDGET_H_ */
