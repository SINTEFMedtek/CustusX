// See DCMTK.cpp for header comments
#undef verify

#include <math.h>

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dctag.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcddirif.h"
#include "dcmtk/dcmdata/dcdirrec.h"
#include "dcmtk/dcmimage/diregist.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmjpeg/djencode.h"
#include "dcmtk/dcmjpeg/djrplol.h"

#include <QCoreApplication>
#include <QThread>

#include "sscLogger.h"
#include "DICOMLib.h"

#include "DCMTK.h"

//#define UID_ROOT "1.3.6.1.4.1.31283.1.2"			// using our unique PEN number (31283), assigned by IANA.
#define UID_ROOT "2.16.578.1.35"				// Assigned by the Norwegian Post and Telecommunications Authority.
#define STUDY_UID_ROOT UID_ROOT ".1.2"				// Following DCMTK usage, although not standards regulated.
#define SERIES_UID_ROOT UID_ROOT ".1.3"				// Ditto
#define INSTANCE_UID_ROOT UID_ROOT ".1.4"			// Ditto
#define FRAME_OF_REFERENCE_UID_ROOT UID_ROOT ".1.5"		// Just made this one up
#define FIDUCIAL_UID_ROOT UID_ROOT ".1.6"			// Ditto
#define SYNCHRONIZATION_UID_ROOT UID_ROOT ".1.7"		// Ditto


// C.7.1.1 Patient
static int exportPatientIE( DcmDataset *dataset, const struct study_t *study, bool anonymize )
{
	const char *patientName = "Doe^John";
	const char *patientID = "N/A";
	OFCondition result;

	if (!anonymize)
	{
		patientName = study->patientName;
		patientID = study->patientID;
	}
	result = dataset->putAndInsertString( DCM_PatientName, patientName );	// test first data entry
	if ( result.bad() )
	{
		SSC_LOG( "Failed to insert data" );
		return -1;
	}
	dataset->putAndInsertString( DCM_PatientID, patientID );
	if (anonymize)
	{
		dataset->insertEmptyElement( DCM_PatientBirthDate );
		dataset->putAndInsertString( DCM_PatientIdentityRemoved, "YES" );
		dataset->putAndInsertString( DCM_DeidentificationMethod, "Proprietary" );
		dataset->insertEmptyElement( DCM_PatientSex );
	}
	else
	{
		dataset->putAndInsertString( DCM_PatientSex, study->patientSex );	// can be either M, F or ... O
		dataset->putAndInsertString( DCM_PatientBirthDate, study->patientBirthDate );
	}
	return 0;
}

// C.7.2.1 General Study
static int exportStudyIE( DcmDataset *dataset, const struct study_t *study )
{
	DcmDate *studyDate = new DcmDate( DCM_StudyDate );	// FIXME!! This should be the time of investigation, NOT the time of export!
	DcmTime *studyTime = new DcmTime( DCM_StudyTime );

	studyDate->setCurrentDate();
	studyTime->setCurrentTime();
	dataset->putAndInsertString( DCM_StudyInstanceUID, study->studyInstanceUID );
	if(dataset->insert( studyDate ) != EC_Normal)
	{
		delete studyDate;
		studyDate = NULL;
	}
	if (dataset->insert( studyTime ) != EC_Normal)
	{
		delete studyTime;
		studyTime = NULL;
	}
	dataset->putAndInsertString( DCM_StudyID, study->studyID );
	dataset->insertEmptyElement( DCM_ReferringPhysicianName );	// maybe read this from the DICOM set and re-insert it later
	dataset->insertEmptyElement( DCM_AccessionNumber );		// not sure how this is used
	//dataset->putAndInsertString( DCM_PhysiciansOfRecord, "" );	// TODO add username from system here
	return 0;
}

// C.7.3.1 General Series
static int exportSeriesIE( DcmDataset *dataset, const struct series_t *series )
{
	char buf[DICOMLIB_VAL_MAX];

	dataset->putAndInsertString( DCM_SeriesInstanceUID, series->seriesInstanceUID );
	snprintf( buf, DICOMLIB_SHORT_STRING, "%d", series->series_id );	// internal DICOMLib number
	dataset->putAndInsertString( DCM_SeriesNumber, buf );
	if ( series->seriesDescription[0] != '\0' )
	{
		dataset->putAndInsertString( DCM_SeriesDescription, series->seriesDescription );
	}
	dataset->putAndInsertString( DCM_BodyPartExamined, "HEAD" );
	return 0;
}

// C.7.5.1 General Equipment and C.7.5.2 Enhanced General Equipment
static int exportEquipment( DcmDataset *dataset, const struct study_t *study )
{
	dataset->putAndInsertString( DCM_Manufacturer, "SONOWAND AS" );
	dataset->putAndInsertString( DCM_ManufacturerModelName, "SonoWand Invite" );
	dataset->putAndInsertString( DCM_InstitutionName, study->institutionName );
	dataset->putAndInsertString( DCM_DeviceSerialNumber, study->serialNumber );
	dataset->putAndInsertString( DCM_SoftwareVersions, study->softwareVersion );
	return 0;
}

int DICOMLib_LoadRegistration( const char *filename, int *numFiducials, double *fiducials, int *numCutplanes, double *cutplanes )
{
	OFCondition result;
	DcmFileFormat fileFormat;
	fileFormat.loadFile( filename );
	DcmDataset *dataset = fileFormat.getDataset();
	DcmSequenceOfItems *sequence;
	DcmItem *itemseq;
	int maxFiducials = *numFiducials;
	int maxCutplanes = *numCutplanes;

	*numFiducials = 0;
	*numCutplanes = 0;

	if ( dataset == NULL )
	{
		SSC_ERROR( "Failed to derive a dataset" );
		return -1;
	}

	// Ignore fluff, go straight to load of data
	if ( dataset->findAndGetSequenceItem( DCM_FiducialSetSequence, itemseq ).good() )
	{
		int i = 0;
		DcmItem *refseq;

		result = itemseq->findAndGetSequence( DCM_FiducialSequence, sequence );
		if ( !result.good() )
		{
			SSC_ERROR( "Failed to read fiducial sequence %u: %s", i, result.text() );
			return -1;
		}

		// First count the number of entries
		while (( refseq = sequence->getItem( i ) ))
		{
			OFString shapeType;
			Sint32 points;

			if ( refseq->findAndGetSint32( DCM_NumberOfContourPoints, points ).bad()
			     || refseq->findAndGetOFString( DCM_ShapeType, shapeType ).bad() )
			{
				SSC_LOG( "Failed to read fiducial sequence data index %d", i );
				i++;
				continue;
			}

			if ( shapeType == "POINT" && (*numFiducials) < maxFiducials )
			{
				for ( int j = 0; j < 3; j++ )
				{
					Float64 value;

					refseq->findAndGetFloat64( DCM_ContourData, value, j );
					fiducials[(*numFiducials) * 3 + j] = value;
				}
				(*numFiducials)++;
			}
			else if ( shapeType == "PLANE" && (*numCutplanes) < maxCutplanes )
			{
				for ( int j = 0; j < 9; j++ )
				{
					Float64 value;

					refseq->findAndGetFloat64( DCM_ContourData, value, j );
					cutplanes[(*numCutplanes) * 3 + j] = value;
				}
				(*numCutplanes)++;
			}
			else
			{
				SSC_LOG( "Loaded unknown fiducial type (%d): %s", i, shapeType.c_str() );
			}

			i++;
		}
	}
	return 0;
}

// Some inputs can be NULL
int DICOMLib_WriteRegistration( const char *filename, const struct study_t *study, const struct series_t *series,
								bool anonymize, int numFiducials, double *fiducials, int numCutplanes, double *cutplanes )
{
	OFCondition result;
	DcmDate *contentDate = new DcmDate( DCM_ContentDate );
	DcmTime *contentTime = new DcmTime( DCM_ContentTime );
	DcmItem *sequence;
	char uid[DICOMLIB_LONG_STRING];
	DcmFileFormat fileFormat;
	DcmDataset *dataset = fileFormat.getDataset();

	if ( dataset == NULL )
	{
		SSC_ERROR( "Failed to derive a dataset" );
		return -1;
	}

	contentDate->setCurrentDate();
	contentTime->setCurrentTime();

	// C.7.1.1 Patient
	exportPatientIE( dataset, study, anonymize );

	// C.7.2.1 General Study
	exportStudyIE( dataset, study );

	// C.7.3.1 General Series
	exportSeriesIE( dataset, series );

	// C.7.4.1 Frame of Reference
	dataset->putAndInsertString( DCM_FrameOfReferenceUID, series->frameOfReferenceUID );
	dataset->putAndInsertString( DCM_PositionReferenceIndicator, "Reference localizer" );		// Anatomical reference point, see C.7.4.1.1.2

	// C.7.5.1 General Equipment
	exportEquipment( dataset, study );

	// C.12.1 SOP Common
	dataset->putAndInsertString( DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier( uid, INSTANCE_UID_ROOT ) );

	// C.12.2 Common Instance Reference

	// C.21.1 Spatial Fiducials Series
	dataset->putAndInsertString( DCM_Modality, "FID" );

	// C.21.2 Spatial Fiducials
	dataset->insert( contentDate );
	dataset->insert( contentTime );
	if ( dataset->findOrCreateSequenceItem( DCM_FiducialSetSequence, sequence ).good() )
	{
		DcmItem *refseq;
		int i;

		sequence->putAndInsertString( DCM_FrameOfReferenceUID, series->frameOfReferenceUID );

		result = sequence->findOrCreateSequenceItem( DCM_FiducialSequence, refseq, numFiducials + numCutplanes - 1 );
		if ( !result.good() )
		{
			SSC_ERROR( "Failed to create %d Fiducial Sequence items: %s", numFiducials, result.text() );
			return -1;
		}
		for ( i = 0; i < numFiducials; i++ )
		{
			char buf[DICOMLIB_VAL_MAX];

			result = sequence->findAndGetSequenceItem( DCM_FiducialSequence, refseq, i );
			if ( !result.good() )
			{
				SSC_ERROR( "Could not read fiducial group sequence: %s", result.text() );
				return -1;
			}

			snprintf( buf, DICOMLIB_SHORT_STRING, "%d", i );
			refseq->putAndInsertString( DCM_FiducialIdentifier, buf );
			refseq->putAndInsertString( DCM_FiducialUID, dcmGenerateUniqueIdentifier( uid, FIDUCIAL_UID_ROOT ) );
			refseq->putAndInsertString( DCM_FiducialDescription, "Patient fiducial" );
			refseq->putAndInsertString( DCM_ShapeType, "POINT" );
			refseq->putAndInsertString( DCM_NumberOfContourPoints, "1" );
			snprintf( buf, DICOMLIB_LONG_STRING, "%f\\%f\\%f", fiducials[i * 3 + 0], fiducials[i * 3 + 1], fiducials[i * 3 + 2] );
			refseq->putAndInsertString( DCM_ContourData, buf );
		}
		for ( i = 0; i < numCutplanes; i++ )
		{
			char buf[DICOMLIB_VAL_MAX];

			result = sequence->findAndGetSequenceItem( DCM_FiducialSequence, refseq, i + numFiducials );
			if ( !result.good() )
			{
				SSC_ERROR( "Could not read fiducial group sequence for cut planes: %s", result.text() );
				return -1;
			}

			snprintf( buf, DICOMLIB_SHORT_STRING, "%d", i + numFiducials );
			refseq->putAndInsertString( DCM_FiducialIdentifier, buf );
			refseq->putAndInsertString( DCM_FiducialUID, dcmGenerateUniqueIdentifier( uid, FIDUCIAL_UID_ROOT ) );
			refseq->putAndInsertString( DCM_FiducialDescription, "Cut plane" );
			refseq->putAndInsertString( DCM_ShapeType, "PLANE" );
			refseq->putAndInsertString( DCM_NumberOfContourPoints, "3" );
			snprintf( buf, DICOMLIB_VAL_MAX, "%f\\%f\\%f\\%f\\%f\\%f\\%f\\%f\\%f", 
			          cutplanes[i * 9 + 0], cutplanes[i * 9 + 1], cutplanes[i * 9 + 2],
			          cutplanes[i * 9 + 3], cutplanes[i * 9 + 4], cutplanes[i * 9 + 5],
			          cutplanes[i * 9 + 6], cutplanes[i * 9 + 7], cutplanes[i * 9 + 8] );
			refseq->putAndInsertString( DCM_ContourData, buf );
		}
	}

	SSC_LOG( "Saving fiducials to %s", filename );
	unlink( filename );
	result = fileFormat.saveFile( filename, EXS_LittleEndianExplicit, EET_ExplicitLength );
	if ( result.bad() )
	{
		SSC_ERROR( "Failed to save %s: %s", filename, result.text() );
		return -1;
	}
	return 0;
}

// gforce must be a 4x4 matrix in row-major order
// TODO: Check frame of reference UID and matrix description
int DICOMLib_LoadGravity( const char *filename, double *gforce )
{
	OFCondition result;
	DcmFileFormat fileFormat;
	fileFormat.loadFile( filename );
	DcmDataset *dataset = fileFormat.getDataset();
	DcmItem *itemseq;

	if ( dataset == NULL )
	{
		SSC_LOG( "Failed to derive a dataset" );
		return -1;
	}

	// Ignore fluff, go straight to load of data
	if ( dataset->findAndGetSequenceItem( DCM_RegistrationSequence, itemseq ).good() )
	{
		DcmItem *refseq;

		if ( itemseq->findAndGetSequenceItem( DCM_MatrixRegistrationSequence, refseq ).good() )
		{
			DcmItem *finalseq;

			if ( refseq->findAndGetSequenceItem( DCM_MatrixSequence, finalseq ).good() )
			{
				for ( int j = 0; j < 16; j++ )
				{
					Float64 value;

					finalseq->findAndGetFloat64( DCM_FrameOfReferenceTransformationMatrix, value, j );
					gforce[j] = value;
				}
			}
			else SSC_ERROR( "Failed to enter DCM_MatrixSequence" );
		}
		else SSC_ERROR( "Failed to enter DCM_MatrixRegistrationSequence" );
	}
	else SSC_ERROR( "Failed to enter DCM_RegistrationSequence" );

	return 0;
}

// gforce must be a 4x4 matrix in row-major order
int DICOMLib_WriteGravity( const char *filename, const struct study_t *study, const struct series_t *series, bool anonymize, double *gforce )
{
	OFCondition result;
	DcmDate *contentDate = new DcmDate( DCM_ContentDate );
	DcmTime *contentTime = new DcmTime( DCM_ContentTime );
	DcmItem *sequence;
	DcmFileFormat fileFormat;
	DcmDataset *dataset = fileFormat.getDataset();
	char uid[DICOMLIB_LONG_STRING];

	contentDate->setCurrentDate();
	contentTime->setCurrentTime();

	// C.7.1.1 Patient
	exportPatientIE( dataset, study, anonymize );

	// C.7.2.1 General Study
	exportStudyIE( dataset, study );

	// C.7.3.1 General Series
	exportSeriesIE( dataset, series );

	// C.7.4.1 Frame of Reference
	dataset->putAndInsertString( DCM_FrameOfReferenceUID, series->frameOfReferenceUID );
	dataset->putAndInsertString( DCM_PositionReferenceIndicator, "HEAD" );		// Anatomical reference point, see C.7.4.1.1.2

	// C.7.5.1 General Equipment
	exportEquipment( dataset, study );

	// C.12.1 SOP Common
	dataset->putAndInsertString( DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier( uid, INSTANCE_UID_ROOT ) );

	// C.12.2 Common Instance Reference

	// C.20.1 Spatial Registration Series
	dataset->putAndInsertString( DCM_Modality, "REG" );

	// C.20.2 Spatial Registration
	dataset->insert( contentDate );
	dataset->insert( contentTime );
	if ( dataset->findOrCreateSequenceItem( DCM_RegistrationSequence, sequence ).good() )
	{
		DcmItem *itemseq;

		sequence->putAndInsertString( DCM_FrameOfReferenceUID, series->frameOfReferenceUID );

		if ( sequence->findOrCreateSequenceItem( DCM_MatrixRegistrationSequence, itemseq ).good() )
		{
			DcmItem *refseq;

			itemseq->putAndInsertString( DCM_FrameOfReferenceTransformationComment, "Direction of gravity" );
			itemseq->insertEmptyElement( DCM_RegistrationTypeCodeSequence );

			result = itemseq->findOrCreateSequenceItem( DCM_MatrixSequence, refseq );
			if ( result.good() )
			{
				char buf[DICOMLIB_VAL_MAX];

				ssprintf( buf, "%f", gforce[0] );
				for ( int i = 1; i < 16; i++ )
				{
					char tmp[DICOMLIB_SHORT_STRING];

					ssprintf( tmp, "\\%f", gforce[i] );
					sstrcat( buf, tmp );
				}
				refseq->putAndInsertString( DCM_FrameOfReferenceTransformationMatrix, buf );
				refseq->putAndInsertString( DCM_FrameOfReferenceTransformationMatrixType, "RIGID" );
			}
		}
	}

	SSC_LOG( "Saving gravity to %s", filename );
	unlink( filename );
	result = fileFormat.saveFile( filename, EXS_LittleEndianExplicit, EET_ExplicitLength );
	if ( result.bad() )
	{
		SSC_ERROR( "Failed to save %s: %s", filename, result.text() );
		return -1;
	}
	return 0;
}

const char *DICOMLib_FrameOfReferenceUID()
{
	char uid[DICOMLIB_LONG_STRING];
	static char frameOfReferenceUID[DICOMLIB_LONG_STRING];
	
	sstrcpy( frameOfReferenceUID, dcmGenerateUniqueIdentifier( uid, FRAME_OF_REFERENCE_UID_ROOT ) );
	
	return frameOfReferenceUID;
}

const char *DICOMLib_StudyUID()
{
	char uid[DICOMLIB_LONG_STRING];
	static char studyUID[DICOMLIB_LONG_STRING];
	
	sstrcpy( studyUID, dcmGenerateUniqueIdentifier( uid, STUDY_UID_ROOT ) );
	
	return studyUID;
}

const char *DICOMLib_SeriesUID()
{
	char uid[DICOMLIB_LONG_STRING];
	static char seriesUID[DICOMLIB_LONG_STRING];
	
	sstrcpy( seriesUID, dcmGenerateUniqueIdentifier( uid, SERIES_UID_ROOT ) );
	
	return seriesUID;
}

static int writeCommonInfo( DcmDataset *dataset, const struct study_t *study, const struct series_t *series, bool anonymize )
{
	DcmDate *contentDate = new DcmDate( DCM_ContentDate );
	DcmTime *contentTime = new DcmTime( DCM_ContentTime );
	char uid[DICOMLIB_LONG_STRING];
	const struct volume_t *volume = series->volume;
	char buf[DICOMLIB_VAL_MAX];
	bool ultrasound = strcmp(series->modality, "US") == 0;
	OFCondition result;

	memset( uid, 0, sizeof( uid ) );

	// Reset this information on each export. TODO: Don't.
	contentDate->setCurrentDate();
	contentTime->setCurrentTime();

	// C.7.1.1 Patient
	exportPatientIE( dataset, study, anonymize );

	// C.7.2.1 General Study
	exportStudyIE( dataset, study );

	// C.7.3.1 General Series
	exportSeriesIE( dataset, series );
	dataset->putAndInsertString( DCM_Modality, series->modality );

	// C.7.4.1 Frame of Reference
	dataset->putAndInsertString( DCM_FrameOfReferenceUID, series->frameOfReferenceUID );
	dataset->putAndInsertString( DCM_PositionReferenceIndicator, "HEAD" );		// Anatomical reference point, see C.7.4.1.1.2

	// C.7.5.1 General Equipment
	exportEquipment( dataset, study );

	// C.7.6.1 General Image
	if (dataset->insert( contentDate ) != EC_Normal)
	{
		delete contentDate;
		contentDate = NULL;
	}
	if (dataset->insert( contentTime ) != EC_Normal)
	{
		delete contentTime;
		contentTime = NULL;
	}
	ssprintf( buf, "%u", series->series_id );
	dataset->putAndInsertString( DCM_InstanceNumber, buf );	// has to be unique for SOP instances in a study
	dataset->insertEmptyElement( DCM_PatientOrientation );	// makes no sense for US - patient can be faced any direction!
	if (ultrasound)
	{
		dataset->putAndInsertString( DCM_ImageType, "DERIVED\\PRIMARY\\VOLUME\\RESAMPLED" );
	}
	else
	{
		dataset->putAndInsertString( DCM_ImageType, "DERIVED\\SECONDARY\\VOLUME\\NONE" );
	}

	// C.7.6.3 Image Pixel
	dataset->putAndInsertUint16( DCM_SamplesPerPixel, volume->samples_per_pixel );
	dataset->putAndInsertUint16( DCM_BitsAllocated, volume->bits_per_sample );
	dataset->putAndInsertUint16( DCM_BitsStored, volume->bits_per_sample );
	dataset->putAndInsertUint16( DCM_HighBit, volume->bits_per_sample - 1 );
	dataset->putAndInsertUint16( DCM_Rows, volume->y );
	dataset->putAndInsertUint16( DCM_Columns, volume->x );
	dataset->putAndInsertUint16( DCM_PixelRepresentation, 0 );

	// Export palette LUT (only for angio US)
	if (ultrasound && series->us.flowData && volume->samples_per_pixel == 1 && series->VOI.lut.table)
	{
		uint16_t LUTDescriptor[] = { 256, 0, 16 };		// size, start, bitsize
		unsigned char *lut = (unsigned char *)series->VOI.lut.table;
		uint16_t red[256], green[256], blue[256];

		dataset->putAndInsertString( DCM_PixelPresentation, "COLOR" );		// Supplemental color palettes exist
		if ( (result = dataset->putAndInsertUint16Array( DCM_RedPaletteColorLookupTableDescriptor, LUTDescriptor, 3 )).bad()
		     || (result = dataset->putAndInsertUint16Array( DCM_GreenPaletteColorLookupTableDescriptor, LUTDescriptor, 3 )).bad()
		     || (result = dataset->putAndInsertUint16Array( DCM_BluePaletteColorLookupTableDescriptor, LUTDescriptor, 3 )).bad() )
		{
			SSC_ERROR( "Couldn't write LUT Descriptor: %s", result.text() );
		}
		for (int i = 0; i < 256; i++)
		{
			red[i] = lut[i * 4 + 0];
			green[i] = lut[i * 4 + 1];
			blue[i] = lut[i * 4 + 2];
		}
		if ( (result = dataset->putAndInsertUint16Array( DCM_RedPaletteColorLookupTableData, red, 256 )).bad()
		     || (result = dataset->putAndInsertUint16Array( DCM_GreenPaletteColorLookupTableData, green, 256 )).bad()
		     || (result = dataset->putAndInsertUint16Array( DCM_BluePaletteColorLookupTableData, blue, 256 )).bad() )
		{
			SSC_ERROR( "Ooops! Couldn't write LUT: %s", result.text() );
		}
	}

	// C.8.6.1 SC Equipment
	dataset->putAndInsertString( DCM_ConversionType, "WSD" );	// Workstation

	// C.11.2 VOI LUT
	{
		char tmp[PATH_MAX];
		ssprintf(tmp, "%f", series->VOI.current.center - volume->rescaleIntercept);
		if ( dataset->putAndInsertString( DCM_WindowCenter, tmp).good() == false ) SSC_LOG("insert DCM_WindowCenter failed");

		ssprintf(tmp, "%i", -1*volume->rescaleIntercept);
		if ( dataset->putAndInsertString( DCM_RescaleIntercept, tmp).good() == false ) SSC_LOG("insert DCM_RescaleIntercept failed");

		if ( dataset->putAndInsertString( DCM_RescaleSlope, "1" ).good()==false ) SSC_LOG("insert DCM_RescaleSlope=\"1\" failed");

		// for Compuer Tomography we set the type to Hounsfield Units (HU)
		if ( strcmp(series->modality, "CT") == 0 )
		{
			if ( dataset->putAndInsertString( DCM_RescaleType, "HU").good()==false ) SSC_LOG("insert DCM_RescaleType HU failed");
		}
		// else we do not have an idea what this should be -> "US" (unspecified)
		else if ( dataset->putAndInsertString( DCM_RescaleType, "US").good()==false ) SSC_LOG("insert DCM_RescaleType US failed");

		ssprintf(tmp, "%f", series->VOI.current.width);
		dataset->putAndInsertString( DCM_WindowWidth, tmp );
	}

	// C.12.1 SOP Common
	dataset->putAndInsertString( DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier( uid, INSTANCE_UID_ROOT ) );

	return 0;
}

static int writeUnenhancedFrame( const char *filename, const struct study_t *study, bool anonymize, enum dicomlib_application_profile profile, int frame )
{
	const struct series_t *series = study->first_series;
	const struct volume_t *volume = series->volume;
	const struct instance_t *instance = series->first_instance;
	DcmFileFormat fileFormat;
	DcmDataset *dataset = fileFormat.getDataset();
	char buf[DICOMLIB_VAL_MAX];
	OFCondition result;
	int i;

	for ( i = 0; i < frame && instance; i++ )
	{
		instance = instance->next_instance;
	}
	if (!instance)
	{
		SSC_ERROR("Did not find instance %d", i);
		return -1;
	}
	if ( dataset == NULL )
	{
		SSC_ERROR( "Failed to derive a dataset" );
		return -1;
	}
	if ( volume->bits_per_sample != 8 && volume->bits_per_sample != 16 )
	{
		SSC_ERROR( "Bad bits per sample" );
		return EINVAL;
	}
	writeCommonInfo( dataset, study, series, anonymize );

	// C.7.6.3 Image Pixel (cont.)
	dataset->putAndInsertString( DCM_SOPClassUID, UID_SecondaryCaptureImageStorage );
	if ( volume->samples_per_pixel == 1 && volume->bits_per_sample == 8 )
	{
		const uint8_t *ptr = (const uint8_t *)volume->volume + volume->x * volume->y * frame;

		dataset->putAndInsertString( DCM_PhotometricInterpretation, "MONOCHROME2" );
		dataset->putAndInsertUint8Array( DCM_PixelData, ptr, volume->x * volume->y );
	}
	else if ( volume->samples_per_pixel == 1 && volume->bits_per_sample == 16 )
	{
		const uint16_t *ptr = (const uint16_t *)volume->volume + volume->x * volume->y * frame;

		dataset->putAndInsertString( DCM_PhotometricInterpretation, "MONOCHROME2" );
		dataset->putAndInsertUint16Array( DCM_PixelData, ptr, volume->x * volume->y );
	}
	else if ( volume->samples_per_pixel == 3 )
	{
		const uint8_t *ptr = (const uint8_t *)volume->volume + volume->x * volume->y * frame * volume->samples_per_pixel;

		dataset->putAndInsertUint16( DCM_PlanarConfiguration, 0 );	// interleaved
		dataset->putAndInsertString( DCM_PhotometricInterpretation, "RGB" );
		dataset->putAndInsertUint8Array( DCM_PixelData, ptr, volume->x * volume->y * volume->samples_per_pixel );
	}
	else
	{
		SSC_LOG( "DICOM_write: Bad samples per pixel" );
		return EINVAL;
	}

	// C.7.6.2 Image Plane
	ssprintf( buf, "%f\\%f", volume->pixel_spacing[0], volume->pixel_spacing[1] );
	dataset->putAndInsertString( DCM_PixelSpacing, buf );
	ssprintf( buf, "%f", volume->pixel_spacing[2] );
	dataset->putAndInsertString( DCM_SliceThickness, buf );
	ssprintf( buf, "%f\\%f\\%f\\%f\\%f\\%f", volume->image_orientation[0], volume->image_orientation[1],
		  volume->image_orientation[2], volume->image_orientation[3], volume->image_orientation[4], volume->image_orientation[5] );
	dataset->putAndInsertString( DCM_ImageOrientationPatient, buf );
	ssprintf( buf, "%f\\%f\\%f", instance->image_position[0], instance->image_position[1], instance->image_position[2] );
	dataset->putAndInsertString( DCM_ImagePositionPatient, buf );

	/*** Save ***/

	unlink( filename );
	if ( profile == DICOMLIB_PROFILE_USB )
	{
		DJ_RPLossless params; // codec parameters, we use the defaults

		dataset->chooseRepresentation(EXS_JPEGProcess14SV1TransferSyntax, &params);
		result = fileFormat.saveFile( filename, EXS_JPEGProcess14SV1TransferSyntax, EET_ExplicitLength );
	}
	else	// DVD/CD
	{
		result = fileFormat.saveFile( filename, EXS_LittleEndianExplicit, EET_ExplicitLength );
	}
	if ( result.bad() )
	{
		SSC_WARNING( "Failed to save %s: %s", filename, result.text() );
		return -1;
	}

	return 0;
}

// Always aligned
int DICOMLib_WriteUnenhancedSeries( const char *basefilename, const struct study_t *study, bool anonymize, enum dicomlib_application_profile profile )
{
	char path[PATH_MAX];
	int i;
	const struct series_t *series = study->first_series;

	for (i = 0; i < series->frames; i++)
	{
		ssprintf(path, "%s_%03d", basefilename, i);			// FIXME: Tool long filename, valid DICOM is only 8 chars max, put in dir?
		writeUnenhancedFrame(path, study, anonymize, profile, i);
	}
	return 0;
}

// Write a snapshot to DICOM SC. Anonymization is not an option here, since it is only relevant for PACS.
int DICOMLib_WriteImageSnapshot( const char *filename, const uint8_t *rgb, int bytes, int width, int height, const struct study_t *study, bool compress )
{
	DcmFileFormat fileFormat;
	DcmDataset *dataset = fileFormat.getDataset();
	char buf[DICOMLIB_VAL_MAX];
	char uid[DICOMLIB_LONG_STRING];
	DcmDate *contentDate = new DcmDate( DCM_ContentDate );
	DcmTime *contentTime = new DcmTime( DCM_ContentTime );
	DcmDate *instanceDate = new DcmDate( DCM_InstanceCreationDate );
	DcmTime *instanceTime = new DcmTime( DCM_InstanceCreationTime );
	DcmDateTime *acqDateTime = new DcmDateTime( DCM_AcquisitionDateTime );
	OFCondition result;
	DJ_RPLossless params; // codec parameters, we use the defaults

	if ( dataset == NULL )
	{
		SSC_ERROR( "Failed to derive a dataset" );
		return -1;
	}

	SSC_LOG("Exporting %s with %d bytes, %d pixels", filename, bytes, bytes / 3);
	dataset->putAndInsertString( DCM_SOPClassUID, UID_SecondaryCaptureImageStorage );

	memset( uid, 0, sizeof( uid ) );

	// Reset this information on each export.
	instanceDate->setCurrentDate();
	instanceTime->setCurrentTime();
	// TODO: The below should be time of snapshot!
	contentDate->setCurrentDate(); // putString( converted timestamp of snapshot )
	contentTime->setCurrentTime();
	acqDateTime->setCurrentDateTime();

	// C.7.1.1 Patient
	exportPatientIE( dataset, study, false );

	// C.7.2.1 General Study
	exportStudyIE( dataset, study );

	// C.7.6.1 General Image
	dataset->putAndInsertString( DCM_ImageType, "DERIVED\\SECONDARY" );

	// C.7.6.3 Image Pixel
	dataset->putAndInsertUint16( DCM_SamplesPerPixel, 3 );
	dataset->putAndInsertUint16( DCM_BitsAllocated, 8 );
	dataset->putAndInsertUint16( DCM_BitsStored, 8 );
	dataset->putAndInsertUint16( DCM_HighBit, 7 );
	dataset->putAndInsertUint16( DCM_Rows, height );
	dataset->putAndInsertUint16( DCM_Columns, width );
	dataset->putAndInsertUint16( DCM_PixelRepresentation, 0 );
	dataset->putAndInsertUint16( DCM_PlanarConfiguration, 0 );	// interleaved
	dataset->putAndInsertString( DCM_PhotometricInterpretation, "RGB" );
	dataset->putAndInsertUint8Array( DCM_PixelData, rgb, bytes );

	// C.7.3.1 General Series
	dataset->putAndInsertString( DCM_Modality, "DOC" );
	dataset->putAndInsertString( DCM_SeriesInstanceUID, dcmGenerateUniqueIdentifier( buf, SERIES_UID_ROOT ) );
	dataset->insertEmptyElement( DCM_SeriesNumber );
	dataset->putAndInsertString( DCM_SeriesDescription, "SonoWand Invite snapshot" );
	dataset->putAndInsertString( DCM_BodyPartExamined, "HEAD" );

	// C.7.6.1 General Image
	dataset->insert( contentDate );
	dataset->insert( contentTime );
	dataset->insert( acqDateTime );
	dataset->insertEmptyElement( DCM_InstanceNumber );
	dataset->insertEmptyElement( DCM_PatientOrientation );	// makes no sense
	dataset->putAndInsertString( DCM_BurnedInAnnotation, "YES" );

	// C.8.6.1 SC Equipment
	dataset->putAndInsertString( DCM_ConversionType, "WSD" );	// Workstation

	// C.12.1 SOP Common
	dataset->putAndInsertString( DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier( uid, INSTANCE_UID_ROOT ) );
	dataset->insert( instanceDate );
	dataset->insert( instanceTime );

	/*** Save ***/

	unlink( filename );
	if ( compress )
	{
		dataset->chooseRepresentation( EXS_JPEGProcess14SV1TransferSyntax, &params );
		result = fileFormat.saveFile( filename, EXS_JPEGProcess14SV1TransferSyntax, EET_ExplicitLength );
	}
	else
	{
		result = fileFormat.saveFile( filename, EXS_LittleEndianExplicit, EET_ExplicitLength );
	}
	if ( result.bad() )
	{
		SSC_WARNING( "Failed to save %s: %s", filename, result.text() );
		return -1;
	}
	return 0;
}

// Write a snapshot to encapsulated PDF. Anonymization is not an option here, since it is only relevant for PACS.
int DICOMLib_WriteSnapshot( const char *filename, const uint8_t *pdfbuffer, int size, const struct study_t *study )
{
	DcmFileFormat fileFormat;
	DcmDataset *dataset = fileFormat.getDataset();
	char buf[DICOMLIB_VAL_MAX];
	char uid[DICOMLIB_LONG_STRING];
	DcmDate *contentDate = new DcmDate( DCM_ContentDate );
	DcmTime *contentTime = new DcmTime( DCM_ContentTime );
	DcmDate *instanceDate = new DcmDate( DCM_InstanceCreationDate );
	DcmTime *instanceTime = new DcmTime( DCM_InstanceCreationTime );
	DcmDateTime *acqDateTime = new DcmDateTime( DCM_AcquisitionDateTime );
	OFCondition result;

	if ( dataset == NULL )
	{
		SSC_ERROR( "Failed to derive a dataset" );
		return -1;
	}

	dataset->putAndInsertString( DCM_SOPClassUID, UID_EncapsulatedPDFStorage );

	memset( uid, 0, sizeof( uid ) );

	// Reset this information on each export.
	instanceDate->setCurrentDate();
	instanceTime->setCurrentTime();
	// TODO: The below should be time of snapshot!
	contentDate->setCurrentDate(); // putString( converted timestamp of snapshot )
	contentTime->setCurrentTime();
	acqDateTime->setCurrentDateTime();

	// C.7.1.1 Patient
	exportPatientIE( dataset, study, false );

	// C.7.2.1 General Study
	exportStudyIE( dataset, study );

	// C.24.1 Encapsulated Document Series
	dataset->putAndInsertString( DCM_Modality, "DOC" );
	dataset->putAndInsertString( DCM_SeriesInstanceUID, dcmGenerateUniqueIdentifier( buf, SERIES_UID_ROOT ) );
	dataset->putAndInsertString( DCM_SeriesNumber, "0" );	// TODO - think about this, index of snapshot?
	dataset->putAndInsertString( DCM_SeriesDescription, "SonoWand Invite snapshot" );
	dataset->putAndInsertString( DCM_BodyPartExamined, "HEAD" );

	// C.24.2 Encapsulated Document
	dataset->putAndInsertString( DCM_InstanceNumber, "0" );	// TODO - think about this, index of snapshot?
	dataset->insert( contentDate );
	dataset->insert( contentTime );
	dataset->insert( acqDateTime );
	dataset->putAndInsertString( DCM_BurnedInAnnotation, "YES" );
	dataset->insertEmptyElement( DCM_DocumentTitle );
	dataset->insertEmptyElement( DCM_ConceptNameCodeSequence );
	dataset->putAndInsertString( DCM_MIMETypeOfEncapsulatedDocument, "application/pdf" );	// see A.45.1.4.1
	dataset->putAndInsertUint8Array( DCM_EncapsulatedDocument, pdfbuffer, size );

	// C.8.6.1 SC Equipment
	dataset->putAndInsertString( DCM_ConversionType, "WSD" );	// Workstation

	// C.12.1 SOP Common
	dataset->putAndInsertString( DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier( uid, INSTANCE_UID_ROOT ) );
	dataset->insert( instanceDate );
	dataset->insert( instanceTime );

	/*** Save ***/

	unlink( filename );
	result = fileFormat.saveFile( filename, EXS_LittleEndianExplicit, EET_ExplicitLength );
	if ( result.bad() )
	{
		SSC_WARNING( "Failed to save %s: %s", filename, result.text() );
		return -1;
	}
	return 0;
}

class ExportThread : public QThread
{
public:
	ExportThread(DcmFileFormat *fileFormat, const char *filename, const E_TransferSyntax syntax, const E_EncodingType encoding)
	{
		mFileFormat = fileFormat;
		mFileName = filename;
		mSyntax = syntax;
		mEncoding = encoding;
	}
	virtual void run()
	{
		mResult = mFileFormat->saveFile( mFileName, mSyntax, mEncoding );
	}
	OFCondition result() { return mResult; }
private:
	DcmFileFormat *mFileFormat;
	const char *mFileName;
	E_TransferSyntax mSyntax;
	E_EncodingType mEncoding;
	OFCondition mResult;
};

// Before calling this function, remember to change frame of reference UID if the object coordinate system has changed.
// Also note that when there is duplicate information between the various structs, we prefer volume over series and series over instance.
int DICOMLib_WriteSeries( const char *filename, const struct study_t *study, bool anonymize, enum dicomlib_application_profile profile,
						  enum dicomlib_volume_alignment aligned )
{
	const struct series_t *series = study->first_series;
	const struct volume_t *volume = series->volume;
	const struct instance_t *instance = series->first_instance;
	DcmFileFormat fileFormat;
	DcmDataset *dataset = fileFormat.getDataset();
	char buf[DICOMLIB_VAL_MAX];
	char uid[DICOMLIB_LONG_STRING];
	DcmItem *sequence;
	int volSize = volume->x * volume->y * volume->z;
	OFCondition result;
	int i;
	OFString sliceVector;
	bool ultrasound = strcmp(series->modality, "US") == 0;

	if ( dataset == NULL )
	{
		SSC_ERROR( "Failed to derive a dataset" );
		return -1;
	}

	if ( volume->bits_per_sample != 8 && volume->bits_per_sample != 16 )
	{
		SSC_ERROR( "Bad bits per sample" );
		return EINVAL;
	}

	writeCommonInfo( dataset, study, series, anonymize );
	
	// Tags borrowed from new 3D US modality
	if (ultrasound)
	{
		// C.8.24.3 Enhanced US Image
		result = dataset->findOrCreateSequenceItem( DCM_TransducerScanPatternCodeSequence, sequence );
		if ( result.good() )
		{
			sequence->putAndInsertString( DCM_CodeValue, "125241" );
			sequence->putAndInsertString( DCM_CodingSchemeDesignator, "DCM" );
			sequence->putAndInsertString( DCM_CodeMeaning, "Plane scan pattern" );
		}
		else
		{
			SSC_ERROR( "Failed to create Transducer Scan Pattern Code Sequence: %s", result.text() );
			return -1;
		}
		result = dataset->findOrCreateSequenceItem( DCM_TransducerGeometryCodeSequence, sequence );
		if ( result.good() )
		{
			sequence->putAndInsertString( DCM_CodingSchemeDesignator, "DCM" );
			if ( series->us.probeType == DICOMLIB_PROBE_LINEAR )
			{
				sequence->putAndInsertString( DCM_CodeValue, "125252" );
				sequence->putAndInsertString( DCM_CodeMeaning, "Linear ultrasound transducer geometry" );
			}
			else if ( series->us.probeType == DICOMLIB_PROBE_SECTOR )
			{
				sequence->putAndInsertString( DCM_CodeValue, "125254" );
				sequence->putAndInsertString( DCM_CodeMeaning, "Sector ultrasound transducer geometry" );
			}
			else
			{
				SSC_ERROR( "Unhandled probe geometry: %d!", (int)series->us.probeType );
			}
		}
		else
		{
			SSC_ERROR( "Failed to create Transducer Geometry Code Sequence: %s", result.text() );
		}
		if ( dataset->findOrCreateSequenceItem( DCM_TransducerBeamSteeringCodeSequence, sequence ).good() )
		{
			sequence->putAndInsertString( DCM_CodingSchemeDesignator, "DCM" );
			if ( series->us.probeType == DICOMLIB_PROBE_LINEAR )
			{
				sequence->putAndInsertString( DCM_CodeValue, "125259" );
				sequence->putAndInsertString( DCM_CodeMeaning, "Phased beam steering" );
			}
			else if ( series->us.probeType == DICOMLIB_PROBE_SECTOR )
			{
				sequence->putAndInsertString( DCM_CodeValue, "125257" );
				sequence->putAndInsertString( DCM_CodeMeaning, "Fixed beam direction" );
			}
			else
			{
				SSC_ERROR( "Unhandled probe beam steering!" );
			}
		}
		if ( dataset->findOrCreateSequenceItem( DCM_TransducerApplicationCodeSequence, sequence ).good() )
		{
			sequence->putAndInsertString( DCM_CodeValue, "125261" );
			sequence->putAndInsertString( DCM_CodingSchemeDesignator, "DCM" );
			sequence->putAndInsertString( DCM_CodeMeaning, "External Transducer" );
		}
		dataset->putAndInsertString( DCM_DimensionOrganizationType, "3D" );
		dataset->putAndInsertString( DCM_PositionMeasuringDeviceUsed, "RIGID" );
		dataset->putAndInsertString( DCM_TransducerData, series->us.transducerData );
		ssprintf( buf, "%f", series->us.TI );
		dataset->putAndInsertString( DCM_SoftTissueThermalIndex, buf );
		dataset->putAndInsertString( DCM_BoneThermalIndex, buf );						// same calculated result as above
		ssprintf( buf, "%f", series->us.MI );
		dataset->putAndInsertString( DCM_MechanicalIndex, buf );
		dataset->putAndInsertString( DCM_UltrasoundAcquisitionGeometry, "UNDEFINED" );
		OFDateTime ofdateTime;
		ofdateTime.setISOFormattedDateTime( series->us.acquisitionDateTime );
		DcmDateTime *acqDateTime = new DcmDateTime( DCM_AcquisitionDateTime );
		acqDateTime->setOFDateTime( ofdateTime );
		dataset->insert( acqDateTime );
		dataset->putAndInsertFloat64( DCM_AcquisitionDuration, series->us.acquisitionDuration );

		// Private tags
		dataset->putAndInsertString( DcmTag(0x0135, 0x0010, EVR_LO), "SONOWAND AS");
		dataset->putAndInsertString( DcmTag(0x0135, 0x1010, EVR_LO), series->us.scannerName );		// Ultrasound Scanner Name
		dataset->putAndInsertString( DcmTag(0x0135, 0x1011, EVR_LO), series->us.transducerSerial );	// Transducer Serial
		dataset->putAndInsertString( DcmTag(0x0135, 0x1012, EVR_LO), series->us.application );		// Probe application

		// C.8.24.2 Ultrasound Frame Of Reference
		dataset->putAndInsertString( DCM_UltrasoundAcquisitionGeometry, "FREEHAND" );
		dataset->putAndInsertString( DCM_VolumeFrameOfReferenceUID, series->frameOfReferenceUID );
	}

	// C.8.24.2 Ultrasound Frame Of Reference cont., also add some of it for non-US because it is generally useful information
	switch (aligned)
	{
	case DICOMLIB_ALIGNMENT_TABLE		: dataset->insertEmptyElement( DCM_PatientFrameOfReferenceSource ); break;
	case DICOMLIB_ALIGNMENT_ESTIMATED	: dataset->putAndInsertString( DCM_PatientFrameOfReferenceSource, "ESTIMATED" ); break;
	case DICOMLIB_ALIGNMENT_REGISTRATION	: dataset->putAndInsertString( DCM_PatientFrameOfReferenceSource, "REGISTRATION" ); break;
	}

	writeCommonInfo( dataset, study, series, anonymize );

	// C.7.4.2 Synchronization
	dataset->putAndInsertString( DCM_SynchronizationFrameOfReferenceUID, dcmGenerateUniqueIdentifier( uid, SYNCHRONIZATION_UID_ROOT ) );
	dataset->putAndInsertString( DCM_SynchronizationTrigger, "NO TRIGGER" );
	dataset->putAndInsertString( DCM_AcquisitionTimeSynchronized, "N" );

	// C.7.6.3 Image Pixel (cont.)
#if 0
	// TODO later
	dataset->putAndInsertSint16( DCM_SmallestImagePixelValue, (Sint16)volume->firstpixel );
	dataset->putAndInsertSint16( DCM_LargestImagePixelValue, (Sint16)volume->lastpixel );
#endif
	if ( volume->samples_per_pixel == 1 && volume->bits_per_sample == 8 )
	{
		/*** Multi-frame Grayscale Byte Secondary Capture Image, see PS3.3 A.8.3 ***/
		dataset->putAndInsertString( DCM_PhotometricInterpretation, "MONOCHROME2" );
		dataset->putAndInsertUint8Array( DCM_PixelData, ( const uint8_t * )volume->volume, volSize );
		dataset->putAndInsertString( DCM_SOPClassUID, UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage );
	}
	else if ( volume->samples_per_pixel == 1 && volume->bits_per_sample == 16 )
	{
		/*** Multi-frame Grayscale Word Secondary Capture Image, see PS3.3 A.8.4 ***/
		dataset->putAndInsertString( DCM_PhotometricInterpretation, "MONOCHROME2" );
		dataset->putAndInsertUint16Array( DCM_PixelData, ( const uint16_t * )volume->volume, volSize );
		dataset->putAndInsertString( DCM_SOPClassUID, UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage );
	}
	else if ( volume->samples_per_pixel == 3 )
	{
		/*** Multi-frame True Color Secondary Capture Image, see PS3.3 A.8.5 ***/
		dataset->putAndInsertUint16( DCM_PlanarConfiguration, 0 );	// interleaved
		dataset->putAndInsertString( DCM_PhotometricInterpretation, "RGB" );
		dataset->putAndInsertUint8Array( DCM_PixelData, ( const uint8_t * )volume->volume, volSize * volume->samples_per_pixel );
		dataset->putAndInsertString( DCM_SOPClassUID, UID_MultiframeTrueColorSecondaryCaptureImageStorage );
	}
	else
	{
		SSC_LOG( "DICOM_write: Bad samples per pixel" );
		return EINVAL;
	}

	// C.7.6.6 Multi-frame
	ssprintf( buf, "%d", series->frames );
	dataset->putAndInsertString( DCM_NumberOfFrames, buf );
	uint16_t incrArray[2] = { 0x0018, 0x2005 };
	dataset->putAndInsertUint16Array( DCM_FrameIncrementPointer, incrArray, 1 );	// Set type of indexing. See C.7.6.6.1.1

	// C.7.6.14 Acquisition Context
	if ( dataset->findOrCreateSequenceItem( DCM_AcquisitionContextSequence, sequence ).good() )
	{
		// Do nothing. Is type 2 and we have no useful data to add.
	}

	// C.7.6.16 Multi-frame Functional Groups
	// First the shared group
	if ( dataset->findOrCreateSequenceItem( DCM_SharedFunctionalGroupsSequence, sequence ).good() )
	{
		DcmItem *shared;

		// C.7.6.16.2.1 Pixel Measures
		if ( sequence->findOrCreateSequenceItem( DCM_PixelMeasuresSequence, shared ).good() )
		{
			ssprintf( buf, "%f\\%f", volume->pixel_spacing[0], volume->pixel_spacing[1] );
			shared->putAndInsertString( DCM_PixelSpacing, buf );
			ssprintf( buf, "%f", volume->pixel_spacing[2] );
			shared->putAndInsertString( DCM_SliceThickness, buf );
		}
		// C.7.6.16.2.4 Plane Orientation
		if ( aligned != DICOMLIB_ALIGNMENT_TABLE
			 && sequence->findOrCreateSequenceItem( DCM_PlaneOrientationSequence, shared ).good() )
		{
			ssprintf( buf, "%f\\%f\\%f\\%f\\%f\\%f", volume->image_orientation[0], volume->image_orientation[1],
			          volume->image_orientation[2], volume->image_orientation[3], volume->image_orientation[4], volume->image_orientation[5] );
			shared->putAndInsertString( DCM_ImageOrientationPatient, buf );
		}
		// C.7.6.16.2.10 Frame VOI LUT
		if ( sequence->findOrCreateSequenceItem( DCM_FrameVOILUTSequence, shared ).good() )
		{
			char tmp[PATH_MAX];

			ssprintf(tmp, "%f", series->VOI.current.center - (double)volume->rescaleIntercept);
			shared->putAndInsertString(DCM_WindowCenter, tmp);
			ssprintf(tmp, "%f", series->VOI.current.width);
			shared->putAndInsertString( DCM_WindowWidth, tmp );
		}
		// C.7.6.16.2.24 Image Data Type
		if ( ultrasound && sequence->findOrCreateSequenceItem( DCM_ImageDataTypeSequence, shared ).good() )
		{
			if ( !series->us.flowData )
			{
				shared->putAndInsertString( DCM_DataType, "TISSUE_INTENSITY" );
			}
			else
			{
				shared->putAndInsertString( DCM_DataType, "FLOW_POWER" );
			}
			shared->putAndInsertString( DCM_AliasedDataType, "YES" );
		}
		// C.8.24.6.1 US Image Description
		if ( ultrasound && sequence->findOrCreateSequenceItem( DCM_USImageDescriptionSequence, shared ).good() )
		{
			shared->putAndInsertString( DCM_FrameType, "DERIVED\\PRIMARY\\VOLUME\\RESAMPLED" );
			shared->putAndInsertString( DCM_VolumetricProperties, "VOLUME" );
			shared->putAndInsertString( DCM_VolumeBasedCalculationTechnique, "NONE" );
		}
	}

	// Then the per frame group
	if ( !dataset->findOrCreateSequenceItem( DCM_PerFrameFunctionalGroupsSequence, sequence, series->frames - 1 ).good() )
	{
		SSC_ERROR( "Could not write per frame functional group sequence" );
		return -1;
	}
	for ( i = 0; i < series->frames; i++ )
	{
		DcmItem *item = NULL;

		if ( !instance )
		{
			SSC_ERROR( "Incomplete list of instances for frame set! Stopped at %d", i );
			return -1;
		}
		if ( !dataset->findAndGetSequenceItem( DCM_PerFrameFunctionalGroupsSequence, sequence, i ).good() )
		{
			SSC_ERROR( "Could not read per frame functional group sequence at %d", i );
			return -1;
		}
		if ( sequence->findOrCreateSequenceItem( DCM_FrameContentSequence, item ).good() )
		{
			item->putAndInsertUint16( DCM_FrameAcquisitionNumber, i );
		}
		if ( aligned != DICOMLIB_ALIGNMENT_TABLE
			 && !sequence->findOrCreateSequenceItem( DCM_PlanePositionSequence, item ).good() )
		{
			SSC_ERROR( "Could not write plane position sequence at %d", i );
			return -1;
		}
		if ( item )
		{
			ssprintf( buf, "%f\\%f\\%f", instance->image_position[0], instance->image_position[1], instance->image_position[2] );
			item->putAndInsertString( DCM_ImagePositionPatient, buf );
		}
		instance = instance->next_instance;
	}

	// C.8.6.1 SC Equipment
	dataset->putAndInsertString( DCM_ConversionType, "WSD" );	// Workstation

	// C.8.6.3 SC Multi-frame Image
	dataset->putAndInsertString( DCM_BurnedInAnnotation, "NO" );
	/* if ( volume->samples_per_pixel == 1 )
	{
		dataset->putAndInsertString( DCM_PresentationLUTShape, "IDENTITY" );
		dataset->putAndInsertString( DCM_RescaleIntercept, "0" );
		dataset->putAndInsertString( DCM_RescaleSlope, "1" );
		dataset->putAndInsertString( DCM_RescaleType, "US" );	// unspecified
	}*/

	// C.8.6.4 SC Multi-frame Vector
	instance = series->first_instance;
	for ( i = 0; i < series->frames; i++ )
	{
		char tmp[128];

		if ( !instance )
		{
			SSC_ERROR( "Incomplete list of instances for slice vector!" );
			return -1;
		}
		ssprintf( tmp, "%f", instance->slice_normal_distance );
		sliceVector += tmp;
		if ( instance->next_instance )
		{
			sliceVector += '\\';
		}
		instance = instance->next_instance;
	}
	dataset->putAndInsertOFStringArray( DCM_SliceLocationVector, sliceVector );	// array of frame positions in mm, see C.7.6.2.1.2

	// C.12.1 SOP Common
	dataset->putAndInsertString( DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier( uid, INSTANCE_UID_ROOT ) );
	/*** Save ***/

	unlink( filename );

	E_TransferSyntax syntax;
	if ( profile == DICOMLIB_PROFILE_USB )
	{
		DJ_RPLossless params; // codec parameters, we use the defaults

		syntax = EXS_JPEGProcess14SV1TransferSyntax;
		dataset->chooseRepresentation(syntax, &params);
	}
	else	// DVD/CD
	{
		syntax = EXS_LittleEndianExplicit;
	}
	ExportThread thread(&fileFormat, filename, syntax, EET_ExplicitLength);
	thread.start();
	while (!thread.isFinished())
	{
		qApp->processEvents();
		thread.wait(1000);
	}
	if ( thread.result().bad() )
	{
		SSC_WARNING( "Failed to save %s: %s", filename, result.text() );
		return -1;
	}

	return 0;
}
