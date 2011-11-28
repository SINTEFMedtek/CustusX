/*
 * sscDICOMWidget.h
 *
 *  Created on: Nov 15, 2011
 *      Author: christiana
 */

#ifndef SSCDICOMWIDGET_H_
#define SSCDICOMWIDGET_H_

#include <QWidget>
#include <QtGui>
#include "sscForwardDeclarations.h"

namespace ssc
{
typedef boost::shared_ptr<class DICOMLibAPI> DICOMLibAPIPtr;
typedef boost::shared_ptr<class Series> SeriesPtr;

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

#endif /* SSCDICOMWIDGET_H_ */
