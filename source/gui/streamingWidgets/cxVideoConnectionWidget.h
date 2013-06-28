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

#ifndef CXIGTLINKWIDGET_H_
#define CXIGTLINKWIDGET_H_

#include "cxBaseWidget.h"

#include <vector>
#include <boost/shared_ptr.hpp>
#include <QProcess>
#include "sscTransform3D.h"
#include "sscForwardDeclarations.h"

class QPushButton;
class QComboBox;
class QLineEdit;
class QStackedWidget;
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace ssc
{
	class FileSelectWidget;
	typedef boost::shared_ptr<class StringDataAdapterXml> StringDataAdapterXmlPtr;
	typedef boost::shared_ptr<class Tool> ToolPtr;
}

namespace cx
{
typedef boost::shared_ptr<class VideoConnectionManager> VideoConnectionManagerPtr;
typedef boost::shared_ptr<class ActiveVideoSourceStringDataAdapter> ActiveVideoSourceStringDataAdapterPtr;

class SimulateUSWidget;

/**
 * \brief GUI for setting up a connection to a video stream
 *
 * \ingroup cxGUI
 *
 * \date 2010.10.27
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 *
 */
class VideoConnectionWidget : public BaseWidget
{
  Q_OBJECT

public:
	VideoConnectionWidget(QWidget* parent);
	virtual ~VideoConnectionWidget();
	virtual QString defaultWhatsThis() const;

protected slots:
	void toggleLaunchServer();
	void launchServer();
	void toggleConnectServer();
	void serverProcessStateChanged(QProcess::ProcessState newState);
	void serverStatusChangedSlot();
	void browseLocalServerSlot();
	void importStreamImageSlot();
	void selectGuiForConnectionMethodSlot();
	void initScriptSelected(QString filename);

protected:
	void connectServer();
	void disconnectServer();
	QHBoxLayout* initializeScriptWidget();
	ssc::StringDataAdapterXmlPtr initializeConnectionSelector();
	ActiveVideoSourceStringDataAdapterPtr initializeActiveVideoSourceSelector();
	QFrame* wrapStackedWidgetInAFrame();
	void updateHostHistory();
	void updateDirectLinkArgumentHistory();
	QProcess* getServerProcess();
	bool serverIsRunning();
	VideoConnectionManagerPtr getVideoConnectionManager();
	void writeSettings();
	QPushButton* initializeConnectButton();
	QPushButton* initializeImportStreamImageButton();
	QStackedWidget* initializeStackedWidget();
	QWidget* createDirectLinkWidget();
	QWidget* createLocalServerWidget();
	QWidget* createRemoteWidget();
	QWidget* createSimulationWidget();
	QWidget* wrapVerticalStretch(QWidget* input);
	ssc::Transform3D calculate_rMd_ForAProbeImage(ssc::ToolPtr probe);
	QString generateFilename(ssc::VideoSourcePtr videoSource);
	void saveAndImportSnapshot(vtkImageDataPtr input, QString filename, ssc::Transform3D rMd);

	QPushButton* mConnectButton;
	QPushButton* mImportStreamImageButton;
	QVBoxLayout* mToptopLayout;
	ssc::FileSelectWidget* mInitScriptWidget;
	QComboBox* mAddressEdit;
	QLineEdit* mPortEdit;
	QLineEdit* mLocalServerEdit;
	QLineEdit* mLocalServerArguments;
	QPushButton* mLaunchServerButton;
	QComboBox* mDirectLinkArguments;
	QStackedWidget* mStackedWidget;
	ssc::StringDataAdapterXmlPtr mConnectionSelector;
	ActiveVideoSourceStringDataAdapterPtr mActiveVideoSourceSelector;
	SimulateUSWidget* mSimulationWidget;

};

}//end namespace cx

#endif /* CXIGTLINKWIDGET_H_ */
