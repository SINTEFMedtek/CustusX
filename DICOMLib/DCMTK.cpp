/*
 *  DCMTK.cpp
 *  DICOMLib
 *
 *  Created by Per M on 07/3/2.
 *  Copyright 2006-2008 SONOWAND AS. All rights reserved.
 *
 */

/* Ah, so Apple in its infinite wisdom decides to create a macro called "verify" in a standard
 * system header file that it seems nearly impossible to make XCode to stop automatically including...
 * This breaks DCMTK badly. */
#undef verify

#include <math.h>
#include <limits>
#include <iconv.h>

#include "DCMTK.h"

#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dctag.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcddirif.h"
#include "dcmtk/dcmdata/dcdirrec.h"
#include "dcmtk/dcmimage/diregist.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmjpeg/djencode.h"
#include "dcmtk/dcmjpeg/djcodecd.h"
#include "dcmtk/dcmjpeg/djdecode.h"
#include "dcmtk/dcmjpls/djencode.h"
#include "dcmtk/dcmjpls/djcodecd.h"
#include "dcmtk/dcmjpls/djdecode.h"
#include "dcmtk/dcmdata/dcrleenc.h"
#include "dcmtk/dcmdata/dcrleccd.h"
#include "dcmtk/dcmdata/dcrleerg.h"
#include "dcmtk/dcmdata/dcrledrg.h"
#include "dcmtk/dcmimage/dicopxt.h"

#include <QStringList>

#include "sscLogger.h"
#include "DICOMLib.h"

#include "vector3d.h"

#ifndef DBL_EPSILON
#define DBL_EPSILON 0.00001f
#endif

#ifndef UINT32_MAX
#define UINT32_MAX             (4294967295U)
#endif

//* private Siemens DTI tags */
DcmTagKey SiemensDCM_DiffusionDirectionality(0x0019,0x100d);
DcmTagKey SiemensDCM_DiffusionGradientOrientation(0x0019,0x100e);
DcmTagKey SiemensDCM_DiffusionBValue(0x0019,0x100c);
//* private Philips DTI Tags */
DcmTagKey PhilipsDCM_DiffusionDirectionality(0x2001,0x1004); // O (Orientation?)
DcmTagKey PhilipsDCM_DiffusionGradientDirectionRL(0x2005,0x10b0);
DcmTagKey PhilipsDCM_DiffusionGradientDirectionAP(0x2005,0x10b1);
DcmTagKey PhilipsDCM_DiffusionGradientDirectionFH(0x2005,0x10b2);
DcmTagKey PhilipsDCM_DiffusionBValue(0x2001,0x1003);

struct 
{
	char appAETitle[256];
	char peerAETitle[256];
	char destAETitle[256];
	char peerIP[256];
	int peerPort;
} static conf;

typedef struct {
	T_ASC_Association *assoc;
	T_ASC_PresentationContextID presId;
	struct study_t **studyList;
	bool cancel;
} MyCallbackInfo;

int DICOM_Init()
{
	DJLSDecoderRegistration::registerCodecs();
	DcmRLEEncoderRegistration::registerCodecs();
	DcmRLEDecoderRegistration::registerCodecs();
	DJEncoderRegistration::registerCodecs();
	DJDecoderRegistration::registerCodecs();
	return 0;
}

int DICOM_Done()
{
	DJLSDecoderRegistration::cleanup();
	DcmRLEEncoderRegistration::cleanup();
	DcmRLEDecoderRegistration::cleanup();
	DJEncoderRegistration::cleanup();
	DJDecoderRegistration::cleanup();
	return 0;
}

/// Convert string to system default encoding
bool convert_string( char *input, const char from_encoding[] )
{
	const char tocode[] = "UTF-8";
	size_t inleft, outleft, converted = 0;
	char *output, *outbuf, *tmp;
	const char *inbuf;
	size_t outlen;
	iconv_t cd;
	char fromcode[strlen(from_encoding)];

	/* Mapping from DICOM SpecificCharacterSet to iconv speak
	 * see DICOM standard C.12.1.1.2  and cmd: 'iconv --list' */
	strcpy (fromcode,from_encoding);
	for( unsigned int i = 0; i < (strlen(fromcode)); i++ )
	{
	     switch(fromcode[i])
	     {
		     case ' ':
			     fromcode[i] = '-';
			     break;
		     case '_':
			     fromcode[i] = '-';
			     break;
	     }
	}


	if (strcmp (fromcode,"ISO-IR-192") == 0 ) // UTF-8
	{
		if ((iconv_t)(-1) == ( cd = iconv_open( tocode, "UTF-8" )))
		{
			SSC_LOG("Failed to iconv_open %s to UTF-8", fromcode);
			return false;
		}
	}
	else if (strcmp (fromcode,"ISO-2022-IR-13") == 0 ) // Japan
	{
		if ((iconv_t)(-1) == ( cd = iconv_open( tocode, "ISO2022JP" )))
		{
			SSC_LOG("Failed to iconv_open %s to UTF-8", fromcode);
			return false;
		}
	}
	else if ((iconv_t)(-1) == ( cd = iconv_open( tocode, fromcode )))
	{
		SSC_LOG("Failed to iconv_open %s to UTF-8 trying now ISO8859-1 as default ...", fromcode);
		if ( (iconv_t)(-1) == ( cd = iconv_open( tocode, "ISO8859-1" ) ) )
		{
			SSC_LOG("ISO8859-1 to UTF-8 failed too");
			return false;
		}
		else
		{
			SSC_LOG("ISO8859-1 to UTF-8 seems to work ...");
		}
	}

	// SSC_LOG("from encoding %s convert: %s",from_encoding, input);

	inleft = strlen (input);
	inbuf = input;

	outlen = inleft;

	/* we allocate 4 bytes more for nul-termination */
	if (!(output = (char*)malloc (outlen + 4)))
	{
		iconv_close (cd);
		return false;
	}

	do
	{
		errno = 0;
		outbuf = output + converted;
		outleft = outlen - converted;
		converted = iconv (cd, (char **) &inbuf, &inleft, &outbuf, &outleft);
		if (converted != (size_t) -1 || errno == EINVAL)
		{
			break;
		}

		if (errno != E2BIG)
		{
		iconv_close (cd);
		free (output);
		return false;
		}

		converted = outbuf - output;
		outlen += inleft * 2 + 8;

		if (!(tmp = (char*)realloc (output, outlen + 4)))
		{
			iconv_close (cd);
			free (output);
			return false;
		}

		output = tmp;
		outbuf = output + converted;
	} while (1);

	iconv (cd, NULL, NULL, &outbuf, &outleft);
	iconv_close (cd);

	// null termination:
	memset (outbuf, 0, 4);

	strcpy (input, output);

	return true;
}

/// Read string
static bool dicomtag_string( DcmItem *dset, char *target, const DcmTagKey &tagKey, int len )
{
	OFString value;

	/* Get the character set encoding  from DICOM data set for later conversion*/
	char specificCharacterSet [DICOMLIB_LONG_STRING];
	if ( !dset->findAndGetOFString( DCM_SpecificCharacterSet, value ).good() )
	{
		strcpy (specificCharacterSet,"ISO_IR 192");
		//SSC_LOG( "%s: No DCM_SpecificCharacterSet tag found.", tagKey.toString().c_str() );
	}
	else
	{
		strlcpy( specificCharacterSet, value.c_str(), DICOMLIB_LONG_STRING );
		//SSC_LOG( "%s: DCM_SpecificCharacterSet tag found: %s", tagKey.toString().c_str(),specificCharacterSet );
	}

	if ( !dset->findAndGetOFString( tagKey, value ).good() )
	{
		target[0] = '\0';
		return false;
	}
	strlcpy( target, value.c_str(), len );

	if ( !convert_string(target,specificCharacterSet) )
	{
		SSC_LOG("String conversion of %s failed", tagKey.toString().c_str());
		strcpy(target,"charset enc error");
	}
	else
	{
		// SSC_LOG("UTF-8 converted string: %s",target);
	}

	return true;
}

/// Read string array
static bool dicomtag_stringv( DcmItem *dset, char *target, const DcmTagKey &tagKey, int len, int vals )
{
	int i;
	OFString value;

	/* Get the character set encoding  from DICOM data set for later conversion*/
	char specificCharacterSet [DICOMLIB_LONG_STRING];
	if ( !dset->findAndGetOFString( DCM_SpecificCharacterSet, value ).good() )
	{
		strcpy (specificCharacterSet,"ISO_IR 192");
		//SSC_LOG( "%s: No DCM_SpecificCharacterSet tag found.", path );
	}
	else
	{
		strlcpy( specificCharacterSet, value.c_str(), DICOMLIB_LONG_STRING );
	}

	for ( i = 0; i < vals; i++ )
	{
		char *ptr = target + ( i * len );

		if ( !dset->findAndGetOFString( tagKey, value, i ).good() )
		{
			return false;
		}
		strlcpy( ptr, value.c_str(), len );
		if ( !convert_string(ptr,specificCharacterSet) )
		{
			SSC_LOG("String conversion of %s failed", tagKey.toString().c_str());
			strcpy(ptr,"charset enc error");
		}
		else
		{
			// SSC_LOG("UTF-8 converted string: %s",target);
		}
	}
	return true;
}

/// Read double array
static bool dicomtag_dv( DcmItem *dset, double *target, const DcmTagKey &tagKey, unsigned long len )
{
	unsigned long i;

	for ( i = 0; i < len; i++ )
	{
		Float64 value;

		if ( !dset->findAndGetFloat64( tagKey, value, i, OFTrue ).good() )
		{
			return false;
		}
		target[i] = value;
	}
	return true;
}

/** Fill the window array */
static bool dicomtag_window( DcmItem *dset, struct instance_t *instance )
{
	double fillCenter[DICOMLIB_PRESET_MAX];
	double fillWidth[DICOMLIB_PRESET_MAX];
	int i;

	memset( fillCenter, 0, sizeof( fillCenter ) );
	memset( fillWidth, 0, sizeof( fillWidth ) );

	dicomtag_dv( dset, fillCenter, DCM_WindowCenter, DICOMLIB_PRESET_MAX );
	dicomtag_dv( dset, fillWidth, DCM_WindowWidth, DICOMLIB_PRESET_MAX );
	memset( &instance->preset, 0, sizeof( instance->preset ) );
	instance->numPresets = 0;

	for ( i = 0; i < DICOMLIB_PRESET_MAX; i++ )	// check all window settings found...
	{
		if ( fillCenter[i] == 0.0f && fillWidth[i] == 0.0f)
		{
			break;
		}
		instance->preset[i].window.center = fillCenter[i];
		instance->preset[i].window.width = fillWidth[i];
		instance->numPresets++;
	}

	return true;
}

/// Read patient info from DICOM file
void readPatientInfo( DcmItem *dset, struct study_t *study, const char *path )
{
	/* Patient ID */
	if ( !dicomtag_string( dset, study->patientID, DCM_PatientID, DICOMLIB_LONG_STRING ) )
	{
		sstrcpy( study->patientID, "None" );
	}

	/* Patient name */
	if ( !dicomtag_string( dset, study->patientName, DCM_PatientName, DICOMLIB_LONG_STRING ) )
	{
		/* Use Patient ID instead */
		if ( !dicomtag_string( dset, study->patientName, DCM_PatientID, DICOMLIB_LONG_STRING ) )
		{
			/* For some incredibly inane reason, both patient id and patient name were optional in
			 * the DICOM standard, and so although I have never seen a set without it, we need to
			 * be able to handle it. - Per */
			sstrcpy( study->patientName, "Anonymous" );
			SSC_LOG( "%s: Search for patient name or ID failed. Using \"%s\".", path, study->patientName );
		}
	}

	/* Patient birth date */
	study->patientBirthDate[0] = '\0';
	(void) dicomtag_string( dset, study->patientBirthDate, DCM_PatientBirthDate, DICOMLIB_LONG_STRING );

	/* Patient sex */
	study->patientSex[0] = '\0';
	(void) dicomtag_string( dset, study->patientSex, DCM_PatientSex, DICOMLIB_SHORT_STRING );
}

/// Read study info from DICOM file
void readStudyInfo( DcmItem *dset, struct study_t *study, const char *path )
{
	(void)path;	// silence compiler warning
	/* Study ID */
	if ( !dicomtag_string( dset, study->studyID, DCM_StudyID, DICOMLIB_LONG_STRING ) )
	{
		/* Study UID is mandatory for DICOM sets. Make us cope anyway. */
		//SSC_LOG( "%s: Search for study ID failed.", path );
		sstrcpy( study->studyID, "None" );
	}
	if ( !dicomtag_string( dset, study->studyInstanceUID, DCM_StudyInstanceUID, DICOMLIB_LONG_STRING ) )
	{
		/* Study UID is mandatory for DICOM sets. Make us cope anyway. */
		sstrcpy( study->studyInstanceUID, "0" );
		//SSC_LOG( "%s: Search for study instance UID failed, setting it to %s.", path, study->studyInstanceUID );
	}

	/* Study time */
	if ( !dicomtag_string( dset, study->studyTime, DCM_StudyTime, DICOMLIB_SHORT_STRING ) )
	{
		//SSC_LOG( "%s: Search for study time failed. Setting to zero", path );
	}

	/* Study date */
	if ( !dicomtag_string( dset, study->studyDate, DCM_StudyDate, DICOMLIB_SHORT_STRING ) )
	{
		//SSC_LOG( "%s: Search for study date failed. Setting to zero", path );
	}

	/* Accession Number */
	(void) dicomtag_string( dset, study->accessionNumber, DCM_AccessionNumber, DICOMLIB_LONG_STRING );

	/* Study description */
	(void) dicomtag_string( dset, study->studyDescription, DCM_StudyDescription, DICOMLIB_LONG_STRING );

	/* Manufakturer */
	(void) dicomtag_string( dset, study->manufacturer, DCM_Manufacturer, DICOMLIB_LONG_STRING );
}

static QStringList var2List(QVariantList variantList)
{
	QStringList stringList;
	QListIterator<QVariant> j(variantList);
	while (j.hasNext())
	{
		QVariant next = j.next();
		if (next.canConvert(QVariant::String))
		{
			stringList << next.toString();
		}
		else if (next.type() == QVariant::List)
		{
			stringList << var2List(next.toList()).join(", ");
		}
		else
		{
			SSC_LOG("unknown type in varlist");
		}
	}
	return stringList;
}

// Parse a CSA blob
void csaparse(DcmElement *csaBlob, QVariantMap *csaMap)
{
	// Found CSA private blob, read it into a buffer
	long length = csaBlob->getLength();
	char *val = (char *)malloc(length);
	long pos = 0;
	uint32_t ntags, nitems, magic;
	char vr[5], *str = NULL;
	if (length <= 4 || !csaBlob->getPartialValue(val, 0, length).good())
	{
		SSC_LOG("Failed to read CSA buffer");
		free(val);
		return;
	}
	// Now parse it
	if (val[0] == 'S' && val[1] == 'V' && val[2] == '1' && val[3] == '0')
	{
		// Siemens CSA v2
		if (val[4] != '\4' || val[5] != '\3' || val[6] != '\2' || val[7] != '\1')
		{
			SSC_LOG("Bad CSAv2 header");
			free(val);
			return;
		}
		pos = 8;
	}
	else if (val[0] == 00 && val[1] == 0)
	{
		// not Siemens CSA v1, giving up
		free(val);
		return;
	}
	memcpy(&ntags, &val[pos], 4);
	memcpy(&magic, &val[pos + 4], 4); // magic tag
	if (magic != 77)
	{
		SSC_LOG("Bad initial magic tag in Siemens CSA");
		return;
	}
	pos += 8;
	memset(vr, 0, sizeof(vr));
	if (csaMap == NULL)
	{
		csaMap = new QVariantMap;
	}
	for (int i = 0; i < (int)ntags; i++)
	{
		str = &val[pos]; pos += 64;
		pos += 4; // ignore VM
		strncpy(vr, &val[pos], 4); pos += 4;
		pos += 4; // ignore syngodt
		memcpy(&nitems, &val[pos], 4); pos += 4;
		memcpy(&magic, &val[pos], 4); pos += 4; // magic tag
		if (magic != 77 && magic != 205)
		{
			SSC_LOG("Bad magic tag in Siemens CSA");
			return;
		}
		QVariantList var;
		for (int j = 0; j < (int)nitems; j++)
		{
			uint32_t itemsize;
			memcpy(&itemsize, &val[pos + 4], 4);
			memcpy(&magic, &val[pos + 8], 4);
			assert(magic == 77 || magic == 205);
			if (magic != 77 && magic != 205)
			{
				SSC_LOG("Bad magic item tag in Siemens CSA");
				return;
			}
			pos += 16;
			if (itemsize > 0)
			{
				QVariant tmp = QVariant::fromValue(QString(val));
				var += tmp;
			}

			pos += itemsize;

			// discard padded garbage
			pos += (4 - itemsize % 4) % 4;
		}
		if (var.size() > 1)
		{
			csaMap->insert(str, QVariant::fromValue(var));
		}
		else if (var.size() == 1)
		{
			csaMap->insert(str, QVariant::fromValue(var.value(0)));
		}
		memset(vr, 0, sizeof(vr));
	}
	free(val);
}

#define seriesError(_x, ...) \
	do { char buf[DICOMLIB_INFO_MAX]; ssprintf(buf, __VA_ARGS__); sstrcat(_x->series_info, "Error: "); sstrcat(_x->series_info, buf); sstrcat(_x->series_info, "\n"); sstrcpy(_x->error, buf); } while (0)

/// Read series and other info from DICOM file (main bulk of information lies here)
static int readSeriesInfo( DcmItem *dset, struct series_t *series, struct instance_t *instance, int frame )
{
	char tmp[DICOMLIB_LONG_STRING];
	DcmItem *sharedFunc = NULL, *perFrameFunc = NULL;
	DcmSequenceOfItems *perFrameFuncSeq = NULL;
	DcmItem *sequence = NULL;
	bool acrnema = false;	// assume not
	bool success = true;

	/* Modality - required */
	if ( !dicomtag_string( dset, series->modality, DCM_Modality, DICOMLIB_SHORT_STRING ) )
	{
		SSC_LOG( "%s: Search for required modality tag failed.", instance->path );
		sstrcpy( series->modality, "NOT FOUND" );
	}
	if ( sstrcmp( series->modality, "US" ) == 0 )
	{
		// Horrible hack due to the ways "US" is hard-coded for intraoperative data in the system.
		sstrcpy( series->modality, "SC" );
	}

	/* Marketing requested that this silly info be added to the series_info field. So hack it in here. */
	if ( series->parent_study->patientName[0] != '\0' )
	{
		sstrcat( series->series_info, "Name: " );
		sstrcat( series->series_info, series->parent_study->patientName );
		sstrcat( series->series_info, "\n" );
	}
	if ( series->parent_study->patientSex[0] != '\0' )
	{
		sstrcat( series->series_info, "Sex: " );
		sstrcat( series->series_info, series->parent_study->patientSex );
		sstrcat( series->series_info, "\n" );
	}
	if ( series->parent_study->patientBirthDate[0] != '\0' )
	{
		sstrcat( series->series_info, "Birthdate: " );
		sstrcat( series->series_info, series->parent_study->patientBirthDate );
		sstrcat( series->series_info, "\n" );
	}

	/* Grab functional group sequences */
	dset->findAndGetSequenceItem( DCM_SharedFunctionalGroupsSequence, sharedFunc );
	dset->findAndGetSequence( DCM_PerFrameFunctionalGroupsSequence, perFrameFuncSeq );
	if ( perFrameFuncSeq )
	{
		perFrameFunc = perFrameFuncSeq->getItem( frame );
	}

	// Fugly macro ahead. First look in first per frame sequence, then in shared sequence.
#define LOOKUPSEQ(_q) 												\
	if ( !perFrameFunc || !perFrameFunc->findAndGetSequenceItem( _q, sequence ).good() )			\
	{													\
		if ( !sharedFunc || !sharedFunc->findAndGetSequenceItem( _q, sequence ).good() )		\
		{												\
			sequence = dset;									\
		}												\
	}

	/* Series ID */
	if ( !dicomtag_string( dset, series->seriesID, DCM_SeriesNumber, DICOMLIB_LONG_STRING ) )
	{
		if ( !dicomtag_string( dset, series->seriesID, DCM_SeriesDescription, DICOMLIB_LONG_STRING ) )
		{
			if ( !dicomtag_string( dset, series->seriesID, DCM_SeriesInstanceUID, DICOMLIB_LONG_STRING ) )
			{
				/* Series UID is mandatory for DICOM sets. This must be an ACR-NEMA set. */
				sstrcpy( series->seriesID, "0" );
			}
		}
	}
	/* Series description */
	series->seriesDescription[0] = '\0';
	if ( !dicomtag_string( dset, series->seriesDescription, DCM_SeriesDescription, DICOMLIB_LONG_STRING )
	     || series->seriesDescription[0] == '\0' )
	{
		sstrcpy( series->seriesDescription, series->seriesID );
	}

	if ( !dicomtag_string( dset, series->SOPClassUID, DCM_SOPClassUID, DICOMLIB_LONG_STRING ) )
	{
		//SSC_LOG( "%s: Search for SOP class UID failed.", instance->path );
		sstrcpy( series->SOPClassUID, "0" );
	}
	if ( !dicomtag_string( dset, series->seriesInstanceUID, DCM_SeriesInstanceUID, DICOMLIB_LONG_STRING ) )
	{
		//SSC_LOG( "%s: Search for series instance UID failed.", instance->path );
		sstrcpy( series->seriesInstanceUID, "0" );
	}

	/* Acquisition ID (optional; not really used in DICOM anymore, so what good is it?)
	A: f.ex. Siemens use this in multi volume scans (fMRI,dwMRI-DTI) to separate volumes/ (Stefano) */
	series->acquisition[0] = '\0'; /* no acquisitions, which is good */
	if ( !dicomtag_string( dset, series->acquisition, DCM_AcquisitionNumber, DICOMLIB_LONG_STRING ) )
	{
		( void ) dicomtag_string( dset, series->acquisition, DCM_AcquisitionDate, DICOMLIB_LONG_STRING );
	}

	/* Echo number (optional, splits series) */
	/* Echo number can have multiple values, but I have no idea what that means. Neither, it seems, do anyone else. - Per */
	series->echo_number[0] = '\0';
	( void ) dicomtag_string( dset, series->echo_number, DCM_EchoNumbers, DICOMLIB_SHORT_STRING );

	/* Sequence name (optional, splits series) */
	series->sequenceName[0] = '\0';
	(void) dicomtag_string( dset, series->sequenceName, DCM_SequenceName, DICOMLIB_SHORT_STRING );

	/* SOP Instance UID (optional, used to weed out duplicates) */
	(void) dicomtag_string( dset, instance->SOPInstanceUID, DCM_SOPInstanceUID, DICOMLIB_LONG_STRING );

	/* FIXME: We should probably also read in 0x18,0x120 Convolution Kernel for CT images, and
	 * sort between series based on this. It is type 3, VR=SH (short string), VM=1+. Never seen this, though. - Per */

	/* Now we have what we need to sort the image into the correct series, but we also need a way
	 * to sort the data internally to a series. Also, all info read below here should give a visible
	 * series with the invalid tag on error, indicated by EPROTO where seriesinfo is set with
	 * error message. */

	/* Series time */
	if ( !dicomtag_string( dset, series->seriesTime, DCM_SeriesTime, DICOMLIB_SHORT_STRING ) )
	{
		if ( !dicomtag_string( dset, series->seriesTime, DCM_AcquisitionTime, DICOMLIB_SHORT_STRING ) )
		{
			if ( !dicomtag_string( dset, series->seriesTime, DCM_StudyTime, DICOMLIB_SHORT_STRING ) )
			{
				series->seriesTime[0] = '\0';		// not required, leaving it empty
				//SSC_LOG( "%s: Search for series time failed. Setting to zero", instance->path );
			}
		}
	}

	/* Series date */
	if ( !dicomtag_string( dset, series->seriesDate, DCM_SeriesDate, DICOMLIB_SHORT_STRING ) )
	{
		if ( !dicomtag_string( dset, series->seriesDate, DCM_AcquisitionDate, DICOMLIB_SHORT_STRING ) )
		{
			if ( !dicomtag_string( dset, series->seriesDate, DCM_StudyDate, DICOMLIB_SHORT_STRING ) )
			{
				series->seriesDate[0] = '\0';		// not required, leaving it empty
				//SSC_LOG( "%s: Search for series date failed. Setting to zero", instance->path );
			}
		}
	}

	if ( series->seriesDate[0] != '\0' )	// see comment in beginning of function about this
	{
		sstrcat( series->series_info, "Series Date: " );
		sstrcat( series->series_info, series->seriesDate );
		sstrcat( series->series_info, "\n" );
	}

	(void) dicomtag_string( dset, series->frameOfReferenceUID, DCM_FrameOfReferenceUID, DICOMLIB_LONG_STRING );
	(void) dicomtag_string( dset, series->repetitionTime, DCM_RepetitionTime, DICOMLIB_LONG_STRING );

	/* Check if we have a concatenation - not supported yet. */
	if ( dicomtag_string( dset, tmp, DCM_ConcatenationUID, DICOMLIB_LONG_STRING ) )
	{
		seriesError(series, "Concatenations not supported.");
		success = false;
	}

	/* Get rows and columns - required */
	if ( !dset->findAndGetUint16( DCM_Rows, series->rows ).good() )
	{
		seriesError(series, "No valid Rows tag.");
		success = false;
	}
	if ( !dset->findAndGetUint16( DCM_Columns, series->columns ).good() )
	{
		seriesError(series, "No valid Columns tag.");
		success = false;
	}
	series->multiframe = true;
	if ( !dset->findAndGetSint32( DCM_NumberOfFrames, series->frames ).good() || series->frames == 1 )
	{
		series->frames = 0; // multi-frame disabled
		series->multiframe = false;
	}

	/* Image Position Patient - required */
	LOOKUPSEQ( DCM_PlanePositionSequence );
	if ( !dicomtag_dv( sequence, instance->image_position, DCM_ImagePositionPatient, 3 ) )
	{
//		if ( !dicomtag_dv( dset, instance->image_position, DCM_ACR_NEMA_ImagePosition, 3 ) )
//		{
//			seriesError(series, "No valid Image Position Patient tag.");
			success = false;
//		}
//		else
//		{
//			acrnema = true;
//		}
	}

	/* Image Orientation Patient - required */
	LOOKUPSEQ( DCM_PlaneOrientationSequence );
	if ( !dicomtag_dv( sequence, series->image_orientation, DCM_ImageOrientationPatient, 6 ) )
	{
//		if ( !dicomtag_dv( dset, series->image_orientation, DCM_ACR_NEMA_ImageOrientation, 6 ) )
//		{
//			seriesError(series, "No valid Image Orientation Patient tag.");
			success = false;
//		}
//		else
//		{
//			acrnema = true;
//		}
	}

	/* Pixel Spacing - required */
	LOOKUPSEQ( DCM_PixelMeasuresSequence );
	if ( !dicomtag_dv( sequence, series->pixel_spacing, DCM_PixelSpacing, 2 ) )
	{
		seriesError(series, "No valid Pixel Spacing tag.");
		success = false;
	}
	else if ( series->pixel_spacing[0] <= 0.0 || series->pixel_spacing[1] <= 0.0 )
	{
		seriesError(series, "Invalid Pixel Spacing value.");
		success = false;
	}

	series->patient_orientation[0][0] = '\0';
	series->patient_orientation[1][0] = '\0';
	if ( !dicomtag_stringv( dset, ( char * )series->patient_orientation, DCM_PatientOrientation, 4, 2 ) )
	{
		/* Since ACR-NEMA 1 and 2 define a coordinate system relative to the gantry of the scanner, we need
		 * to know the orientation of the patient in the system to avoid upside down or mirrored images. */
		if ( acrnema )
		{
			sstrcat( series->series_info, "Warning: ACR-NEMA coordinate system without patient orientation defined.\n" );
		}
	}
	else
	{
		int i, j;

		/* Safety measure */
		series->patient_orientation[0][3] = '\0';
		series->patient_orientation[1][3] = '\0';

		/* Check validity of the patient orientation */
		if ( series->patient_orientation[0][0] == '\0' || series->patient_orientation[1][0] )
		{
			/* Not legal according to standard */
			sstrcat( series->series_info, "Warning: Patient orientation is defined empty.\n" );
		}
		for ( i = 0; i < 2; i++ )
		{
			for ( j = 0; j < 3; j++ )
			{
				if ( series->patient_orientation[i][j] == '\0' )
				{
					break;
				}
				if ( strchr( "APRLHF", series->patient_orientation[i][j] ) == NULL )
				{
					/* Not legal according to standard */
					sstrcat( series->series_info, "Warning: Invalid patient orientation defined.\n" );
				}
				if ( ( series->patient_orientation[i][j] == 'A' && strchr( series->patient_orientation[i], 'P' ) )
				        || ( series->patient_orientation[i][j] == 'R' && strchr( series->patient_orientation[i], 'L' ) )
				        || ( series->patient_orientation[i][j] == 'F' && strchr( series->patient_orientation[i], 'H' ) ) )
				{
					/* Be aware: For non-brain acquisitions, this may actually be the valid. Eg axial dental images may
					 *  have left and right in the same direction. */
					sstrcat( series->series_info, "Warning: Nonsensical patient orientation defined.\n" );
				}
			}
		}
		/* TODO: For ACR-NEMA, wrap coordinate system to patient orientation */
		/* TODO: For now, just check that patient orientation is in DICOM patient orientation. */

		// We can also test patient position tag (0x0018, 0x5100) as a fallback. If value is "HFS" then
		// this means Head First-Supine, or head first into gantry, with face up.

		// I wonder if we will ever make this a priority. ACR-NEMA is getting more obsolete every year that goes by. - Per
	}

	/* Get image metadata. We do this here so that we can give useful error messages and invalid notification
	 * before we try to import instead of after. */

	/* Test modality and SOP Class UID combination. */
	if ( sstrcmp( series->SOPClassUID, UID_SecondaryCaptureImageStorage ) != 0
	     && sstrcmp( series->SOPClassUID, UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage ) != 0
	     && sstrcmp( series->SOPClassUID, UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage ) != 0
	     && sstrcmp( series->SOPClassUID, UID_MultiframeTrueColorSecondaryCaptureImageStorage ) != 0
	     && sstrcmp( series->SOPClassUID, UID_CTImageStorage ) != 0
	     && sstrcmp( series->SOPClassUID, UID_MRImageStorage ) != 0
	     && sstrcmp( series->SOPClassUID, UID_EnhancedCTImageStorage ) != 0
	     && sstrcmp( series->SOPClassUID, UID_EnhancedMRImageStorage ) != 0 )
	{
		if (strncmp( series->modality, "MR", 2 ) == 0 || strncmp( series->modality, "CT", 2 ) == 0 || strncmp( series->modality, "PT", 2 ) )
		{
			// Unsupported SOP Class, but support anyway. Might be ACR-NEMA. If valid ACR-NEMA,
			// warning overwritten below with a general ACR-NEMA warning.
			series->warning = DICOMLIB_WARNING_UNKNOWN_SOP_CLASS;
		}
		else
		{
			seriesError(series, "Modality \"%s\" and SOP class \"%s\" is not supported.", series->modality, series->SOPClassUID );
			success = false;
		}
	}

	/* Get samples per pixel - required */
	if ( !dset->findAndGetUint16( DCM_SamplesPerPixel, series->samples_per_pixel ).good() )
	{
		if ( !acrnema )
		{
			seriesError(series, "No valid Samples per Pixel tag.");
			success = false;
		}
		else
		{
			series->samples_per_pixel = 1;
		}
	}

	/* Get bits per sample - required */
	if ( !dset->findAndGetUint16( DCM_BitsAllocated, series->bits_per_sample ).good() )
	{
		if ( !acrnema )
		{
			seriesError(series, "No valid bits allocated tag.");
			success = false;
		}
		else
		{
			series->bits_per_sample = 16;
		}
	}

	//* test for diffusion multi directional data (DTI) */
	series->DTI.isDTI=false;
	series->DTI.bval[0] = '\0';
	series->DTI.bvec[0]=0;series->DTI.bvec[1]=0;series->DTI.bvec[2]=0;
	const Float64* bvec=NULL;
	OFString OFSread;

	// testing for standard DICOM DTI tags (only seen in philips data-sets up to now)
	// works for Philips Achieva 3.2.1/3.2.1.1
	if ( dset->findAndGetFloat64Array(DCM_DiffusionGradientOrientation, bvec ).good() )
	{
		// Gradient orientation
		series->DTI.bvec[0]=bvec[0];
		series->DTI.bvec[1]=bvec[1];
		series->DTI.bvec[2]=bvec[2];
		// SSC_LOG( "Tag DCM_DiffusionGradientOrientation found in file %s %f,%f,%f", instance->path ,series->DTI.bvec[0], series->DTI.bvec[1], series->DTI.bvec[2] );
		// Directionality: we don't really need this but who knows...
		if ( dset->findAndGetOFString(DCM_DiffusionDirectionality, OFSread ).good() )
		{
			// SSC_LOG( "Tag DCM_DiffusionDirectionality found in file %s : %s",instance->path , OFSread.c_str() );
			sstrcat(series->DTI.directionality, OFSread.c_str());
		}
		// B-value
		if (dset->findAndGetOFString( DCM_DiffusionBValue,OFSread ).good() )
		{
			// SSC_LOG( "Tag DCM_DiffusionBValue found %s", OFSread.c_str() );
			sstrcat(series->DTI.bval, OFSread.c_str());
		}
		series->DTI.isDTI=true;
	}
	// testing for private Siemens DTI tags
	// works for Syngo MR B15; MR B17; MR D11
	/* TODO: get B-matrix from CSA-tag and estimate B-values and B-vectors from this by default and use DICOM-tags as fallback
		 see: https://github.com/matthew-brett/nibabel/blob/master/nibabel/nicom/dwiparams.py
		 (SIEMENS DICOM tags for bvec and bvals could be inaccurate for older datasets) */
	else if ( dset->tagExists(SiemensDCM_DiffusionBValue) )
	{
		// if there is a B-value we have DTI data
		series->DTI.isDTI=true;

		// Gradient orientation
		if ( dset->findAndGetFloat64Array(SiemensDCM_DiffusionGradientOrientation, bvec ).good() )
		{
			// Siemens Diff.gradients are in patient space coordinates so we convert to image space coordinates:
			double cross[3];
			series->DTI.bvec[0]=vector3d_inner_product(&series->image_orientation[0], bvec);
			series->DTI.bvec[1]=vector3d_inner_product(&series->image_orientation[3], bvec);
			vector3d_cross_product(cross, &series->image_orientation[0], &series->image_orientation[3] );
			series->DTI.bvec[2]=vector3d_inner_product(cross, bvec);
			// SSC_LOG( "Tag SiemensDCM_DiffusionGradientOrientation found in file %s %f,%f,%f", instance->path ,series->DTI.bvec[0], series->DTI.bvec[1], series->DTI.bvec[2] );
		}

		// Directionality:string with "DIRECTIONAL" or "NONE" - we don't really need this but who knows...
		if ( dset->findAndGetOFString(SiemensDCM_DiffusionDirectionality, OFSread ).good() )
		{
			// SSC_LOG( "Tag SiemensDCM_DiffusionDirectionality found in file %s :\"%s\"",instance->path , OFSread.c_str() );
			sstrcat(series->DTI.directionality, OFSread.c_str());
		}

		// B-value
		if (dset->findAndGetOFString( SiemensDCM_DiffusionBValue,OFSread ).good() )
		{
			// SSC_LOG( "Tag DCM_DiffusionBValue found %s", OFSread.c_str() );
			sstrcat(series->DTI.bval, OFSread.c_str());
		}
	}

#ifdef DTI
	// Siemens CSA data?
	// first: CSA IMAGE Header
	if (dset->tagExistsWithValue(DcmTagKey(0x0029, 0x1010)))
	{
		// This is a CSA, probably. Make sure.
		char val[200];
		if (dicomtag_string(dset, val, DcmTagKey(0x0029, 0x0010), 200))
		{
			if (strcmp(val, "SIEMENS CSA HEADER") == 0)
			{
				DcmElement *csaBlob;
				dset->findAndGetElement(DcmTagKey(0x0029, 0x1010), csaBlob, false, false);
				csaparse(csaBlob, series->DTI.csaImageMap);
			}
		}
	}
	/* CSA SERIES Header not needed so far ...
	if (dset->tagExistsWithValue(DcmTagKey(0x0029, 0x1020)))
	{
		// This is a CSA, probably. Make sure.
		char val[200];
		if (dicomtag_string(dset, val, DcmTagKey(0x0029, 0x0010), 200))
		{
			if (strcmp(val, "SIEMENS CSA HEADER") == 0)
			{
				DcmElement *csaBlob;
				dset->findAndGetElement(DcmTagKey(0x0029, 0x1020), csaBlob, false, false);
				csaparse(csaBlob, series->DTI.csaSeriesMap);
			}
		}
	}*/
#endif

	// test Mosaic and if yes set relevant information for selected frame
	series->mosaic = false;
	if (dset->tagExistsWithValue(DcmTagKey(0x0019,0x100a))) // NumberOfImagesInMosaic
	{
		// SSC_LOG( "Tag 0x0019,0x100a found -> mosaic: %s", instance->path );

		// NumberOfImagesInMosaic -> frames
		uint16_t help;
		if ( !dset->findAndGetUint16( DcmTagKey(0x0019,0x100a), help ).good() )
		{
			seriesError(series, "Reading of NumberOfImagesInMosaic failed.");
		}
		else
		{
			series->frames = help;
			instance->frame = frame;
		}

		// SSC_LOG( "--> found %i frames in mosaic (processing frame %i now)",series->frames, instance->frame);
		if ( series->frames > 1 ) // sanity check
		{
			Float64 sliceDistValue = 2;
			// SSC_LOG( "--> old rows/cols: %i/%i", series->rows, series->columns);

			// calc rows and colums of frames in mosaic
			int mosaicSize = ceil( sqrt( series->frames ));
			series->rows = series->rows / mosaicSize;
			series->columns = series->columns / mosaicSize;

			// SSC_LOG( "--> calculated rows/cols: %i/%i", series->rows, series->columns);

			/*
			// correct ImagePatientPosition because IPP is given with mosaic's full size
			*/

			double sliceDistance[3], imageRowVector[3], imageColumnVector[3];
			//  calc vector from opper left voxel of mosaic to opper left voxel of desired slice:
			vector3d_copy(imageRowVector,&series->image_orientation[0] );
			vector3d_copy(imageColumnVector,&series->image_orientation[3] );
			vector3d_scalar_multiply(imageRowVector, (double)(series->rows * (mosaicSize - 1)) * 0.5 *  series->pixel_spacing[0]);
			vector3d_scalar_multiply(imageColumnVector, (double)(series->columns * (mosaicSize - 1)) * 0.5 * series->pixel_spacing[1]);

			// calc Image Orientation normalvector:
			vector3d_cross_product( sliceDistance, &series->image_orientation[0],
						&series->image_orientation[3] );

			// and now calc the slice position (distance from first slice):
			if ( !dset->findAndGetFloat64( DcmTagKey(0x0018,0x0088) , sliceDistValue ).good() || sliceDistValue < 0.001 ) // DCM_SliceSpacing
			{
				if ( !dset->findAndGetFloat64( DCM_SliceThickness , sliceDistValue ).good() ) // DCM_SliceSpacing
					SSC_LOG( "No SliceSpacing or SliceDistance Tag found. Default slice distance is %f",sliceDistValue);
			}
			vector3d_scalar_multiply( sliceDistance, instance->frame * sliceDistValue);


			// and add all vectors to get real image position of this slice
			vector3d_add( instance->image_position, instance->image_position, imageRowVector);
			vector3d_add( instance->image_position, instance->image_position, imageColumnVector);
			vector3d_add( instance->image_position, instance->image_position, sliceDistance);
			// SSC_LOG("Image position: %f %f %f",instance->image_position[0],instance->image_position[1],instance->image_position[2] );
			// and tag as mosaic for raw data extraction
			series->mosaic = true;
		}
	}
	//SSC_LOG( "--> IPP %f / %f / %f", instance->image_position[0], instance->image_position[1], instance->image_position[2] );

	// Look for RGB colour LUT
	if (dset->tagExistsWithValue(DCM_RedPaletteColorLookupTableDescriptor))
	{
		const int lutLength = 256;
		const int lutBits = 16;
		unsigned long dcount = 3, lcount = lutLength;
		const Uint16 *dRed, *dGreen, *dBlue;
		const Uint16 *lRed, *lGreen, *lBlue;

		if (!dset->findAndGetUint16Array(DCM_RedPaletteColorLookupTableDescriptor, dRed, &dcount).good()
		    || !dset->findAndGetUint16Array(DCM_GreenPaletteColorLookupTableDescriptor, dGreen, &dcount).good()
		    || !dset->findAndGetUint16Array(DCM_BluePaletteColorLookupTableDescriptor, dBlue, &dcount).good()
		    || dRed[1] != 0 || dRed[2] != lutBits		// LUTs must be 16bit and start from zero
		    || dGreen[1] != 0 || dGreen[2] != lutBits
		    || dBlue[1] != 0 || dBlue[2] != lutBits
		    || dRed[0] != dGreen[0] || dRed[0] != dBlue[0])	// LUTs must be same size
		{
			sstrcpy( series->series_info, "Warning: Failed to read image color LUT descriptors." );
			SSC_LOG("Failed to read image color LUT descriptors from %s", instance->path);
		}
		else if (!dset->findAndGetUint16Array(DCM_RedPaletteColorLookupTableData, lRed, &lcount).good()
			 || !dset->findAndGetUint16Array(DCM_GreenPaletteColorLookupTableData, lGreen, &lcount).good()
			 || !dset->findAndGetUint16Array(DCM_BluePaletteColorLookupTableData, lBlue, &lcount).good())
		{
			// Descriptors were fine but reading the LUTs failed
			sstrcpy( series->series_info, "Warning: Failed to read image color LUT." );
			SSC_LOG("Failed to read image color LUT from %s", instance->path);
		}
		else if (series->VOI.lut.table == NULL)	// ignoring if already loaded, assuming same for all slices
		{
			// Both descriptors and LUT were fine. Add it to first preset.
			int i;

			series->VOI.lut.table = malloc(dRed[0] * 4);
			series->VOI.lut.bits = 8;	// reduced
			series->VOI.lut.samples = 4;
			series->VOI.lut.start = dRed[1];
			series->VOI.lut.length = dRed[0];
			for (i = 0; i < dRed[0]; i++)
			{
				uint32_t *dst = (uint32_t *)series->VOI.lut.table;
				uint32_t store = (lRed[i] | (lGreen[i] << 8) | (lBlue[i] << 16) | (255 << 24));
				dst[i] = store;
			}
		}
	}

	/* Get window settings - as many unique values as possible. Since some radiologists set useful values by
	 * looking at a series that are saved only in a single slice, we look through all slices for useful values
	 * and store them. */
	LOOKUPSEQ( DCM_VOILUTSequence );
	( void ) dicomtag_window( sequence, instance );

//	if ( acrnema )
//	{
//		char tmp[DICOMLIB_LONG_STRING];
//
//		if (!dicomtag_string( dset, tmp, DCM_ACR_NEMA_RecognitionCode, DICOMLIB_LONG_STRING)
//		    || strncmp(tmp, "ACR-NEMA", 8) != 0 || (tmp[9] != '1' && tmp[9] != '2'))
//		{
//			seriesError(series, "Identified as ACR-NEMA but no recognition code found.");
//			return EPROTO;
//		}
//		if (tmp[9] == '1')
//		{
//			series->warning = DICOMLIB_WARNING_ACR_NEMA_1;
//		}
//		else
//		{
//			series->warning = DICOMLIB_WARNING_ACR_NEMA_2;
//		}
//	}
	return success ? 0 : EPROTO;
}

struct filenode *DICOM_Dir_Nodes( struct study_t *study )
{
	struct filenode *node = NULL;

	if ( !study )
	{
		SSC_LOG( "Invalid parameter" );
		return NULL;
	}
	DcmDicomDir dicomdir( study->resource );
	DcmDirectoryRecord *root = &(dicomdir.getRootRecord());
	DcmDirectoryRecord *patientRecord = NULL;
	DcmDirectoryRecord *studyRecord = NULL;
	DcmDirectoryRecord *seriesRecord = NULL;
	DcmDirectoryRecord *fileRecord = NULL;

	if (root == NULL)
	{
		SSC_LOG( "Not legal DICOMDIR" );
		return NULL;
	}
	while (((patientRecord = root->nextSub(patientRecord)) != NULL))
	{
		while (((studyRecord = patientRecord->nextSub(studyRecord)) != NULL))
		{
			OFString studyUID;

			if ( !studyRecord->findAndGetOFString( DCM_StudyInstanceUID, studyUID ).good()
			     || strcmp( studyUID.c_str(), study->studyInstanceUID ) != 0 )
			{
				continue;
			}
			while (((seriesRecord = studyRecord->nextSub(seriesRecord)) != NULL))
			{
				while (((fileRecord = seriesRecord->nextSub(fileRecord)) != NULL))
				{
					struct filenode *next = (struct filenode *)malloc( sizeof( *next ) );
					char *sub;
					OFString value;
					int i = 0;

					next->next = node;
					node = next;
					
					sstrcpy( next->path, study->resource );
					sub = strstr( next->path, "DICOMDIR" );
					if ( sub )
					{
						*sub = '\0'; // remove trailing DICOMDIR
					}
					if ( next->path[strlen(next->path) - 1] == '/' )
					{
						next->path[strlen(next->path) - 1] = '\0'; // remove trailing slash
					}
					while ( fileRecord->findAndGetOFString( DCM_ReferencedFileID, value, i ).good() )
					{
						sstrcat( next->path, "/" );
						sstrcat( next->path, value.c_str() );
						i++;
					}
				}
			}
		}
	}
	return node;
}

int DICOM_Dir( const char *path, struct study_t **studyPtr )
{
	char dirfile[PATH_MAX];

	if ( !path || !studyPtr )
	{
		SSC_LOG( "Invalid parameter" );
		return EINVAL;
	}
	sstrcpy( dirfile, path );
	if ( !strstr( path, "DICOMDIR" ) )
	{
		if ( dirfile[strlen( dirfile ) - 1] != '/' )
		{
			sstrcat( dirfile, "/" );
		}
		sstrcat( dirfile, "DICOMDIR" );
	}

	DcmDicomDir dicomdir( dirfile );
	if ( dicomdir.error().bad() )
	{
		SSC_LOG( "Failed to load DICOMDIR: %s", dicomdir.error().text() );
		return -1;
	}
	DcmDirectoryRecord *root = &(dicomdir.getRootRecord());
	DcmDirectoryRecord *patientRecord = NULL;
	DcmDirectoryRecord *studyRecord = NULL;
	DcmDirectoryRecord *seriesRecord = NULL;
	int count = 0;	// unique study ID within list

	*studyPtr = NULL;
	if (root == NULL)
	{
		SSC_LOG( "Not legal DICOMDIR" );
		return -1;
	}
	SSC_LOG( "Opened %s", dirfile );
	while (((patientRecord = root->nextSub(patientRecord)) != NULL))
	{
		while (((studyRecord = patientRecord->nextSub(studyRecord)) != NULL))
		{
			struct study_t *study = (struct study_t *)calloc(1, sizeof( *study ));

			study->next_study = *studyPtr;
			*studyPtr = study;
			study->series_count = 0;
			study->valid = true;

			// mark as having come from DICOMDIR
			study->port = -1;

			sstrcpy( study->resource, dirfile );
			study->first_series = NULL;
			study->study_id = count++;
			readPatientInfo( patientRecord, study, dirfile );
			readStudyInfo( studyRecord, study, dirfile );
			while (((seriesRecord = studyRecord->nextSub(seriesRecord)) != NULL))
			{
				study->series_count++;
			}
		}
	}
	SSC_LOG( "Found %d studies", count );
	return 0;
}

// Returns 0 on success, EPROTO if invalid series, EINVAL if internal error
int parse_DICOM( DcmItem *dset, struct study_t *study, struct series_t *series, struct instance_t *instance, int curFrame )
{
	series->series_info[0] = '\0';

	if ( instance->path == NULL || instance == NULL || series == NULL || dset == NULL )
	{
		SSC_LOG( "Parameter error" );
		return EINVAL;
	}

	readPatientInfo( dset, study, instance->path );
	readStudyInfo( dset, study, instance->path );
	return readSeriesInfo( dset, series, instance, curFrame );
}

// Raw image function - no VOI, using interdata
const void *DICOM_raw_image(const struct series_t *series, struct instance_t *instance, int frame)
{
	static DicomImage *dicomimage = NULL;	// to keep resulting buffer in memory, and cache result
	static char *savedPath = NULL;
	static void *buffer = NULL;	// to keep RGB buffer in memory
	int maxFrames;

	if (buffer)
	{
		free(buffer);
		buffer = NULL;
	}
	if ( !dicomimage || !savedPath || strcmp( instance->path, savedPath ) != 0 || series->mosaic )
	{
		DicomImage * scaledDicomImage;
		delete dicomimage;
		dicomimage = new DicomImage( instance->path, CIF_AcrNemaCompatibility | CIF_MayDetachPixelData );

		// Siemens mosaic stuff: Clipping the desired image from the mosaic ...
		if ( series->mosaic )
		{
			signed long clipX = 0; // Top left x coordinate for mosaic clipping
			signed long clipY = 0; // Top left y coordinate for mosaic clipping
			unsigned long clipWidth = series->rows; // Clipping width for mosaics clipping
			unsigned long clipHeight = series->columns; // Clipping height for mosaics clipping
			// Size of the quadratic mosaic f.ex. 60 frames -> 8x8 Matrix -> matrixSize=8 :
			unsigned int mosaicSize = ceil(sqrt(series->frames));
			clipX = ( frame % mosaicSize ) * clipWidth;
			clipY = ( frame / mosaicSize ) * clipHeight;

			scaledDicomImage = dicomimage->createScaledImage( clipX, clipY, clipWidth, clipHeight, clipWidth, clipHeight, 0, 0 );
			delete dicomimage;
			dicomimage = scaledDicomImage;

			// mosaic has really only one frame:
			frame = 0;
		}
		free( savedPath );
		savedPath = strdup( instance->path );
	}
	EI_Status status = dicomimage->getStatus();
	if ( status != EIS_Normal )
	{
		SSC_LOG( "Error from DCMTK reading %s: %s", instance->path, dicomimage->getString( status ) );
		delete dicomimage;
		dicomimage = NULL;
		errno = EIO;
		return NULL;
	}
	maxFrames = dicomimage->getFrameCount();
	if (series->multiframe && maxFrames <= frame)
	{
		SSC_ERROR("Not enough frames!");
		return NULL;
	}
	else if (!series->multiframe)
	{
		frame = 0;	// just grab the first (and only) pixel data
	}
	const DiPixel *pixels = dicomimage->getInterData();

	// Figure out what we are dealing with here
	switch (pixels->getRepresentation())
	{
	case EPR_Uint8:
		instance->internal_bits_per_sample = 8;
		instance->isSigned = false;
		break;
	case EPR_Uint16:
		instance->isSigned = false;
		instance->internal_bits_per_sample = 16;
		break;
	case EPR_Uint32:
		instance->isSigned = false;
		instance->internal_bits_per_sample = 32;
		break;
	case EPR_Sint8:
		instance->isSigned = true;
		instance->internal_bits_per_sample = 8;
		break;
	case EPR_Sint16:
		instance->isSigned = true;
		instance->internal_bits_per_sample = 16;
		break;
	case EPR_Sint32:
		instance->isSigned = true;
		instance->internal_bits_per_sample = 32;
		break;
	}

	// Adjust pointer for selected frame in a multi-frame series
	if (pixels->getPlanes() == 1)
	{
		return ((char *)pixels->getData()) + frame * series->rows * series->columns * (instance->internal_bits_per_sample / 8) * series->samples_per_pixel;
	}
	else
	{
		// Use ordinary image function here instead of interdata, since we cannot alter window levels for RGB anyway
		const int planar = 1;
		const int bits = 8;
		const int count = series->rows * series->columns * 3;
		assert(series->rows == dicomimage->getHeight() && series->columns == dicomimage->getWidth());
		assert((int)dicomimage->getOutputDataSize( 8 ) == count);
		buffer = malloc(count);
		dicomimage->setNoVoiTransformation();
		dicomimage->setPresentationLutShape( ESP_Default );
		dicomimage->hideAllOverlays();
		if (dicomimage->getOutputData(buffer, count, bits, frame, planar) == 0)
		{
			SSC_ERROR("Failed to get RGB volume");
			free(buffer);
			buffer = NULL;
		}
		return buffer;
	}
}


void *DICOM_image_scaled( const struct instance_t *instance, int *x, int *y, int bits, int frame )
{
	const int planar = 0; /* interleave colour data, not separate; not that we use this for our monochrome data */
	const int interpolate = 4; /* bicubic algorithm */
	const int aspect = (*x == 0 || *y == 0); /* no aspect ratio lock unless either x or y is zero */
	struct series_t *series = instance->parent_series;
	EI_Status status;
	void *buffer;
	DicomImage *scaled;
	DicomImage dicomimage(instance->path, CIF_AcrNemaCompatibility | CIF_MayDetachPixelData | CIF_UsePartialAccessToPixelData, frame, 1);
	status = dicomimage.getStatus();
	if ( status != EIS_Normal )
	{
		SSC_LOG( "Error from DCMTK reading %s: %s", instance->path, dicomimage.getString( status ) );
		errno = EIO;
		return NULL;
	}
	// Siemens mosaic stuff: Clipping the desired image from the mosaic ...
	signed long clipX = 0; // Top left x coordinate for mosaic clipping
	signed long clipY = 0; // Top left y coordinate for mosaic clipping
	unsigned long clipWidth = series->rows; // Clipping width for mosaics clipping
	unsigned long clipHeight = series->columns; // Clipping height for mosaics clipping
	if (series->mosaic)
	{
		// Size of the quadratic mosaic f.ex. 60 frames -> 8x8 Matrix -> matrixSize=8:
		unsigned int mosaicSize = ceil(sqrt(series->frames));
		clipX = ( frame % mosaicSize ) * clipWidth;
		clipY = ( frame / mosaicSize ) * clipHeight;
		// mosaic has really only one frame:
		frame = 0;
	}

	if ( !series->mosaic )
	{
		scaled = dicomimage.createScaledImage( (unsigned long)*x, (unsigned long)*y, interpolate, aspect );
	}
	else
	{
		scaled = dicomimage.createScaledImage( clipX, clipY, clipWidth, clipHeight, (unsigned long)*x, (unsigned long)*y, interpolate, aspect );
	}
	if ( !scaled )
	{
		errno = -2;
		return NULL;
	}
	if ( scaled->getStatus() != EIS_Normal )
	{
		SSC_LOG( "Error from DCMTK when scaling: %s", scaled->getString( status ) );
		delete scaled;
		errno = -3;
		return NULL;
	}
	scaled->setPresentationLutShape( ESP_Default );
	scaled->hideAllOverlays();
	if ( series->VOI.current.width == 0 )
	{
		scaled->setNoVoiTransformation();
	} else {
		scaled->setWindow( series->VOI.current.center, series->VOI.current.width );
	}
	int size = scaled->getOutputDataSize(bits);
	buffer = malloc(size);
	if (!scaled->getOutputData(buffer, size, bits, 0, planar))
	{
		SSC_LOG("Failure");
	}
	*x = scaled->getWidth();
	*y = scaled->getHeight();
	delete scaled;
	return buffer;
}

int DICOM_image_window_auto( struct series_t *series, struct instance_t *instance )
{
	DicomImage dicomimage( instance->path, CIF_AcrNemaCompatibility | CIF_MayDetachPixelData );
	double firstpixel = 0.0, lastpixel = 0.0;
	EI_Status status = dicomimage.getStatus();

	if ( status != EIS_Normal )
	{
		series->valid = false;
		SSC_LOG( "Failed to read image data from %s", instance->path );
		sstrcat( series->series_info, "Failed to read image data" );
		return -1;
	}

	// get possible
	if (dicomimage.getMinMaxValues( firstpixel, lastpixel, 1 ) == 0)
	{
		// don't spam
	}
	series->VOI.range.width = lastpixel - firstpixel;
	series->VOI.range.center = lastpixel - ( series->VOI.range.width / 2 );

	// get actual
	if (dicomimage.getMinMaxValues( series->firstpixel, series->lastpixel, 0 ) == 0)
	{
		// don't spam
	}
	series->VOI.minmax.width = series->lastpixel - series->firstpixel;
	series->VOI.minmax.center = series->lastpixel - ( series->VOI.minmax.width / 2 );

	// paranoia check for lastpixel == firstpixel
	if ( series->VOI.minmax.width == 0 )
	{
		if ( instance->numPresets > 0 )
		{
			// fall back to instance presets:
			series->VOI.minmax.width = instance->preset[0].window.width;
			series->VOI.minmax.center = instance->preset[0].window.center;
		}
		else
		{
			// fall back to window width 200 -> could be a problem if there
			// are anywhere other extremely different values in the volume
			series->VOI.minmax.width = 200;
			series->VOI.minmax.center = series->VOI.range.center + 100;
		}
		series->lastpixel = series->VOI.minmax.center + (series->VOI.minmax.width/2);
		series->firstpixel = series->VOI.minmax.center - (series->VOI.minmax.width/2);
	}

	// make suggestion based on modality
	if ( series->modality[0] == 'C' && series->modality[1] == 'T' )
	{
		/* CT brain preset */
		series->VOI.suggestion.center = 20;
		series->VOI.suggestion.width = 80;
	}
	else
	{
		series->VOI.suggestion = series->VOI.minmax;
	}

	// Set series presets based on chosen instance's presets
	memcpy( &series->VOI.preset, &instance->preset, sizeof( instance->preset ) );
	series->VOI.numPresets = instance->numPresets;

	return 0;
}

int DICOMLib_Network( const char *appAETitle, const char *peerAETitle, const char *destAETitle, const char *peerIP, int peerPort )
{
	if (appAETitle) sstrcpy(conf.appAETitle, appAETitle); else conf.appAETitle[0] = '\0';
	if (peerAETitle) sstrcpy(conf.peerAETitle, peerAETitle); else conf.peerAETitle[0] = '\0';
	if (destAETitle) sstrcpy(conf.destAETitle, destAETitle); else conf.destAETitle[0] = '\0';
	if (peerIP) sstrcpy(conf.peerIP, peerIP); else conf.peerIP[0] = '\0';
	conf.peerPort = peerPort;
	return 0;
}

static int study_id_counter = 0;

/**
 * This function.is used to indicate progress when findscu receives search results over the
 * network. This function will simply cause some information to be dumped to stdout.
 *
 * Contains commented out code that may be used for debugging, as this code is not yet fully
 * operational.
 *
 * Parameters:
 *   callbackData        - [in] data for this callback function
 *   request             - [in] The original find request message.
 *   responseCount       - [in] Specifies how many C-FIND-RSP were received including the current one.
 *   rsp                 - [in] the C-FIND-RSP message which was received shortly before the call to
 *                              this function.
 *   responseIdentifiers - [in] Contains the record which was received. This record matches the search
 *                              mask of the C-FIND-RQ which was sent.
 */
static void progressCallback( void *callbackData, T_DIMSE_C_FindRQ *request, int responseCount, T_DIMSE_C_FindRSP *rsp, DcmDataset *responseIdentifiers )
{
	MyCallbackInfo *myCallbackData = OFstatic_cast( MyCallbackInfo *, callbackData );
	OFString value;
	struct study_t **studyList = myCallbackData->studyList;
	struct study_t *next = *studyList;
	struct study_t *newStudy = (struct study_t *)calloc( 1, sizeof( **studyList ) );

	newStudy->valid = true;
	newStudy->first_series = NULL;
	readPatientInfo( responseIdentifiers, newStudy, "PACS" );
	readStudyInfo( responseIdentifiers, newStudy, "PACS" );
	newStudy->port = conf.peerPort;
	newStudy->study_id = study_id_counter++;

	if ( responseIdentifiers->findAndGetSint32( DCM_NumberOfStudyRelatedSeries, newStudy->series_count ).bad() )
	{
		SSC_LOG( "Could not read number of series in study" );
		newStudy->series_count = 0;	// error
	}
	SSC_LOG( "Received study %s (with %d series not received yet) from PACS", newStudy->studyID, newStudy->series_count );

	// Add to end of linked list, so that index order is preserved
	newStudy->next_study = NULL;
	if (!next)
	{
		// No current study list (first node)
		*studyList = newStudy;
	}
	else
	{
		while (next->next_study) next = next->next_study;
		next->next_study = newStudy;
	}

	if ( myCallbackData->cancel )	// FIXME: never actually set from anywhere
	{
		OFCondition cond = DIMSE_sendCancelRequest( myCallbackData->assoc, myCallbackData->presId, request->MessageID );
		if ( cond.bad() )
		{
			SSC_LOG( "Cancel RQ Failed" );
			DimseCondition::dump( cond );
		}
	}

	/* Silence compiler warnings */
	if (false)
	{
		(void) request;
		(void) responseCount;
		(void) rsp;
	}
}

// is macro to do va list properly - inline function is more work
#define returnErrorStudy(message, ...)					\
do {									\
	struct study_t *study = (study_t *)calloc(1, sizeof(*study));	\
	study->valid = true;						\
	sstrcpy(study->patientName, message);				\
	ssprintf(study->patientID, __VA_ARGS__);			\
	study->next_study = NULL;					\
	SSC_LOG("%s: %s", study->patientName, study->patientID);		\
	study->study_id = UINT32_MAX;					\
	sstrcpy(study->studyInstanceUID, "ERROR");			\
	return study;							\
} while(0)

struct study_t *DICOMLib_StudiesFromPACS( const char *searchString, enum pacs_search_param_t relatedTo, const char *timeSpan )
{
	struct study_t *studyList = NULL;
	DcmFileFormat dcmff;
	MyCallbackInfo callbackData;
	const char *opt_abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;
	const char *transferSyntaxes[] = { UID_LittleEndianExplicitTransferSyntax, UID_BigEndianExplicitTransferSyntax, UID_LittleEndianImplicitTransferSyntax };
	T_ASC_Network *net = NULL;
	T_ASC_Parameters *params = NULL;
	DIC_NODENAME localHost;
	DIC_NODENAME peerHost;
	T_ASC_Association *assoc = NULL;
	DcmDataset *statusDetail = NULL;
	DcmElement *elem = NULL;
	DcmDataset *overrideKeys = dcmff.getDataset();

	study_id_counter = 0;

	// ** Add search keys **

	elem = newDicomElement( DCM_QueryRetrieveLevel, 1 );
	elem->putString( "STUDY" );
	overrideKeys->insert( elem );

	elem = newDicomElement( DCM_StudyDate, 1 );
	if ( relatedTo == DICOMLIB_SEARCH_STUDY_DATE )
	{
		elem->putString( searchString );
	}
	else if ( timeSpan )
	{
		elem->putString( timeSpan );
	}
	overrideKeys->insert( elem );

	elem = newDicomElement( DCM_AccessionNumber, 1 );
	if ( relatedTo == DICOMLIB_SEARCH_ACCESSION_NUMBER )
	{
		elem->putString( searchString );
	}
	overrideKeys->insert( elem );

	elem = newDicomElement( DCM_PatientName, 1 );
	if ( relatedTo == DICOMLIB_SEARCH_PATIENT_NAME )
	{
		elem->putString( searchString );
	}
	overrideKeys->insert( elem );

	elem = newDicomElement( DCM_PatientID, 1 );
	if ( relatedTo == DICOMLIB_SEARCH_PATIENT_ID )
	{
		elem->putString( searchString );
	}
	overrideKeys->insert( elem );

	elem = newDicomElement( DCM_PatientBirthDate, 1 );
	if ( relatedTo == DICOMLIB_SEARCH_PATIENT_BIRTH )
	{
		elem->putString( searchString );
	}
	overrideKeys->insert( elem );

	overrideKeys->insert( newDicomElement( DCM_StudyDescription, 1 ) );
	overrideKeys->insert( newDicomElement( DCM_StudyTime, 1 ) );
	overrideKeys->insert( newDicomElement( DCM_StudyInstanceUID, 1 ) );
	overrideKeys->insert( newDicomElement( DCM_StudyID, 1 ) );
	overrideKeys->insert( newDicomElement( DCM_NumberOfStudyRelatedSeries, 1 ) );

	// ** Initialize network **

	/* initialize network, i.e. create an instance of T_ASC_Network*. */
	dcmConnectionTimeout.set( (int)DICOMLib_GetConfig(DICOMLIB_CONF_NETWORK_TIMEOUT) );
	SSC_LOG( "Initializing network..." );
	OFCondition cond = ASC_initializeNetwork( NET_REQUESTOR, 0, (int)DICOMLib_GetConfig(DICOMLIB_CONF_NETWORK_TIMEOUT), &net );
	if ( cond.bad() )
	{
		DimseCondition::dump( cond );
		returnErrorStudy( "Network initialization failed", "%s", cond.text() );
	}

	/* initialize asscociation parameters, i.e. create an instance of T_ASC_Parameters*. */
	cond = ASC_createAssociationParameters( &params, ASC_DEFAULTMAXPDU );
	if ( cond.bad() )
	{
		DimseCondition::dump( cond );
		returnErrorStudy( "Association initialization failed", "%s", cond.text() );
	}
	ASC_setAPTitles( params, conf.appAETitle, conf.peerAETitle, NULL );

	/* Figure out the presentation addresses and copy the corresponding values into the association parameters.*/
	gethostname( localHost, sizeof( localHost ) - 1 );
	ssprintf( peerHost, "%s:%d", conf.peerIP, conf.peerPort );
	ASC_setPresentationAddresses( params, localHost, peerHost );

	/* Set the presentation contexts which will be negotiated when the network connection will be established */
	/* we prefer Little Endian Explicit */
	cond = ASC_addPresentationContext( params, 1, opt_abstractSyntax, transferSyntaxes, 3 );
	if ( cond.bad() )
	{
		DimseCondition::dump( cond );
		SSC_LOG( "Failed to add presentation context: %s", cond.text() );
	}

	/* Create association, i.e. try to establish a network connection to another DICOM application. This call creates an instance of T_ASC_Association*. */
	SSC_LOG( "Create association..." );
	cond = ASC_requestAssociation( net, params, &assoc );
	if ( cond.bad() )
	{
		if (cond == DUL_ASSOCIATIONREJECTED)
		{
			T_ASC_RejectParameters rej;
			OFOStringStream dump;

			ASC_getRejectParameters( params, &rej );
			ASC_printRejectParameters( dump, &rej );
			SSC_LOG( "Reject parameters: %s", dump.str().c_str() );
			returnErrorStudy( "Association rejected", "%s", cond.text() );
		}
		else
		{
			DimseCondition::dump( cond );
			returnErrorStudy( "Association failed", "%s", cond.text() );
		}
	}

	if ( ASC_countAcceptedPresentationContexts( params ) == 0 )
	{
		returnErrorStudy( "No presentation contexts", " " );
	}

	/* C-FIND-RQ to the other DICOM application and receive corresponding response messages. */
	T_ASC_PresentationContextID presId = ASC_findAcceptedPresentationContextID( assoc, opt_abstractSyntax );
	assert( presId != 0 );
	DIC_US msgId = assoc->nextMsgID++;

	/* prepare the transmission of data */
	T_DIMSE_C_FindRQ req;
	T_DIMSE_C_FindRSP rsp;

	/* prepare the callback data */
	callbackData.assoc = assoc;
	callbackData.presId = presId;
	callbackData.studyList = &studyList;
	callbackData.cancel = false;

	/* prepare the transmission of data */
	bzero( ( char* )&req, sizeof( req ) );
	req.MessageID = msgId;
	strcpy( req.AffectedSOPClassUID, opt_abstractSyntax );
	req.DataSetType = DIMSE_DATASET_PRESENT;
	req.Priority = DIMSE_PRIORITY_LOW;

	/* finally conduct transmission of data */
	SSC_LOG( "Request FIND request..." );
	cond = DIMSE_findUser( assoc, presId, &req, dcmff.getDataset(), progressCallback, &callbackData, DIMSE_NONBLOCKING, 5, &rsp, &statusDetail );
	if ( cond == DIMSE_NODATAAVAILABLE )
	{
		returnErrorStudy( "Find request failed", "Timed out - retry later" );
	}

	/* dump some more general information */
	if ( cond != EC_Normal )
	{
		returnErrorStudy( "Find request failed", "%s", cond.text() );
		dcmff.getDataset()->print( COUT );
		DimseCondition::dump( cond );
	}

	/* Release association */
	SSC_LOG( "Release association..." );
	cond = ASC_releaseAssociation( assoc );
	if ( cond.bad() )
	{
		DimseCondition::dump( cond );
		SSC_LOG( "Release association failed: %s", cond.text() );
	}

	/* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
	/* call is the counterpart of ASC_requestAssociation(...) which was called above. */
	cond = ASC_destroyAssociation( &assoc );
	if ( cond.bad() )
	{
		DimseCondition::dump( cond );
		SSC_LOG( "Could not destroy association: %s", cond.text() );
	}

	/* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
	/* is the counterpart of ASC_initializeNetwork(...) which was called above. */
	cond = ASC_dropNetwork( &net );
	if ( cond.bad() )
	{
		DimseCondition::dump( cond );
		SSC_LOG( "Could not drop network: %s", cond.text() );
	}

	SSC_LOG( "all done" );
	return studyList;
}

int DICOMLib_FetchStudy( const char *studyUID )
{
	char cmd[PATH_MAX];

	ssprintf(cmd, "/usr/local/bin/swmovescu -S -k 0008,0052=STUDY -k 0020,000D=\"%s\"", studyUID);
	SSC_LOG( "Running: %s", cmd );

	// We fork off here to prevent this process from timing out due to lack of ping-pong responsiveness
	if (fork() == 0)
	{
		system( cmd );
		exit( 0 );
	}
	else // parent
	{
		return 0;
	}
}
