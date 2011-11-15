#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DICOMLib.h"

#include "ui_cyDICOM.h"

class MainWindow : public QMainWindow,
	private Ui::MainWindow
{
	Q_OBJECT
	public:
	MainWindow( QWidget *parent = 0 );
	~MainWindow();

	protected:
	void setupActions();

	protected slots:
	void loadStudy();
	void loadDICOMDIR();
	void loadPACS();
	void exportDICOM();
	void settingsPACS();
	void autoVOI();
	void noVOI();
	void setWidth( int w );
	void setCenter( int c );
	void setReject( int r );

	private:
	void errmsg( const char *msg );
	struct study_t *studyList;
	struct study_t *currentStudy;
	struct series_t *seriesList;
	struct series_t *currentSeries;
	const struct volume_t *volume;
	int sizes[3];
	GLuint texnames[1];
	void processStudy( struct study_t *study );
	void processSeries( struct series_t *series, int series_count );
	GLint internalFormat;
	GLenum pixelFormat;
	GLenum pixelType;
};

#endif
