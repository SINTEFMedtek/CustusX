/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIGTLINKWIDGET_H_
#define CXIGTLINKWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

#include <vector>
#include <boost/shared_ptr.hpp>
#include <QProcess>
#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"
#include "cxVisServices.h"

class QPushButton;
class QComboBox;
class QLineEdit;
class QStackedWidget;
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace cx
{
class SimulateUSWidget;
class FileInputWidget;
class StreamerService;
class XmlOptionFile;
class DetailedLabeledComboBoxWidget;
typedef boost::shared_ptr<class VideoConnectionManager> VideoConnectionManagerPtr;
typedef boost::shared_ptr<class StringPropertyActiveVideoSource> StringPropertyActiveVideoSourcePtr;
typedef boost::shared_ptr<class StringProperty> StringPropertyPtr;
typedef boost::shared_ptr<class Tool> ToolPtr;
typedef boost::shared_ptr<class VisServices> VisServicesPtr;

/**
 * \brief GUI for setting up a connection to a video stream
 *
 * \ingroup cx_gui
 *
 * \date 2010.10.27
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 *
 */
class cxGui_EXPORT VideoConnectionWidget : public BaseWidget
{
  Q_OBJECT

public:
	VideoConnectionWidget(VisServicesPtr services, QWidget* parent);
	virtual ~VideoConnectionWidget();

protected slots:
	void toggleConnectServer();
	void serverStatusChangedSlot();
	void importStreamImageSlot();
	void setConnectionMethodAndUpdateGuiForConnectionMethodSlot();
	void connectionMethodChangedSlot();
	void onServiceAdded(StreamerService *service);
	void onServiceRemoved(StreamerService *service);

protected:
	StringPropertyActiveVideoSourcePtr initializeActiveVideoSourceSelector();
	QFrame* wrapStackedWidgetInAFrame();
	QPushButton* initializeConnectButton();
	QPushButton* initializeImportStreamImageButton();
	QWidget* wrapVerticalStretch(QWidget* input);
	Transform3D calculate_rMd_ForAProbeImage(ToolPtr probe);
	QString generateFilename(VideoSourcePtr videoSource);
	void saveAndImportSnapshot(vtkImageDataPtr input, QString filename, Transform3D rMd);

	QPushButton* mConnectButton;
	QPushButton* mImportStreamImageButton;
	QVBoxLayout* mToptopLayout;
	QStackedWidget* mStackedWidget;
	StringPropertyPtr mConnectionSelector;
	StringPropertyActiveVideoSourcePtr mActiveVideoSourceSelector;
	XmlOptionFile mOptions;
	DetailedLabeledComboBoxWidget* mConnectionSelectionWidget;
	QFrame* mStackedWidgetFrame;

	VisServicesPtr mServices;

private:
	QWidget* createStreamerWidget(StreamerService* service);
	void addServiceToSelector(StreamerService *service);
	void removeServiceFromSelector(StreamerService *service);
	void removeServiceWidget(QString name);
	void addExistingStreamerServices();

	std::map<QString, QWidget*> mStreamerServiceWidgets;
	bool mUpdateConnectionInService;
};

}//end namespace cx

#endif /* CXIGTLINKWIDGET_H_ */
