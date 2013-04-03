/*
 *  DICOMLib.h
 *  Part of DICOMLib
 *
 *  This is the one and only header for all external users of the DICOMLib library.
 *
 */

#ifndef DICOMLIB_H
#define DICOMLIB_H

#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#ifdef __cplusplus
#include <QMap>
#include <QVariant>
#include <QString>
#endif

/**
 * @defgroup DICOMLib
 * This module deals with importing and sorting DICOM series. In short,
 * we are given paths to disk areas that may contain DICOM files, and
 * output series info, 2D images, and 3D volumes. The series may be validated,
 * 2D images may be scaled, and 3D volumes may be corrected for gantry tilt.
 * Currently supported formats are ACR-NEMA 1.0 and 2.0, and DICOM 3.0.
 *
 * Thread safe except for DICOMLib_Init, DICOMLib_Done and DICOMLib_Config.
 *
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

/* Defined sizes */
#define DICOMLIB_VAL_MAX 255
#define DICOMLIB_LONG_STRING 65		/**< 64 + trailing zero termination. Implements 'LO' Value Representation in DICOM. */
#define DICOMLIB_SHORT_STRING 17	/**< 16 + trailing zero termination. Implements 'SH' Value Representation in DICOM. */
#define DICOMLIB_INFO_MAX 1024 * 16	/**< Maximum size of series information field. */
#define DICOMLIB_PRESET_MAX 8		/**< Maximum number of window settings. */

/* Flags */
#define DICOMLIB_NO_DICOMDIR	0x01	///< Do not parse DICOMDIR
#define DICOMLIB_MEDIA_DIR	0x02	///< Parse this directory as a directory of external media directories (eg /media)
#define DICOMLIB_REINDEX_STUDY	0x04	///< Make sure each identified study is given a unique study uid
#define DICOMLIB_NO_CACHE	0x08	///< Turn off the internal database that caches studies

/**
 *	Search parameter for PACS
 */
enum pacs_search_param_t
{
	DICOMLIB_SEARCH_PATIENT_NAME,
	DICOMLIB_SEARCH_PATIENT_ID,
	DICOMLIB_SEARCH_PATIENT_BIRTH,
	DICOMLIB_SEARCH_STUDY_DATE,
	DICOMLIB_SEARCH_ACCESSION_NUMBER,
	DICOMLIB_SEARCH_NONE
};

/**
 *	Photometric interpretation of stored image data.
 *	@see struct series_t
 */
enum photometric_interpretation_t
{
	/* Supported for MR & CT: */
	DICOMLIB_PI_MONOCHROME1,
	DICOMLIB_PI_MONOCHROME2,
	DICOMLIB_PI_OTHER,			/**< Not supported for MR and CT yet. */
	DICOMLIB_PI_LAST
};

/**
 *	Various configuration parameters.
 *	@see DICOMLib_Config
 */
enum dicomlib_config_type
{
	DICOMLIB_CONF_SPLIT_ACQUISITION,	///< Split images with different acquisition numbers into different series?
	DICOMLIB_CONF_SPLIT_SERIES_DESCR,	///< Split images with different study description into different series?
	DICOMLIB_CONF_SPLIT_SEQUENCE_NAME,	///< Split images with different sequence name into different series?
	DICOMLIB_CONF_MIN_NUM_SLICES,		///< The minimum number of valid slices for a valid series.
	DICOMLIB_CONF_MIN_DIST_SLICES,		///< The minimum distance between positions in a slice.
	DICOMLIB_CONF_MAX_ORIENT_DIFF,		///< The maximum difference between the image orientation patient between two images to be in the same series
	DICOMLIB_CONF_SPLIT_DIRECTORIES,	///< Split images in different directories into different series?
	DICOMLIB_CONF_NETWORK_TIMEOUT,		///< Network timeout in seconds
	DICOMLIB_CONF_MAX_GAP,			///< Multiple of average distance that is the limit before a gap in volume is a problem
	DICOMLIB_CONF_LAST
};

enum dicomlib_volume_alignment
{
	DICOMLIB_ALIGNMENT_TABLE,			///< No patient orientation
	DICOMLIB_ALIGNMENT_ESTIMATED,		///< Patient orientation given with navigator tool
	DICOMLIB_ALIGNMENT_REGISTRATION		///< Patient orientation given by volume registration
};

enum dicomlib_probe_type
{
	DICOMLIB_PROBE_NONE,
	DICOMLIB_PROBE_SECTOR,				///< Sector phased beam probe
	DICOMLIB_PROBE_LINEAR				///< Linear fixed beam probe
};

enum dicomlib_application_profile
{
	DICOMLIB_PROFILE_DVDRAM,			///< PS 3.11 Annex D, STD-GEN-DVD-RAM multi-frame enhanced object
	DICOMLIB_PROFILE_DVDRAM_UNENHANCED,		///< PS 3.11 Annex D, STD-GEN-DVD-RAM single-frame unenhanced object
	DICOMLIB_PROFILE_USB,				///< PS 3.11 Annex J, STD-GEN-USB-JPEG multi-frame enhanced object
	DICOMLIB_PROFILE_USB_UNENHANCED			///< PS 3.11 Annex J, STD-GEN-USB-JPEG single-frame unenhanced object
};

enum dicomlib_warning_type
{
	DICOMLIB_WARNING_ACR_NEMA_1,
	DICOMLIB_WARNING_ACR_NEMA_2,
	DICOMLIB_WARNING_UNKNOWN_SOP_CLASS
};

struct image_window_t
{
	double center;
	double width;
};

struct image_lut_t
{
	void *table;						///< If non-NULL, use this LUT instead of window settings.
	int bits;
	int samples;
	int start;
	int length;
};

struct window_preset_t
{
	char explanation[DICOMLIB_LONG_STRING];
	struct image_window_t window;
};

/**
 *	One instance, or image, in a 3D series.
 */
struct instance_t
{
	char path[DICOMLIB_VAL_MAX];	/**< Full path to file */
	double image_position[3];	/**< See DICOM standard C.7.6.2.1.1 */
	double slice_normal_distance;	/**< Distance along the slice normal in mm, used for sorting. */
	uint32_t instance_id;		/**< Generated internally, this is *not* the DICOM instance number! Probably superfluous now. */
	uint32_t frame;			/**< Frame inside a multi-frame DICOM file that this instance represents, starting at zero. */
	struct window_preset_t preset[DICOMLIB_PRESET_MAX];
	int numPresets;
	int internal_bits_per_sample;	/**< Internally saved format in DCMTK. Ugly hack because it may differ from source bps. */
	bool isSigned;			/**< Is the pixel data signed or unsigned? */
	char SOPInstanceUID[DICOMLIB_LONG_STRING];	///< Unique ID for this object

	/* Private data - hands off */
	struct instance_t *next_instance;	///< Next instance in linked list.
	struct series_t *parent_series;		///< Series that this instance belongs to.
};

struct volume_t
{
	int x, y, z, bits_per_sample, samples_per_pixel;
	void *volume;
	double image_orientation[6];		/**< See DICOM standard C.7.6.2.1.1 */
	double pixel_spacing[3];		/**< Distance between pixels in the volume in mm */
	struct image_window_t voi;		/**< Current VOI settings (transformed) */
	unsigned int firstpixel, lastpixel;	/**< Lowest and highest pixel value in volume */
	int rescaleIntercept;			/**< Append transform applied to each value in volume */
};

struct ultrasound_t
{
	double MI;						///< Mechanical Index
	double TI;						///< Thermal Index
	char transducerData[DICOMLIB_LONG_STRING];		///< Free form description of probe
	char scannerName[DICOMLIB_LONG_STRING];			///< Name of US scanner
	char application[DICOMLIB_LONG_STRING];			///< Name of scanner probe application used for acquisition
	char transducerSerial[DICOMLIB_LONG_STRING];		///< Serial number of probe
	enum dicomlib_probe_type probeType;			///< Type of probe used to acquire this series
	bool flowData;						///< If ultrasound, this indicates that the data is flow data.
	char acquisitionDateTime[DICOMLIB_LONG_STRING];		///< Time of acquisition
	uint64_t acquisitionDuration;				///< Number of milliseconds
};

/**
 *	One series containing (hopefully) 3D image data. path_id and series_id combine to form our unique primary key
 *	for getting series data.
 */
struct series_t
{
	uint32_t series_id;					///< Unique number for each series in this study
	int frames;						///< The number of frames in this series. Used for multi frame and mosaic
	double image_orientation[6];				///< See DICOM standard C.7.6.2.1.1
	double pixel_spacing[2];				///< Distance between pixels in the plane in mm
	uint16_t bits_per_sample;				///< Bits allocated for each sample
	uint16_t samples_per_pixel;				///< Number of samples per pixel, eg RGB is 3 samples.
	uint16_t rows;						///< Height of image in pixels.
	uint16_t columns;					///< Width of image in pixels.
	char SOPClassUID[DICOMLIB_LONG_STRING];			///< Unique ID for SOP Class (set only internally)
	char repetitionTime[DICOMLIB_LONG_STRING];
	char seriesInstanceUID[DICOMLIB_LONG_STRING];		///< Unique ID for series (redefined internally)
	char seriesID[DICOMLIB_LONG_STRING];			///< Series name
	char seriesTime[DICOMLIB_LONG_STRING];			///< Time series was initially created.
	char seriesDate[DICOMLIB_LONG_STRING];			///< Date series was created.
	char seriesDescription[DICOMLIB_LONG_STRING];		///< Series description, if any.
	char acquisition[DICOMLIB_LONG_STRING];			///< The acquisition name. Not used by default.
	char frameOfReferenceUID[DICOMLIB_LONG_STRING];		///< Frame of reference ID that is unique for each separate patient coordinate system
	char modality[DICOMLIB_SHORT_STRING];			///< Modality.
	char series_info[DICOMLIB_INFO_MAX];			///< Misc messages related to reading in this series are stored here.
	char error[DICOMLIB_INFO_MAX];				///< Just the error part (hack)
	char echo_number[DICOMLIB_SHORT_STRING];		///< If this is part of a multi-echo MR series, the echo number of this series is here.
	char sequenceName[DICOMLIB_SHORT_STRING];
	char splitReason[DICOMLIB_SHORT_STRING];		///< Reason for splitting this series from others -- for debugging
	bool valid;						///< If series is valid. If not, see error message in series_info.
	bool multiframe;					///< Whether this is a multi-frame series or not
	bool mosaic;						///< Whether this series comes from a Siemens mosaic
								///< --> frames tag contains number of images
	char patient_orientation[4][2];				///< See DICOM standard C.7.6.1.1.1, if applicable.
	struct series_t *next_series;				///< Next series in the linked list.
	struct study_t *parent_study;				///< Owning study
	struct volume_t *volume;				///< Reference to 3D volume that may be set by user; if so, cleaned up on exit.
	double slice_thickness;					///< After conversion, this will contain pixel spacing in the z dimension.
	struct ultrasound_t us;

	struct instance_t *first_instance;			///< Pointer to the linked list of instances.

	struct {
		struct image_window_t range;			///< Possible value range of data in this series.
		struct image_window_t current;			///< Currently applied window settings.
		struct image_window_t suggestion;		///< Automatically generated window setting suggestions by DICOMLib.
		struct image_window_t minmax;			///< Actual data range in this series.
		struct window_preset_t preset[DICOMLIB_PRESET_MAX];	///< List of preset window settings and/or LUT
		int numPresets;					///< How many stored window values.
		int currentPreset;				///< Which preset to use, if any. -1 if none.
		struct image_lut_t lut;
	} VOI;
	struct {
		double bvec[3];					///< DTI gradient direction for diffusion weighting
		char bval[DICOMLIB_SHORT_STRING];		///< DTI B-value for diffusion weighting
		char directionality[DICOMLIB_LONG_STRING];	///< DTI directionality string from DICOM
		bool isDTI;					///< Whether this Series contain DTI ( Diffusion Weighted multi-directional MR data )
#ifdef __cplusplus
		QVariantMap *csaImageMap;				///< CSA data dictionary
		QVariantMap *csaSeriesMap;				///< CSA data dictionary
#else
		void *csaImageMap;					// horrible hack to avoid porting everything to C++ right now...
		void *csaSeriesMap;					// horrible hack to avoid porting everything to C++ right now...
#endif
	} DTI;
	double firstpixel, lastpixel;				///< Similar to minmax above, just without conversion to window values.
	enum dicomlib_warning_type warning;			///< There is a reason for a user warning
	char rootpath[PATH_MAX];				///< Root path of series
};

struct study_t
{
	uint32_t study_id;					///< Internally unique number for this study.
	char patientName[DICOMLIB_LONG_STRING];			///< Patient name in normalized DICOM form, eg Mathisen^Per Inge
	char patientID[256];					///< Patient ID. Length is a hack for passing an error message thru it...
	char patientBirthDate[DICOMLIB_LONG_STRING];		///< Patient birth date
	char patientSex[DICOMLIB_SHORT_STRING];			///< Patient gender
	char studyDate[DICOMLIB_LONG_STRING];			///< Study date of acquisition
	char studyTime[DICOMLIB_LONG_STRING];			///< Study time of acquisition
	char studyID[DICOMLIB_SHORT_STRING];			///< Study name
	char accessionNumber[DICOMLIB_LONG_STRING];		///< Accession number (implementation-defined but important)
	char studyInstanceUID[DICOMLIB_LONG_STRING + 4];	///< Unique ID for study (redefined internally)
	char institutionName[DICOMLIB_LONG_STRING];		///< Name of institution using the system (only written)
	char softwareVersion[DICOMLIB_SHORT_STRING];		///< Software release number (only written)
	char serialNumber[DICOMLIB_SHORT_STRING];		///< Serial number of system (only written)
	char studyDescription[DICOMLIB_LONG_STRING];		///< Study description
	char manufacturer[DICOMLIB_LONG_STRING];		///< Manufacturer (important for parsing non-standard information)
	char modalities[DICOMLIB_LONG_STRING];			///< List of modalities in study (for PACS)

	struct study_t *next_study;				///< Next study in this linked list
	struct series_t *first_series;				///< Pointer to linked list of series in the study
	char resource[PATH_MAX];				///< Path or URL to source resource
	int port;						///< If applicable, port number to PACS server
	int series_count;					///< May be -1 if series are not read yet
	bool valid;						///< Hide from list if false
	bool reindex;						///< Force study indices to be unique if split
	char rootpath[PATH_MAX];				///< Root path of study
	bool initialized;					///< If lazy loading initialization has been run for this study
	char SpecificCharacterSet[DICOMLIB_LONG_STRING];	///< Character set encoding  for export see DICOM Part 3 C.12.1.1.2
};

/**
 *	This callback will be called repeatedly during tasks known to take long, with a parameter giving it
 *	a permille estimate of how far in the process we are. If the callback returns a non-zero return value,
 *	the process will be aborted.
 */
typedef int progress_func_t(int);

/**
  * Linked list of files to process to identify whether they are DICOM files or not. We need to create
  * such a list in advance in order to generate a progress indication.
  */
struct filenode
{
	struct filenode *next;
	char path[PATH_MAX];
};

// --- Initializers ---

int DICOMLib_Init( void );	///< Initialize DICOMLib.
int DICOMLib_Done( void );	///< Clean up DICOMLib.

/**
 *	Setup connection to a PACS server. May test the connection. Returns zero on success.
 */
int DICOMLib_Network( const char *appAETitle, const char *peerAETitle, const char *destAETitle, const char *peerIP, int peerPort );

/** 
 *	Create a list of studies from a path. This is the slowest initializer, and
 *	therefore takes a progress report callback (which may be NULL).
 */
struct study_t *DICOMLib_StudiesFromPath( const char *path, progress_func_t *callback, int flags );

/** 
 *	Create a list of studies from a DICOMDIR file. This should be fast.
 */
struct study_t *DICOMLib_StudiesFromDICOMDIR( const char *path );

/** 
 *	Create a list of studies from a PACS station based on search parameters. This may
 *	be slow, but due to the method of communication, it is not feasible to report 
 *	incremental progress. If searchString is a date, it must be in the format YYYYMMDD 
 *	exactly 8 characters long. timeSpan must be either in "YYYYMMDD-YYYYMMDD" format or NULL.
 *	It makes no sense to give both a non-NULL timeSpan and a study date relatedTo parameter,
 *	and timeSpan will be ignored in this case.
 */
struct study_t *DICOMLib_StudiesFromPACS( const char *searchString, enum pacs_search_param_t relatedTo, const char *timeSpan );

/**
 *	Request that a study be sent from PACS to us. Remember to set our AETITLE first, both locally and in the PACS.
 */
int DICOMLib_FetchStudy( const char *studyUID );

/**
 *	Incrementally add new studies to existing list of studies by reading files, like DICOMLib_StudiesFromPath.
 *	Tries to use DICOMDIR if available, unless flags tells us otherwise. studyList can be NULL.
 *	@todo If it weren't for that stupid indexing, we could just combine this with DICOMLib_StudiesFromPath instead.
 *	In the future, let's generate new UIDs for legacy objects without them and use those to index internally.
 */
struct study_t *DICOMLib_GetStudies( struct study_t *studyList, const char *path, progress_func_t *callback, int flags );


// --- Cleanup ---

/**
 *	Close down list of studies properly. Will close and free child series and image as well. Give it the first study in the list.
 */
int DICOMLib_CloseStudies( struct study_t *study );


// --- Configuration ---

/**
 *	Set various configuration parameters.
 *	@see dicomlib_config_type
 */
int DICOMLib_Config( enum dicomlib_config_type c, double val );

/// Read configuration
double DICOMLib_GetConfig( enum dicomlib_config_type c );

// --- Utilities ---

/**
 *	Fill an image into given RGBA memory buffer, centered and resized with respect to correct aspect ratio.
 */
int DICOMLib_Image_RGB_Fill( const struct series_t *series, int sizeX, int sizeY, int frame, char *image );

/**
 *	Return an image slice from the given series. Give suggested size and frame, and check actual size
 *	that is returned. Do not free the returned buffer.
 */
char *DICOMLib_Image( const struct series_t *series, int *sizeX, int *sizeY, int bits_per_sample, int frame );

/**
 *	Delete the given study from the disk.
 *	@todo Prune empty directories left over from delete study operation.
 */
int DICOMLib_DeleteStudy( struct study_t *study );

/**
 *	Delete the given series from the disk.
 *	@todo Prune empty directories left over from delete series operation.
 */
int DICOMLib_DeleteSeries( struct series_t *series );

/**
 *	Write study to SONOWAND multi-frame DICOM volume format.
 *	See DICOM Conformance Statement for information on contents.
 */
int DICOMLib_WriteSeries( const char *filename, const struct study_t *study, bool anonymize,
			  enum dicomlib_application_profile profile, enum dicomlib_volume_alignment aligned );


// --- Accessors ---

/** 
 * 	Generate and return a list of series from the given study. It will cache the result, and repeated calls
 *	will not recalculate the result. It may take some time, and the callback may be called repeatedly during 
 *	the process. Do not free the result.
 */
struct series_t *DICOMLib_GetSeries( struct study_t *study, progress_func_t *callback );

/**
 *	Generate and return a volume block from a given series. It may take some time, and the callback 
 *	may be called repeatedly during the call. Caller must free the resulting buffer.
 */
struct volume_t *DICOMLib_GetVolume( struct series_t *series, progress_func_t *callback );
void DICOMLib_FreeVolume( struct volume_t *volume);
	
/**
 *	Add a 8bpp LUT preset with given start value and length. If there are
 *	window settings, they are discarded.
 */
int DICOMLib_SetVOILUT( struct series_t *series, int start, int length, char *lut );

/**
 *	Set series window settings (instead of LUT). If there is a LUT, it is discarded. If
 *	center and width are negative, an automatic min/max window setting is applied.
 */
int DICOMLib_SetWindow( struct series_t *series, double center, double width );

/**
 *	Set window settings and/or LUT according to given window preset. Returns zero for window, one for LUT,
 *	and a negative value on error.
 */
int DICOMLib_UsePreset( struct series_t *series, int index );

/**
 *	Try to use optimal VOI window settings. If a preset is being used, it is discarded.
 */
int DICOMLib_UseAutoVOI( struct series_t *series );

/**
 *	Set window settings and/or LUT according to given window preset. Returns zero for window, one for LUT,
 *	and a negative value on error.
 */
int DICOMLib_NoVOI( struct series_t *series );

/**
 *	Get current series window settings and return zero. If there is a LUT, approximate window settings
 *	are calculated and stored in center and width, and the function returns one.
 */
int DICOMLib_GetWindow( struct series_t *series, double *center, double *width );

/**
 *	Export fiducials.
 */
int DICOMLib_WriteRegistration( const char *filename, const struct study_t *study, const struct series_t *series,
				bool anonymize, int numFiducials, double *fiducials, int countCutplanes, double *cutplanes );

/**
 *	Import fiducials. Preallocate buffers to store them in, with 3x doubles for fiducials and 9x doubles for cut planes.
 */
int DICOMLib_LoadRegistration( const char *filename, int *numFiducials, double *fiducials, int *numCutplanes, double *cutplanes );

/**
 *	Import direction of gravity. Preallocate buffer to store it in, 16 doubles in size.
 */
int DICOMLib_LoadGravity( const char *filename, double *gforce );

/**
 *	Export direction of gravity (gforce registration).
 */
int DICOMLib_WriteGravity( const char *filename, const struct study_t *study, const struct series_t *series, bool anonymize, double *gforce );

/**
 *	Describe the patient orientation axis in one direction. Axis must be either 0 for X or 1 for Y. Set inverse to describe the inverse axis.
 */
char DICOMLib_DescribeOrientation( const struct series_t *series, int axis, bool inverse );

/**
 *  Create a new frame of reference UID.
 */
const char *DICOMLib_FrameOfReferenceUID();

/**
 *  Create a new study UID.
 */
const char *DICOMLib_StudyUID();

/**
 *  Create a new series UID.
 */
const char *DICOMLib_SeriesUID();

/** Export old style unenhanced DICOM series. */
int DICOMLib_WriteUnenhancedSeries( const char *basefilename, const struct study_t *study, bool anonymize, enum dicomlib_application_profile profile );

/** Export snapshots as DICOM encapsulated PDF */
int DICOMLib_WriteSnapshot( const char *filename, const uint8_t *pdfbuffer, int size, const struct study_t *study );

/** Export snapshots as DICOM SC JPEG */
int DICOMLib_WriteImageSnapshot( const char *filename, const uint8_t *rgb, int bytes, int width, int height, const struct study_t *study, bool compress );

/**
 *	Output a slice with Modality LUT but without any VOI or Presentation LUT applied, nor with any
 *	overlay or other additional information added. Uses DCMTK's inter data format.
 */
const void *DICOM_raw_image(const struct series_t *series, struct instance_t *instance, int frame );

int DICOMLib_Verify( struct series_t *series );

/**
 * Exports all DTI-series from given study in nifti format incl. bval and bvec files to disc
 **/
int DICOMLib_WriteNifti( const char *filename, const struct study_t *study );

/** For testing only */
bool DICOMLib_INTERNAL_TEST(void);

#ifdef __cplusplus
}
#endif // __cplusplus

/* @} */

#endif
