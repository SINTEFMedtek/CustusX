/*
 *  DICOMLib.c
 *
 *  See header file for documentation.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <math.h>
#include <sys/dir.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <strings.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sqlite3.h>

#include "sscLogger.h"
#include "DICOMLib.h"
#include "DCMTK.h"
#include "vector3d.h"
#include "identify.h"

#define SLICE_COUNT_FILES 300			///< Time slice in progress bar for counting
#define SLICE_PARSE_FILES 700			///< Time slice in progress bar for parsing
#define CALLBACK_TIMESLICE 25000		///< Microseconds between each progress update

static double config[DICOMLIB_CONF_LAST];	///< Global configuration parameters

static const char *dbname = "/Data/Cache/dicom";
static const char *sqlinit =
	"CREATE TABLE IF NOT EXISTS studies(studyuid text not null, patientsname text, patientid text, birthdate text, "
	"	accessionnumber text, studyid text, studydate text, path text not null);";

static bool dbquery(sqlite3 *db, const char *statement, int (*callback)(void*,int,char**,char**), void *cbdata);
static struct study_t *studiesFromNodes( struct study_t *root, struct filenode *node, progress_func_t *callback, int progress, bool keepRoot, int flags, const char *rootdir );

int DICOMLib_Init()
{
	/* make sure data dictionary is loaded */
	if ( !dcmDataDict.isDictionaryLoaded() )
	{
		SSC_ERROR( "Error: no data dictionary loaded, check environment variable: %s (contains %s)",
			   DCM_DICT_ENVIRONMENT_VARIABLE, getenv( DCM_DICT_ENVIRONMENT_VARIABLE ) );
		return ENOSYS;
	}

	/* Do not trust the transfer syntax given in the meta file info header. Some implementations
	 * put the wrong info in there. */
	dcmAutoDetectDatasetXfer.set( true );

	config[DICOMLIB_CONF_SPLIT_ACQUISITION] = false;
	config[DICOMLIB_CONF_SPLIT_SERIES_DESCR] = false;
	config[DICOMLIB_CONF_SPLIT_SEQUENCE_NAME] = false;
	config[DICOMLIB_CONF_MIN_NUM_SLICES] = 6;
	config[DICOMLIB_CONF_MIN_DIST_SLICES] = 0.0001;
	config[DICOMLIB_CONF_MAX_ORIENT_DIFF] = 0.001;
	config[DICOMLIB_CONF_SPLIT_DIRECTORIES] = true;
	config[DICOMLIB_CONF_NETWORK_TIMEOUT] = 40;	// 40 seconds default
	config[DICOMLIB_CONF_MAX_GAP] = 1.5;
	DICOMLib_Network( NULL, NULL, NULL, NULL, -1 );		// pre-initialize
	DICOM_Init();
	return 0;
}

int DICOMLib_Done()
{
	DICOM_Done();
	return 0;
}

static struct instance_t *findInstance( struct series_t *series, int instance_id )
{
	int i;
	struct instance_t *instance = series->first_instance;

	for ( i = 0; i < instance_id && instance != NULL; i++, instance = instance->next_instance );
	return instance;
}

static int setupSeries( struct study_t *study, progress_func_t *callback = NULL )
{
	if ( study->initialized == true )
	{
		return 0;
	}
	struct series_t *series;
	QList<QString> DTIUidList;

	for ( series = study->first_series; series != NULL; series = series->next_series )
	{
		// catch all series with contain DTI stuff
		if (series->DTI.isDTI) DTIUidList.append(series->seriesInstanceUID);
	}
	// DTI: Now separate the wheat from the chaff (real DTI from trash)
	for  ( series = study->first_series; series != NULL; series = series->next_series )
	{
		/*
		// DTI data should usally contain more than 7 diffusion volumes
		// if we had found diffusion parameters anyway this could be ADC maps or
		// pre calculated DTI results so we reset the isDTI tag to false
		*/
		if (series->DTI.isDTI)
			if ( DTIUidList.count(series->seriesInstanceUID)<7 )
				series->DTI.isDTI=false;
	}

	for ( series = study->first_series; series != NULL; series = series->next_series )
	{
		if (callback)
		{
			callback(1000);
		}
		struct instance_t *middle = findInstance( series, series->frames / 2 );
		DICOMLib_Verify( series );
		if ( !middle ) // eg in multiframe case
		{
			middle = series->first_instance;
		}

		// Set auto values besides DTI data - except for the representative bval=0 series in DTI data
		// FIXME - we should move auto value setting from here to when it is actually used, since it is
		// quite slow for large series, where some series may be invalid or not even images
		if (series->DTI.isDTI != true || strcmp(series->DTI.bval, "0") == 0)
		{
			if ( middle && DICOM_image_window_auto( series, middle ) == 0 )
			{
				series->VOI.current = series->VOI.suggestion;
				series->VOI.currentPreset = -1;
				SSC_LOG( "Found auto values %f, %f from frame %d in (file %s) -- minmax (%f, %f), range (%f, %f)",
					series->VOI.suggestion.center, series->VOI.suggestion.width,
					series->frames / 2, middle->path, series->VOI.minmax.center, series->VOI.minmax.width,
					series->VOI.range.center, series->VOI.range.width );
			}
			else // should never happen ...
				SSC_LOG( " Found no auto values %f, %f from frame %d in (file %s) -- minmax (%f, %f), range (%f, %f)",
				      series->VOI.suggestion.center, series->VOI.suggestion.width,
				      series->frames / 2, middle->path, series->VOI.minmax.center, series->VOI.minmax.width,
				      series->VOI.range.center, series->VOI.range.width );
		}
	}
	study->initialized = true;
	return 0;
}

static void freeInstances( struct series_t *s )
{
	struct instance_t *i = s->first_instance, *i2;

	while ( i != NULL )
	{
		i2 = i->next_instance;
		free( i );
		i = i2;
	}
}

static void freeSeries( struct series_t *s )
{
	free( s->VOI.lut.table );
	delete s->DTI.csaImageMap;
	delete s->DTI.csaSeriesMap;
	if ( s->volume )
	{
		DICOMLib_FreeVolume(s->volume);
	}
}

static void freeSeriesFromStudy( struct study_t *p )
{
	struct series_t *s = p->first_series, *next;

	while ( s != NULL )
	{
		freeInstances( s );
		next = s->next_series;
		freeSeries( s );
		free( s );
		s = next;
	}
	p->series_count = 0;
	p->first_series = NULL;
}

int DICOMLib_CloseStudies( struct study_t *study )
{
	struct study_t *next;

	for (; study; study = next)
	{
		next = study->next_study;

		freeSeriesFromStudy( study );
		free( study );
	}
	return 0;
}

/// Add single frame to instance list
static void add_instance( struct series_t *series, const struct instance_t *instance )
{
	struct instance_t *addition = (struct instance_t *)calloc( 1, sizeof( *addition ) );
	struct instance_t *iter_instance = series->first_instance;
	double normalv[3];

	/* Calculate slice distance */
	memcpy( addition, instance, sizeof( *instance ) );

	vector3d_cross_product( normalv, &series->image_orientation[0], &series->image_orientation[3] );
	addition->slice_normal_distance = vector3d_inner_product( normalv, addition->image_position );

	/* Find position in linked list - it is always correctly sorted */

	/* First case - empty list */
	if ( series->first_instance == NULL )
	{
		series->first_instance = addition;
		addition->next_instance = NULL;
		return;
	}

	/* Second case - lowest value of all, add to beginning */
	if ( addition->slice_normal_distance < series->first_instance->slice_normal_distance )
	{
		addition->next_instance = series->first_instance;
		series->first_instance = addition;
		return;
	}

	/* Third case - search list. We add ourselves after a node if its next node is smaller than our new value. */
	while ( iter_instance->next_instance != NULL
	        && addition->slice_normal_distance > iter_instance->next_instance->slice_normal_distance )
	{
		iter_instance = iter_instance->next_instance;
	}
	addition->next_instance = iter_instance->next_instance;
	iter_instance->next_instance = addition;
}

/// Add single series to study list
static void add_series( struct study_t *iter, struct series_t *series, const struct instance_t *instance )
{
	struct series_t *addition = (struct series_t *)calloc( 1, sizeof( *addition ) );

	memcpy( addition, series, sizeof( *series ) );
	series->VOI.lut.table = NULL;
	series->volume = NULL;
	
	assert(series->next_series == NULL);
	if ( iter->first_series == NULL )
	{
		/* Create beginning of list */
		iter->first_series = addition;
		addition->next_series = NULL;
	} else {
		/* Add to beginning of list */
		addition->next_series = iter->first_series;
		iter->first_series = addition;
	}
	add_instance( addition, instance );
}

/// Add single series to study list
static void add_study( struct study_t *studyList, struct study_t *study, struct series_t *series, const struct instance_t *instance )
{
	if ( studyList->studyID[0] == '\0' )	// we have only the first, empty study
	{
		/* Create beginning of list */
		memcpy( studyList, study, sizeof( *studyList ) );
		studyList->next_study = NULL;
		add_series( studyList, series, instance );
		studyList->valid = true;
	} else {
		struct study_t *addition = (struct study_t *)calloc( 1, sizeof( *addition ) );

		// Add it behind the first study, weird ordering, but has to be this way
		memcpy( addition, study, sizeof( *addition ) );
		addition->next_study = studyList->next_study;
		addition->valid = true;
		studyList->next_study = addition;
		add_series( addition, series, instance );
	}
}

/** See if this new file fits into an existing study or series, or requires a new series */
#define SPLITBY(_reason) { sstrcpy(series->splitReason, _reason); continue; }
static void insert_into_studylist( struct study_t *studyList, struct study_t *study, struct series_t *series, const struct instance_t *instance )
{
	struct study_t *firstStudy = studyList;

	/* Search through all existing studies and series and see if there is a match */
	for ( ; studyList; studyList = studyList->next_study )
	{
		struct series_t *seriesList = studyList->first_series;

		/* Compare patient, study, series, modality and echo_number. */
		if ( study && sstrcmp( studyList->patientName, study->patientName ) != 0 ) continue;
		if ( study && sstrcmp( studyList->studyID, study->studyID ) != 0 ) continue;
		if ( study && sstrcmp( studyList->studyInstanceUID, study->studyInstanceUID ) != 0 ) continue;
		/* Check if split by path (not valid for DICOMDIR) */
		if (config[DICOMLIB_CONF_SPLIT_DIRECTORIES] != 0 && study->port == 0 && sstrcmp(study->resource, studyList->resource) != 0)
		{
			continue;
		}

		/* Study is right, see if we have a matching series, too. */
		for ( ; seriesList; seriesList = seriesList->next_series )
		{
			if ( sstrcmp( seriesList->seriesID, series->seriesID ) != 0 ) SPLITBY("seriesID");
			if ( sstrcmp( seriesList->seriesInstanceUID, series->seriesInstanceUID ) != 0 ) SPLITBY("instanceUID");
			if ( config[DICOMLIB_CONF_SPLIT_SERIES_DESCR] && sstrcmp( seriesList->seriesDescription, series->seriesDescription ) != 0 ) SPLITBY("seriesDescr");
			if ( config[DICOMLIB_CONF_SPLIT_ACQUISITION] && sstrcmp( seriesList->acquisition, series->acquisition ) != 0 ) SPLITBY("acq");
			if ( sstrcmp( seriesList->echo_number, series->echo_number ) != 0 ) SPLITBY("echo");
			if ( sstrcmp( seriesList->modality, series->modality ) != 0 ) SPLITBY("modality");
			if ( config[DICOMLIB_CONF_SPLIT_SEQUENCE_NAME] && sstrcmp( seriesList->sequenceName, series->sequenceName ) != 0 ) SPLITBY("seq");
			/* if DTI && different bvectors -> split
			(there is for each bvector one volume) */
			if ( seriesList->DTI.isDTI && series->DTI.isDTI )
			{
				double bvecDiff[3];
				vector3d_subtract(bvecDiff,seriesList->DTI.bvec,series->DTI.bvec);
				if ( ( fabs(vector3d_length(bvecDiff)) > 0.00001 ) || ( strcmp( seriesList->DTI.bval, series->DTI.bval)!=0))
					SPLITBY("bvec");
			}
			/* Check image orientation */
			if ( fabs( series->image_orientation[0] - seriesList->image_orientation[0] ) > config[DICOMLIB_CONF_MAX_ORIENT_DIFF] ||
			     fabs( series->image_orientation[1] - seriesList->image_orientation[1] ) > config[DICOMLIB_CONF_MAX_ORIENT_DIFF] ||
			     fabs( series->image_orientation[2] - seriesList->image_orientation[2] ) > config[DICOMLIB_CONF_MAX_ORIENT_DIFF] ||
			     fabs( series->image_orientation[3] - seriesList->image_orientation[3] ) > config[DICOMLIB_CONF_MAX_ORIENT_DIFF] ||
			     fabs( series->image_orientation[4] - seriesList->image_orientation[4] ) > config[DICOMLIB_CONF_MAX_ORIENT_DIFF] ||
			     fabs( series->image_orientation[5] - seriesList->image_orientation[5] ) > config[DICOMLIB_CONF_MAX_ORIENT_DIFF] )
			{
				SPLITBY("orientation");
			}


			/* If DTI: check if same image position in instance list -> split
			(usually there are multiple volumes which we want to split each in to one series) */
			if ( seriesList->DTI.isDTI )
			{
				struct instance_t *instanceList = seriesList->first_instance;
				double distance[3];
				float dist=0;
				bool split=false;
				for ( ;	 instanceList; instanceList = instanceList->next_instance )
				{
					vector3d_subtract(distance, instance->image_position,instanceList->image_position );
					dist=fabs(vector3d_length(distance));
					if ( dist < config[DICOMLIB_CONF_MIN_DIST_SLICES] )
					{
						split=true;
						break;
					}
				}

				if ( split) SPLITBY("volume")
			}

			// TODO: Split by Image Type also

			/* If we came this far, we have a match */
			break;
		}
		/* We found a match - add instance to it */
		if ( seriesList != NULL )
		{
			/* We assume that we never re-read the same volume twice, so we just dump the found instance into the series and sort it later. */
			add_instance( seriesList, instance );
			return;
		}
		/* No matching series found, but matching study found. Add to study. */
		add_series( studyList, series, instance );
		return;
	}

	/* Create a new study */
	if ( !study )
	{
		SSC_LOG( "Tried to add study when no study was given" );
		return;
	}
	SSC_LOG( "Create new study: %s (%s)", study->studyID, study->patientName );
	add_study( firstStudy, study, series, instance );
}

// Fancy function to find longest common shared path
static void longestSharedPath(const char *fixed, char *moving)
{
	char *t;
	unsigned n = 0;
	while (moving[n] == fixed[n] && moving[n] && fixed[n]) n++;
	if (!moving[n] && (!fixed[n] || fixed[n] == '/')) return;
	moving[n] = '\0';
	t = strrchr(moving, '/');
	if (t && t != moving) *t = '\0';	// drop conflicting remainder
	else moving[1] = '\0';		// keep filesystem root
}

/** Make sure the studies, series and instances are properly indexed and linked */
static void index_objects( struct study_t *studyList, int start, bool reindex )
{
	char extra[5];
	int study_count = start;

	for ( struct study_t *study = studyList; study; study = study->next_study )
	{
		if ( reindex )
		{
			// Force studies to be unique, even if their StudyInstanceUID is not. Fixes
			// problems with incorrectly cloned studies that do not update this field.
			ssprintf(extra, "-%03d", study_count);
			sstrcat(study->studyInstanceUID, extra);
		}

		study->study_id = study_count++;
		study->series_count = 0;

		for ( struct series_t *iter = study->first_series; iter != NULL; iter = iter->next_series )
		{
			int k = 0;

			iter->parent_study = study;
			iter->series_id = study->series_count;
			study->series_count++;

			for (struct instance_t *iter_instance = iter->first_instance; iter_instance != NULL; iter_instance = iter_instance->next_instance, k++ )
			{
				iter_instance->instance_id = k;
				iter_instance->parent_series = iter; /* it should know its parent in case we only pass the instance */
			}

			if ( iter->frames == 0 )
			{
				iter->frames = k;	// not multi-frame, so number of frames is identical to number of instances
			}
		}
	}
}

static struct study_t *readSeriesFromDICOMDIR( struct study_t *study, progress_func_t *callback )
{
	char *tmp, path[PATH_MAX];
	// Generate a list of files associated with a given study
	struct filenode *node = DICOM_Dir_Nodes( study );

	if ( !node )
	{
		SSC_LOG( "failed" );
		return NULL;
	}
	sstrcpy(path, study->resource);
	tmp = strrchr(path, '\0');
	if (tmp) *tmp = '\0';

	if (!studiesFromNodes( study, node, callback, 30, true, 0, path ))
	{
		SSC_LOG("canceled");
		return NULL;
	}

	/* Set indexing values */
	index_objects( study, study->study_id, false );
	return study;
}

struct study_t *DICOMLib_GetStudies( struct study_t *studyList, const char *path, progress_func_t *callback, int flags )
{
	struct study_t *additions = NULL;

	if ( !path )
	{
		SSC_LOG( "Bad parameter" );
		return NULL;
	}
	if ( ! ( flags & DICOMLIB_NO_DICOMDIR ) )
	{
		char buf[PATH_MAX];
		sstrcpy( buf, path );
		if ( buf[strlen( buf ) - 1] != '/' )
		{
			sstrcat( buf, "/" );
		}
		sstrcat( buf, "DICOMDIR" );
		additions = DICOMLib_StudiesFromDICOMDIR( buf );
		SSC_LOG( "reading dicomdir %s", buf );
	}
	if ( !additions )
	{
		SSC_LOG( "using !dicomdir" );
		additions = DICOMLib_StudiesFromPath( path, callback, flags );
	}
	if ( !additions )
	{
		SSC_LOG( "Nothing found" );
		return studyList;
	}

	if ( !studyList )
	{
		SSC_LOG( "first study list" );
		return additions;		// new study list
	}
	else
	{
		struct study_t *last;		// must add new additions to last existing study
		int study_count;
		struct study_t *study;

		SSC_LOG( "concatenating study list" );
		for ( last = studyList; last->next_study; last = last->next_study );
		study_count = last->study_id + 1;
		for ( study = additions; study; study = study->next_study )
		{
			study->study_id = study_count++;
		}
		last->next_study = additions;
		return studyList;
	}
}

/**
  *	Update progress bar through callback. est_min is estimated start of where we should be at this time, while
  *	est_max is the maximum of where we should be at this time.
  */
static inline bool maybeCallback( struct timeval *last, progress_func_t *callback, int *progress, int est_min, int est_max )
{
	struct timeval now, diff;
	const int current = *progress;

	if ( !callback )
	{
		return 0;
	}
	gettimeofday( &now, NULL );
	timersub( &now, last, &diff );
	if ( diff.tv_usec > CALLBACK_TIMESLICE )
	{
		*progress = MAX( est_min, current );
		*progress = MIN( 1000, current + ( est_max - est_min ) / ( current + 1 ) );
		return callback( *progress );
	}
	return 0;
}

static int dbstudycallback(void *userdata, int cols, char **data, char **colnames)
{
	struct study_t **studyList = (struct study_t **)userdata;
	struct study_t *study = (struct study_t *)calloc(1, sizeof(*study));
	(void)cols;
	(void)colnames;
	sstrcpy(study->studyInstanceUID, data[0]);
	sstrcpy(study->patientName, data[1]);
	sstrcpy(study->patientID, data[2]);
	sstrcpy(study->patientBirthDate, data[3]);
	sstrcpy(study->accessionNumber, data[4]);
	sstrcpy(study->studyID, data[5]);
	sstrcpy(study->studyDate, data[6]);
	sstrcpy(study->rootpath, data[7]);
	study->next_study = *studyList;
	study->valid = true;
	*studyList = study;
	SSC_LOG("Got from cache %s (%s)", study->patientName, study->rootpath);
	return 0;
}

static int dbstrcallback(void *userdata, int cols, char **data, char **colnames)
{
	(void)cols;
	(void)colnames;
	strcpy((char *)userdata, data[0]);
	return 0;
}

static bool dbquery(sqlite3 *db, const char *statement, int (*callback)(void*,int,char**,char**), void *cbdata)
{
	int rc;
	char *zErrMsg = NULL;

	rc = sqlite3_exec(db, statement, callback, cbdata, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		SSC_ERROR("SQL error from %s: %s", statement, zErrMsg);
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

/**  Scan path for regular files, and put them into a linked list */
static int scan_dir( struct filenode **first, const char *path, struct timeval *last, int *progress, progress_func_t *callback, int *depth, sqlite3 *db )
{
	DIR *pdir;
	struct dirent *dp;
	struct stat sb;
	int dirno;
	static char rbuf[PATH_MAX];
	static char cacheduid[PATH_MAX];
	struct filenode *node;

	if ( *depth > 20 )
	{
		// Ooops. Too deep.
		SSC_LOG( "Too deep, restricting scan at %s.", path );
		return 0;
	}
	pdir = opendir( path );
	if (!pdir && errno == ENOTDIR && stat(path, &sb) == 0 && S_ISREG(sb.st_mode) && *first == NULL)
	{
		// Single file given as input
		*first = (struct filenode *)malloc( sizeof( **first ) );

		sstrcpy( (*first)->path, path );
		(*first)->next = NULL;
		closedir( pdir );
		return 0;
	}
	else if (!pdir)
	{
		SSC_LOG( "Could not open directory \"%s\": %s.", path, strerror( errno ) );
		closedir( pdir );
		return 0;
	}
	if (fchdir(dirfd(pdir)) == -1)
	{
		SSC_LOG( "Could not change to directory \"%s\": %s.", path, strerror( errno ) );
		closedir( pdir );
		return 0;
	}
	if (db && path[0] != '/')	// check if dir is part of cached study
	{
		getcwd(cacheduid, sizeof(cacheduid));
		sprintf(rbuf, "SELECT studyuid FROM studies WHERE path=\"%s\"", cacheduid);
		cacheduid[0] = '\0';
		if (dbquery(db, rbuf, dbstrcallback, cacheduid))
		{
			if (cacheduid[0] != '\0')
			{
				SSC_LOG("Directory \"%s\" is part of cached study %s", path, cacheduid);
				closedir(pdir);
				return 0;
			}
		}
	}
	SSC_LOG( "Searching path \"%s\".", path );
	dirno = dirfd( pdir );
	while ( ( dp = readdir( pdir ) ) != NULL )
	{
		unsigned char dtype = dp->d_type;

		if (dtype == DT_UNKNOWN)	// fs bug for cd and dvd
		{
			if (stat(dp->d_name, &sb) == 0)
			{
				if (S_ISDIR(sb.st_mode))
				{
					dtype = DT_DIR;
				}
				else if (S_ISREG(sb.st_mode))
				{
					dtype = DT_REG;
				}
				else
				{
					SSC_LOG("[%s] is unknown and not dir, type %d - hence file", dp->d_name, (int)sb.st_mode);
					dtype = DT_REG;
				}
			}
			else
			{
				SSC_LOG("Failed to stat %s: %s", dp->d_name, strerror(errno));
			}
		}
		if (dtype == DT_REG)
		{
			if (strcasecmp(dp->d_name, "DICOMDIR") == 0)
			{
				// It is a DICOMDIR file. We do not want to parse it as DICOM here.
				continue;
			}
			/* Create new node */
			node = (struct filenode *)malloc(sizeof(*node));
			node->next = *first;
			*first = node;

			/* Get full path */
			getcwd( node->path, DICOMLIB_VAL_MAX );
			sstrcat( node->path, "/" );
			sstrcat( node->path, dp->d_name );
		}
		else if (dtype == DT_DIR)
		{
			if (dp->d_name[0] == '.')
			{
				/* Either ., .. or hidden directory (Unix). Do nothing! */
				continue;
			}
			if (maybeCallback(last, callback, progress, 0, SLICE_COUNT_FILES) != 0)
			{
				closedir(pdir);
				return -1; /* abort - user cancelled */
			}
			/* We can recurse this directory */
			(*depth)++;
			if ( scan_dir( first, dp->d_name, last, progress, callback, depth, db ) != 0 )
			{
				closedir( pdir );
				return -1; /* abort */
			}
			(*depth)--;
			fchdir( dirno ); /* reset to this directory */
		}
		// else ignore it
	}

	(void) closedir( pdir );
	return 0;
}

struct study_t *DICOMLib_StudiesFromDICOMDIR( const char *path )
{
	struct study_t *studyList = NULL;

	if ( DICOM_Dir( path, &studyList ) != 0 )
	{
		SSC_LOG( "Failed - no valid DICOMDIR found." );
		return NULL;
	}

	return studyList;
}

/// Look for duplicate by SOPInstanceUID
static bool hasDuplicate( struct study_t *root, struct instance_t *instance )
{
	struct study_t *istudy;		// i = iter...
	struct series_t *iseries;
	struct instance_t *iinstance;

	if ( !root || !instance )
	{
		return true;	// should never happen
	}
	if ( instance->SOPInstanceUID[0] == '\0' || instance->parent_series->frames > (int)instance->frame )
	{
		return false;	// nothing to match against
	}
	for ( istudy = root; istudy; istudy = istudy->next_study )
	{
		for ( iseries = istudy->first_series; iseries; iseries = iseries->next_series )
		{
			for ( iinstance = iseries->first_instance; iinstance; iinstance = iinstance->next_instance )
			{
				if ( iinstance->SOPInstanceUID[0] != '\0' && strcmp(iinstance->SOPInstanceUID, instance->SOPInstanceUID) == 0 )
				{
					return true;	// these are by definition identical
				}
			}
		}
	}

	return false;
}

/// Convert list of files to list of studies by identifying files
static struct study_t *studiesFromNodes( struct study_t *root, struct filenode *node, progress_func_t *callback, int progress, bool keepRoot,
					 int flags, const char *rootdir )
{
	struct study_t *study = root;
	struct filenode *next;
	bool cancel = false;
	int recount, count;
	struct timeval last, now;

	(void) flags; // to keep compiler happy
	if ( !study )	// root may be NULL
	{
		study = (struct study_t *)calloc( 1, sizeof( *study ) );
		study->studyID[0] = '\0';
		study->next_study = NULL;
	}

	// count nodes
	for ( next = node, count = 0; next != NULL; next = next->next, count++ );

	gettimeofday( &last, NULL );
	now = last;

	/* Read as many as we should */
	for ( next = node, recount = 0; next != NULL; node = next, recount++ )
	{
		struct study_t temp_study;
		struct series_t temp_s;
		struct instance_t temp_i;
		int status = -1;
		char *tmp;

		memset( &temp_study, 0, sizeof( temp_study ) );
		memset( &temp_s, 0, sizeof( temp_s ) );
		memset( &temp_i, 0, sizeof( temp_i ) );
		temp_study.first_series = NULL;
		temp_study.next_study = NULL;
		temp_s.first_instance = NULL;
		temp_s.next_series = NULL;
		temp_i.next_instance = NULL;
		temp_i.frame = 0;
		temp_s.valid = false;
		temp_s.parent_study = &temp_study;
		temp_s.VOI.lut.table = NULL;
		temp_i.parent_series = &temp_s;
		sstrcpy( temp_s.patient_orientation[0], "R" ); /* for ACR-NEMA, this means head first... */
		sstrcpy( temp_s.patient_orientation[1], "P" ); /* on his back into the gantry of the system */
		sstrcpy( temp_i.path, node->path );
		sstrcpy( temp_s.splitReason, "(n/a)" );

		// Find root directory for study. Add just one directory in addition to the root directory for file scan.
		sstrcpy(temp_study.resource, node->path);
		tmp = temp_study.resource + strlen(rootdir);
		if (rootdir[strlen(rootdir)] == '/') tmp--;
		if (tmp && tmp[0] == '/' && strchr(tmp + 1, '/'))
		{
			tmp = strchr(tmp + 1, '/');
		}
		if (tmp) *tmp = '\0';

		/* We can check this file. Unless we are reading from a DICOMDIR, then skip identify to go faster. */
		if ( !cancel && ((study->resource[0] != '\0' && study->port == -1) || identify( node->path )) )
		{
			int curFrame = 0;
			DcmFileFormat dfile;
			OFCondition cond;
			DcmItem *dset = NULL;

			cond = dfile.loadFile( temp_i.path, EXS_Unknown, EGL_noChange, DCM_MaxReadLength, ERM_autoDetect );
			if ( cond.good() )
			{
				dset = dfile.getDataset();
				do
				{
					temp_i.frame = curFrame;
					status = parse_DICOM( dset, &temp_study, &temp_s, &temp_i, curFrame );
					if (!hasDuplicate(study, &temp_i))
					{
						temp_s.valid = (status == 0);
						if ( keepRoot )
						{
							insert_into_studylist( study, root, &temp_s, &temp_i ); // be sure we do not create another study
						}
						else
						{
							insert_into_studylist( study, &temp_study, &temp_s, &temp_i );
						}
					}
					curFrame++;
				}
				while (temp_s.frames > curFrame);
			}
			else
			{
				SSC_LOG( "Could not load file: %s", temp_i.path );
			}
		}
		freeSeries( &temp_s );

		progress = MAX( progress, (double)recount / (double)count * SLICE_PARSE_FILES + SLICE_COUNT_FILES );
		if ( !cancel && callback )
		{
			struct timeval now, diff;

			gettimeofday( &now, NULL );
			timersub( &now, &last, &diff );
			if ( diff.tv_usec > CALLBACK_TIMESLICE )
			{
				last = now;
				if ( callback( progress ) != 0 )
				{
					cancel = true;
					SSC_LOG( "User cancelled" );
				}
			}
		}

		next = node->next;
		free( node );
	}
	//SSC_LOG( "ended on progress: %d", progress );

	if ( study->studyID[0] == '\0' || cancel )
	{
		if (!root)
		{
			DICOMLib_CloseStudies( study );
		}
		// No studies found
		return NULL;
	}

	return study;
}

static struct study_t *studiesFromPath( const char *path, progress_func_t *callback, bool keepRoot, struct study_t *root, int flags, bool ignoreCache )
{
	struct study_t *studyList = NULL, *next;
	DIR *wdir = opendir( "." ); // we need to return to this directory
	struct filenode *node = NULL;
	int progress = 1, depth = 0, retval;
	struct timeval last, now;
	sqlite3 *db = NULL;
	struct stat buf;
	bool exists = (stat(dbname, &buf) == 0);
	int rc = SQLITE_OK;
	char tmp[PATH_MAX], *s;

	if (!(flags & DICOMLIB_NO_CACHE))	// Initialize db cache
	{
		rc = sqlite3_open(dbname, &db);
	}
	if (rc != SQLITE_OK)
	{
		SSC_ERROR("Failed to open db %s: %s", dbname, sqlite3_errmsg(db));
		db = NULL;
	}
	if (!exists && db)	// create database if it does not exist
	{
		char *zErrMsg = NULL;
		rc = sqlite3_exec(db, sqlinit, NULL, NULL, &zErrMsg);
		exists = (rc == SQLITE_OK);
		if (rc != SQLITE_OK)
		{
			SSC_ERROR("SQL error: %s", zErrMsg);
			sqlite3_free(zErrMsg);
			db = NULL;
		}
	}

	gettimeofday( &last, NULL );
	now = last;

	// Note that we do not permit a search to scan the entire root system of the disk. This
	// is usually a terminal error.
	if ( !path || strlen( path ) == 0 || strcmp( path, "" ) == 0 || strcmp( path, "/" ) == 0 )
	{
		SSC_LOG( "Empty parameter (%s) - invalid", path ? path : "NULL" );
		if (db)
		{
			sqlite3_close(db);
		}
		errno = EINVAL;
		closedir( wdir );
		return NULL;
	}

	SSC_LOG( "called on %s", path );

	// First read filenames of all files in the directory and its subdirectories, this
	// way we can update a progress indicator through the callback properly.
	retval = scan_dir( &node, path, &last, &progress, callback, &depth, db );
	fchdir( dirfd( wdir ) ); // reset working directory
	closedir( wdir );
	if ( retval != 0 )
	{
		SSC_LOG( "Aborted while looking for files" );
		if (db)
		{
			sqlite3_close(db);
		}
		errno = ECANCELED;
		return NULL;
	}
	else if ( node )
	{
		studyList = studiesFromNodes( root, node, callback, progress, keepRoot, flags, path );
	}

	// Find longest shared path for studies, for cache
	for ( struct study_t *study = studyList; study; study = study->next_study )
	{
		for (struct series_t *iter = study->first_series; iter != NULL; iter = iter->next_series)
		{
			if (iter->first_instance)
			{
				sstrcpy(iter->rootpath, iter->first_instance->path);
				s = strrchr(iter->rootpath, '/');
				if (s) *s = '\0'; // drop filename from path
			}
			for (struct instance_t *iter_instance = iter->first_instance; iter_instance != NULL; iter_instance = iter_instance->next_instance)
			{
				sstrcpy(tmp, iter_instance->path);
				s = strrchr(tmp, '/');
				if (s) *s = '\0'; // drop filename from path
				longestSharedPath(tmp, iter->rootpath);
			}
		}
		// Find longest possible shared path for series in study
		if (study->first_series) sstrcpy(study->rootpath, study->first_series->rootpath);
		for (struct series_t *iter = study->first_series; iter != NULL; iter = iter->next_series)
		{
			longestSharedPath(iter->rootpath, study->rootpath);
		}
	}

	// Add to cache to speed up reading next time; fetch existing entries from the cache
	if (db && !ignoreCache && !(flags & DICOMLIB_MEDIA_DIR))
	{
		char rbuf[PATH_MAX];
		timeval tv;
		struct study_t *start = studyList;
		// Add cached entries

		sstrcpy(rbuf, "SELECT studyuid, patientsname, patientid, birthdate, accessionnumber, studyid, studydate, path FROM studies");
		if (!dbquery(db, rbuf, dbstudycallback, &studyList))
		{
			SSC_ERROR("query \"%s\" failed", rbuf);
		}
		// See if we should add new entries to the cache (start after above added entries, which were added at beginning of the list)
		gettimeofday(&tv, NULL);
		for (next = start; next; next = next->next_study)
		{
			if (stat(next->resource, &buf) == 0 && S_ISDIR(buf.st_mode)
			    && tv.tv_sec > buf.st_mtime + 24 * 60 * 60	// must be older than 24 hours
			    && strcmp(path, next->rootpath) != 0)	// and deeper than root of path
			{
				sprintf(rbuf, "INSERT OR REPLACE INTO studies(studyuid, patientsname, patientid, birthdate, accessionnumber, "
					"studyid, studydate, path) values (\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\")",
					next->studyInstanceUID, next->patientName, next->patientID, next->patientBirthDate, next->accessionNumber,
					next->studyID, next->studyDate, next->rootpath);
				dbquery(db, rbuf, NULL, NULL);
			}
		}
	}
	if (db)
	{
		sqlite3_close(db);
	}

	/* Set indexing values */
	retval = 0;
	// find highest studyuid in existing study
	for (next = root; next; next = next->next_study) { if ((int)next->study_id > retval) retval = next->study_id; }
	index_objects( studyList, retval, flags & DICOMLIB_REINDEX_STUDY );

	return studyList;
}

struct study_t *DICOMLib_StudiesFromPath( const char *path, progress_func_t *callback, int flags )
{
	return studiesFromPath( path, callback, false, NULL, flags, false );
}

int DICOMLib_Image_RGB_Fill( const struct series_t *series, int sizeX, int sizeY, int frame, char *image )
{
	struct instance_t *instance;
	int x = sizeX, y = sizeY, diffX, diffY, i, j;
	char *buffer;
	float targetfx = sizeX, targetfy = sizeY, sourcefx, sourcefy;

	if ( !series || !image || frame < 0 || sizeX < 0 || sizeY < 0 || !series->first_instance )
	{
		SSC_LOG( "Bad parameter" );
		return EINVAL;
	}
	instance = series->first_instance;
	sourcefx = series->columns;
	sourcefy = series->rows;

	if ( series->samples_per_pixel == 2 || series->samples_per_pixel == 4 )
	{
		SSC_LOG( "Bad samples per pixel setting %d", (int)series->samples_per_pixel );
		return EINVAL;
	}
	memset( image, 0, sizeX * sizeY * 3 );

	// Find the appropriate instance to match the frame
	for ( i = 0; i < frame && instance != NULL; i++, instance = instance->next_instance );
	if ( instance == NULL )
	{
		SSC_LOG( "Could not find expected frame (looking for %d, had only %d instances)", frame, series->frames );
		return EFAULT;
	}

	// Check if we can fit in Y direction if aspect ratio clamped in X direction
	if ( targetfy >= ( targetfx / sourcefx ) * sourcefy )
	{
		y = 0;
		x = sizeX;
	}
	else if ( targetfx >= ( targetfy / sourcefy ) * sourcefx )
	{
		x = 0;
		y = sizeY;
	}
	else
	{
		SSC_LOG( "Could not find aspect ratio: %f %f, %f %f",
		        targetfx, targetfy, sourcefx, sourcefy );
		image = NULL;
		return -1;
	}
	buffer = (char *)DICOM_image_scaled( instance, &x, &y, 8, instance->frame ); /* rescale to fit given parameters */
	if ( !buffer )
	{
		return errno;	// errno set in DCMTK.cpp
	}
	diffX = (sizeX - x) / 2;
	diffY = (sizeY - y) / 2;
	for ( i = 0; i < x; i++)
	{
		for ( j = 0; j < y; j++ )
		{
			if ( series->samples_per_pixel == 1 )
			{
				const char value = buffer[j * x + i];
				const int posX = i + diffX;
				const int posY = j + diffY;
				char *imptr = &image[(posY * sizeX + posX) * 3];

				*imptr++ = value;
				*imptr++ = value;
				*imptr++ = value;
			}
			else if ( series->samples_per_pixel == 3 )
			{
				const char *valptr = &buffer[(j * x + i) * series->samples_per_pixel];
				const int posX = i + diffX;
				const int posY = j + diffY;
				char *imptr = &image[(posY * sizeX + posX) * 3];

				*imptr++ = *valptr++;
				*imptr++ = *valptr++;
				*imptr++ = *valptr++;
			}
		}
	}
	free(buffer);
	return 0;
}

char *DICOMLib_Image( const struct series_t *series, int *sizeX, int *sizeY, int bits_per_sample, int frame )
{
	int i;
	char *buffer = NULL;

	if ( !series || !series->first_instance || !sizeX || !sizeY )
	{
		SSC_LOG( "Bad parameter" );
		errno = EINVAL;
		return NULL;
	}
	struct instance_t *instance = series->first_instance;
	if (*sizeX <= 0) *sizeX = series->columns;
	if (*sizeY <= 0) *sizeY = series->rows;

	// Find the appropriate instance for the given frame
	for ( i = 0; i < frame && instance != NULL; i++, instance = instance->next_instance );
	if ( instance == NULL )
	{
		SSC_LOG( "Could not find expected frame" );
		errno = EFAULT;
		return NULL;
	}

	buffer = (char *)DICOM_image_scaled( instance, sizeX, sizeY, bits_per_sample, instance->frame );
	if ( !buffer )
	{
		errno = EFAULT;
		return NULL;
	}

	return buffer;
}

/** Will delete non-valid slices from the list. Report deletions in the series_info field. */
#define REPORT(...) do { \
char s[500]; int left = DICOMLIB_INFO_MAX - strlen(series->series_info) - 1; \
left = left >= 0 ? left : 0; /* no negative numbers here!! */ \
snprintf( s, 500-1, __VA_ARGS__ ); SSC_LOG( "DICOMLib_Verify: %s", s ); \
strncat( series->series_info, s, left ); strncat( series->series_info, "\n", left ); sstrcpy( series->error, s ); \
} while(0)
int DICOMLib_Verify( struct series_t *series )
{
	struct instance_t *instance = NULL, *last = NULL;
	int count;
	double dir[3];
	double avgDist = 0;
	int minimum = MAX(config[DICOMLIB_CONF_MIN_NUM_SLICES], 3);

	if ( !series || !series->valid )
	{
		return 0; // avoid error spam
	}

	for ( count = 1, instance = series->first_instance->next_instance; instance != NULL; count++, instance = instance->next_instance );
	if ( series->frames != count )
	{
		SSC_LOG( "Number of frames reported (%d) does not match number found (%d)!", series->frames, count );
		series->frames = MIN(count, series->frames);
	}
	// check if there are enough slices except for DTI data (hack for DTI-mosaic from siemens)
	if ( (count < minimum || count < 3) && !series->DTI.isDTI )
	{
		REPORT( "Series does not have enough slices. %d required, %d found.", MAX( 3, minimum ), count );
		series->valid = FALSE;
		return 0;
	}

	/*** Run through all slices and check them for individual validity. ***/
	struct instance_t *previous = series->first_instance;
	for ( instance = series->first_instance->next_instance; instance != NULL; previous = instance, instance = instance->next_instance )
	{
		// Calculate direction vector (diff between 2 image positions, mm units)
		vector3d_subtract( dir, instance->image_position, previous->image_position );

		// check if we have valid positions in the files
		if ( fabs( dir[0] ) < config[DICOMLIB_CONF_MIN_DIST_SLICES]
		     && fabs( dir[1] ) < config[DICOMLIB_CONF_MIN_DIST_SLICES]
		     && fabs( dir[2] ) < config[DICOMLIB_CONF_MIN_DIST_SLICES] )
		{
			REPORT( "Series has image slices too close to each other. Minimum distance is %f, found (%f, %f, %f).",
			        config[DICOMLIB_CONF_MIN_DIST_SLICES], fabs( dir[0] ), fabs( dir[1] ), fabs( dir[2] ) );
			series->valid = FALSE;
			return 0;
		}
		last = instance;
	}

	/* Check if the slices are too far apart */
	previous = series->first_instance;
	vector3d_subtract(dir, last->image_position, series->first_instance->image_position);
	avgDist = dir[2] / count;	// average distance between slices is our z direction
	for ( instance = series->first_instance->next_instance; instance != NULL; previous = instance, instance = instance->next_instance )
	{
		// Calculate direction vector (diff between 2 image positions, mm units)
		vector3d_subtract( dir, instance->image_position, previous->image_position );

		// Check if we have valid spacing between slices
		if ( fabs(dir[2]) > fabs(avgDist * config[DICOMLIB_CONF_MAX_GAP]) )
		{
			REPORT( "Series volume has missing frames. Average distance is %f, found space of %f, limit is %f in instance %s" ,
				avgDist, dir[2], config[DICOMLIB_CONF_MAX_GAP], instance->path );
			series->valid = FALSE;
			return 0;
		}
	}

	return 0;
}
#undef REPORT

int DICOMLib_SetWindow( struct series_t *series, double center, double width )
{
	if ( !series )
	{
		return EINVAL;
	}
	series->VOI.current.width = width;
	series->VOI.current.center = center;
	series->VOI.currentPreset = -1;

	return 0;
}

int DICOMLib_UsePreset( struct series_t *series, int index )
{
	if ( !series || index >= series->VOI.numPresets || index < 0 )
	{
		SSC_LOG( "Bad parameter (%p, %d)", series, index );
		return EINVAL;
	}
	series->VOI.currentPreset = index;
	series->VOI.current = series->VOI.preset[index].window;
	return 0;
}

int DICOMLib_UseAutoVOI( struct series_t *series )
{
	if ( !series )
	{
		SSC_LOG( "Bad parameter" );
		return EINVAL;
	}
	series->VOI.currentPreset = -1;
	series->VOI.current = series->VOI.suggestion;
	SSC_LOG( "Setting VOI to suggestion (%f, %f)", series->VOI.suggestion.center, series->VOI.suggestion.width );
	return 0;
}

int DICOMLib_NoVOI( struct series_t *series )
{
	if ( !series )
	{
		return EINVAL;
	}
	series->VOI.currentPreset = -1;
	series->VOI.current.width = 0;
	series->VOI.current.center = 1;
	return 0;
}

int DICOMLib_GetWindow( struct series_t *series, double *center, double *width )
{
	if ( !series || !center || !width )
	{
		return EINVAL;
	}
	*center = series->VOI.current.center;
	*width = series->VOI.current.width;
	return 0;
}

static int pruneEmpty(DIR *dir)
{
	int count = 0;
	struct dirent *dp;
	while (dir && (dp = readdir(dir)))
	{
		if (dp->d_type == DT_DIR && dp->d_name[0] != '.')
		{
			DIR *newDir = opendir(dp->d_name);
			int contents = pruneEmpty(newDir);
			closedir(newDir);
			if (contents == 0)	// empty? - nuke it!
			{
				char path[PATH_MAX];
				realpath(dp->d_name, path);
				rmdir(path);
			}
		}
		count++;
	}
	return count;
}

int DICOMLib_DeleteStudy( struct study_t *study )
{
	char rbuf[PATH_MAX];
	sqlite3 *db = NULL;
	int rc = sqlite3_open(dbname, &db);

	if (!study)
	{
		return 0;
	}

	if (rc != SQLITE_OK)
	{
		SSC_ERROR("Failed to open db %s: %s", dbname, sqlite3_errmsg(db));
		db = NULL;
	}

	// Delete known files
	struct series_t *series = DICOMLib_GetSeries(study, NULL);
	if (series)
	{
		for (struct series_t *next = series; next; next = next->next_series)
		{
			DICOMLib_DeleteSeries(next);	// node is just marked invalid, no dangling pointers here
		}
		if (study->resource[0] != '\0' && study->port == -1)
		{
			// delete referenced DICOMDIR; we may not be the only entry in the DICOMDIR, but this is the only safe way
			SSC_LOG("Deleting %s", study->resource);
			unlink(study->resource);
		}
		sprintf(rbuf, "DELETE FROM studies WHERE studyuid=\"%s\"", study->studyInstanceUID);
		dbquery(db, rbuf, NULL, NULL);
		study->valid = false;
	}
	// Prune empty directories
	DIR *dir = opendir("/Data/DICOM/");
	pruneEmpty(dir);
	closedir(dir);

	// Prune from the cache
	ssprintf(rbuf, "DELETE FROM studies WHERE path=\"%s\"", study->rootpath);
	SSC_LOG("Executing sql statement for deletion: %s", rbuf);
	if (!dbquery(db, rbuf, NULL, db))
	{
		SSC_ERROR("query \"%s\" failed", rbuf);
	}
	sqlite3_close(db);
	return 0;
}

static int deleteInstances( struct series_t *series )
{
	struct instance_t *instance;

	/* Run through all slices and delete them. */
	for ( instance = series->first_instance; instance != NULL; instance = instance->next_instance )
	{
		if ( instance->path == NULL )
		{
			SSC_LOG( "Filename is NULL!" );
			return -1;
		}

		/* Delete file. */
		unlink( instance->path );
	}

	return 0;
}

int DICOMLib_DeleteSeries( struct series_t *series )
{
	if ( series == NULL )
	{
		SSC_LOG( "Bad parameter" );
		return EINVAL;
	}

	deleteInstances( series );

	/* We do not even bother to unlink the nodes. We do that on cleanup. This way all counts
	 * remain valid. */
	series->valid = false;

	return 0;
}

double DICOMLib_GetConfig( enum dicomlib_config_type c )
{
	if ( c < DICOMLIB_CONF_LAST )
	{
		return config[c];
	}
	return NAN;
}

int DICOMLib_Config( enum dicomlib_config_type c, double val )
{
	if ( c < DICOMLIB_CONF_LAST )
	{
		config[c] = val;
		SSC_LOG( "changing setting %d to %f", (int)c, val );
		return 0;
	}
	return EINVAL;
}

struct series_t *DICOMLib_GetSeries( struct study_t *study, progress_func_t *callback )
{
	if ( !study )
	{
		SSC_LOG( "Invalid parameter" );
		errno = EINVAL;
		return NULL;
	}
	if ( !study->first_series )
	{
		if ( study->port > 0 && study->resource[0] == '\0' )
		{
			// Fetch from PACS
			SSC_LOG( "Get series for study id %d uid %s from PACS", (int)study->study_id, study->studyInstanceUID );
			if ( DICOMLib_FetchStudy( study->studyInstanceUID ) != 0 )
			{
				SSC_ERROR( "Failed to fetch study %s from PACS", study->studyInstanceUID );
				study->first_series = (struct series_t *)calloc(1, sizeof(*study->first_series));
				sstrcpy(study->first_series->series_info, "Failed to retrieve series from PACS");
				sstrcpy(study->first_series->seriesDescription, "Network failure");
				sstrcpy(study->first_series->seriesID, "N/A");
				study->first_series->next_series = NULL;
				study->first_series->valid = false;
				return study->first_series;	// return dummy error series... no other way to report failure atm :/
			}
			// Now wait patiently to receive images
			return NULL;
		}
		else if ( study->port == 0 && study->rootpath[0] != '\0' )
		{
			// Read PACS results or cached series
			SSC_LOG( "Get series from resource path %s", study->rootpath );
			studiesFromPath( study->rootpath, callback, true, study, 0, true );
		}
		else if ( study->port == -1 )
		{
			// DICOMDIR
			SSC_LOG( "Get series from DICOMDIR" );
			if (!readSeriesFromDICOMDIR( study, callback ))
			{
				SSC_LOG("Cancelled");
				return NULL;
			}
		}
		else
		{
			// Nothing
			SSC_LOG( "Had no series?!" );
			errno = -1;
			return NULL;
		}

	}
	setupSeries( study, callback );
	return study->first_series;
}

// Inspired by http://public.kitware.com/pipermail/insight-users/2005-March/012246.html
char DICOMLib_DescribeOrientation( const struct series_t *series, int axis, bool inverse )
{
	// Use either Y or X axis, respectively, of volume depending on axis orientation
	const double *iop = axis ? &series->image_orientation[3] : &series->image_orientation[0];
	const double factor = inverse ? -1.0 : 1.0;	// Inversion factor
	const double threshold = sqrt(1.0 / 2.0);	// Check whether is within 45 degrees of orientation
	const char orientationX = iop[0] * factor < 0 ? 'R' : 'L';
	const char orientationY = iop[1] * factor < 0 ? 'A' : 'P';
	const char orientationZ = iop[2] * factor < 0 ? 'I' : 'S';
	const double absX = fabs(iop[0] * factor);
	const double absY = fabs(iop[1] * factor);
	const double absZ = fabs(iop[2] * factor);

	if ( absX > threshold && absX > absY && absX > absZ )
	{
		return orientationX;
	}
	else if ( absY > threshold && absY > absX && absY > absZ )
	{
		return orientationY;
	}
	else if ( absZ > threshold && absZ > absX && absZ > absY )
	{
		return orientationZ;
	}
	return '?';
}

/**
 * This function is required to pass 100% coverage testing of extremely oddball cases that
 * cannot happen when just exercising the exposed API. These calls will all fail. The 
 * important check is that they do not crash and that all code lines are covered.
 */
bool DICOMLib_INTERNAL_TEST()
{
	readSeriesFromDICOMDIR( NULL, NULL );
	insert_into_studylist( NULL, NULL, NULL, NULL );
	DICOMLib_Verify( NULL );
	DICOMLib_SetWindow( NULL, 0.0, 0.0 );
	DICOMLib_UsePreset( NULL, 0 );
	DICOMLib_GetWindow( NULL, NULL, NULL );
	DICOMLib_DeleteStudy( NULL );
	DICOMLib_DeleteSeries( NULL );
	DICOMLib_Config( DICOMLIB_CONF_LAST, 0.0 );
	hasDuplicate( NULL, NULL );
	return true;
}
