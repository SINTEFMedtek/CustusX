/*
 * sscDICOMWidget.h
 *
 *  \date Nov 15, 2011
 *      \author christiana
 */
#ifndef SSCDICOMWIDGET_H_
#define SSCDICOMWIDGET_H_

#include "sscDICOMLibConfig.h"

#ifdef SSC_USE_DCMTK

#include <QWidget>
#include <QtGui>
#include "sscForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class DICOMLibAPI> DICOMLibAPIPtr;
typedef boost::shared_ptr<class Series> SeriesPtr;

/** Widget for import of DICOM images.
 * \ingroup cxGUI
 *
 */
class DICOMWidget : public QWidget
{
	Q_OBJECT
public:
	DICOMWidget(QWidget* parent = NULL);
	virtual ~DICOMWidget();
//	void setImageSavePath(QString savePath);

private slots:
	void selectRootFolder();
	void rootFolderEditeditingFinished();
	void itemSelectionChanged();
	void dicomChanged();
	void loadSeries();
	void showMetaData();

	void refreshStartedSlot();
	void refreshProgressSlot(int value);
	void refreshFinishedSlot();

private:
	SeriesPtr getSelectedSeries();

 	DICOMLibAPIPtr mApi;
// 	QString mImageSavePath;

	QLineEdit* mRootFolderEdit;
	QToolButton* mSelectRootFolderButton;
	QToolButton* mLoadSeriesButton;
	QToolButton* mShowMetaDataButton;
	QProgressBar* mProgressBar;
	QAction* mSelectRootFolderAction;
	QAction* mLoadSeriesAction;
	QAction* mShowMetaDataAction;
	QTableWidget* mTable; ///< the table widget presenting the landmarks
};

}

#endif // SSC_USE_DCMTK

#endif /* SSCDICOMWIDGET_H_ */
