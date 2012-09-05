

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <QMap>
#include <QString>
#include "sscLogger.h"
#include "DICOMLib.h"
#include "nifti1.h"
#include "nifti1_io.h"
#include "vector3d.h"
#include "DCMTK.h"

using namespace std;

template <typename T>
std::string to_string(T const& value) {
    stringstream sstr;
    sstr << value;
    return sstr.str();
}

int DICOMLib_WriteNifti( const char *dir, const struct study_t *study )
{
	struct series_t *s = NULL;
	QMap<QString, uint32_t> uid;
	int retval = 0;

	// find DTI-volumes by seriesInstanceUID
	for (s = study->first_series; s; s = s->next_series)
	{
	//	if (s->DTI.isDTI)
		{
			uid.insertMulti(s->seriesInstanceUID, s->series_id);
		}
	}
	QList<QString> uidList = uid.uniqueKeys();
	for (int i = 0; i < uidList.size(); i++) // for each found relevant UID
	{
		// bvec components for collect bvec data and bvalues
		string bvecX="";
		string bvecY="";
		string bvecZ="";
		string bval="";

		QString usedUid = uidList[i];
		znzFile outfp = NULL;
		nifti_image hdr;
		bool headerWritten = false;
		for (s = study->first_series; s; s = s->next_series)	// scan each series for actual processed UID
		{
			if (s->DTI.isDTI && usedUid.compare(s->seriesInstanceUID) == 0)
			{
				bvecX = bvecX + to_string( s->DTI.bvec[0] ) + ' ';
				bvecY = bvecY + to_string( s->DTI.bvec[1] ) + ' ';
				bvecZ = bvecZ + to_string( s->DTI.bvec[2] ) + ' ';
				bval = bval + s->DTI.bval + ' ';

				if (!headerWritten)
				{
					double image_pos_vector[3];
					double image_orientation_normal[3];
					struct instance_t *instance, *last;

					/* Grab first instance */
					instance = s->first_instance;

					/* Grab last instance */
					for ( last = s->first_instance; last->next_instance != NULL; last = last->next_instance );

					// Make header
					headerWritten = true;

					memset(&hdr, 0, sizeof(hdr));
					hdr.nx = s->columns;
					hdr.ny = s->rows;
					hdr.nz = s->frames; // number of slices per volume
					hdr.nt = uid.count(usedUid);
					hdr.nu = 1;
					hdr.ndim = 4;
					hdr.nv = 1;
					hdr.nw = 1;
					hdr.nvox = hdr.nx * hdr.ny * hdr.nz * hdr.nt * hdr.nu * hdr.nv * hdr.nw;
					hdr.datatype = (s->bits_per_sample == 16) ? DT_UINT16 : DT_UINT8;
					nifti_datatype_sizes(hdr.datatype, &(hdr.nbyper), &(hdr.swapsize));

					// Find image vector from first to last image through their image positions
					vector3d_subtract( image_pos_vector, last->image_position, instance->image_position );

					// Divide image_pos_vector by number of slices - 1
					vector3d_scalar_divide( image_pos_vector , s->frames - 1 );

					// find the image orientation in z direction
					vector3d_cross_product( image_orientation_normal, &s->image_orientation[0],
								&s->image_orientation[3] );
					s->slice_thickness = vector3d_inner_product(image_orientation_normal, image_pos_vector);

					hdr.dx = s->pixel_spacing[0];
					hdr.dy = s->pixel_spacing[1];
					hdr.dz = s->slice_thickness;
					if (s->repetitionTime[0] != '\0')
					{
						hdr.dt = atof(s->repetitionTime) / 1000.0;
					}
					else
					{
						hdr.dt = 0.0;
					}
					hdr.du = 1.0;
					hdr.dv = 1.0;
					hdr.dw = 1.0;

					hdr.scl_slope = 0;
					hdr.scl_inter = 0;

					hdr.cal_min = 0;
					hdr.cal_max = 0;

					hdr.qform_code = NIFTI_XFORM_SCANNER_ANAT;
					hdr.sform_code = 0;

					hdr.freq_dim  = 1; // not sure if this is always the case... FIXME
					hdr.phase_dim = 2; // not sure if this is always the case... FIXME
					hdr.slice_dim = 3;

					hdr.slice_code = 0;
					hdr.slice_start = 0;
					hdr.slice_end = hdr.nz - 1;
					hdr.slice_duration = 0.0;

					// Generate quaternion
					hdr.qto_xyz.m[0][0] = -s->image_orientation[0];
					hdr.qto_xyz.m[0][1] = -s->image_orientation[3];
					hdr.qto_xyz.m[0][2] = -(s->image_orientation[1] * s->image_orientation[5]
								- s->image_orientation[2] * s->image_orientation[4]);

					hdr.qto_xyz.m[1][0] = -s->image_orientation[1];
					hdr.qto_xyz.m[1][1] = -s->image_orientation[4];
					hdr.qto_xyz.m[1][2] = -(s->image_orientation[2] * s->image_orientation[3]
								- s->image_orientation[0] * s->image_orientation[5]);

					hdr.qto_xyz.m[2][0] = s->image_orientation[2];
					hdr.qto_xyz.m[2][1] = s->image_orientation[5];
					hdr.qto_xyz.m[2][2] = (s->image_orientation[0] * s->image_orientation[4]
							      - s->image_orientation[1] * s->image_orientation[3]);

					hdr.qoffset_x = -(s->first_instance->image_position[0]);
					hdr.qoffset_y = -(s->first_instance->image_position[1]);
					hdr.qoffset_z =  (s->first_instance->image_position[2]);

					nifti_mat44_to_quatern(hdr.qto_xyz, &(hdr.quatern_b), &(hdr.quatern_c), &(hdr.quatern_d),
							       NULL, NULL, NULL, NULL, NULL, NULL, &(hdr.qfac));
					hdr.toffset = 0.0;
					hdr.xyz_units = NIFTI_UNITS_MM;
					hdr.time_units = NIFTI_UNITS_MSEC;

					hdr.intent_code = NIFTI_INTENT_NONE;
					hdr.intent_p1 = 0.0;
					hdr.intent_p2 = 0.0;
					hdr.intent_p3 = 0.0;
					hdr.intent_name[0] = '\0';
					hdr.aux_file[0] = '\0';

					hdr.byteorder = nifti_short_order();
					hdr.data = NULL;
					memset(hdr.descrip, 0, sizeof(hdr.descrip));
					strncpy(hdr.descrip, "SonoWand nifti export", sizeof(hdr.descrip) - 1);
					hdr.nifti_type = 1; // single file

					char filename[PATH_MAX];
					ssprintf(filename, "%s/%s.nii", dir, usedUid.toStdString().c_str() );

					hdr.fname = strdup(filename);
					hdr.iname = NULL;

					outfp = nifti_image_write_hdr_img(&hdr, 2, "wb");
					if (outfp == NULL)
					{
						SSC_ERROR("Failed to write nifti header");
						return -1;
					}

				}

				// Export raw data
				instance_t *inst = s->first_instance;
				size_t size = hdr.nx * hdr.ny * hdr.nbyper;
				// SSC_LOG("STime:%s  Frames: %i / bvec: %f %f %f / bval: %s",s->seriesTime,s->frames, s->DTI.bvec[0], s->DTI.bvec[1], s->DTI.bvec[2], s->DTI.bval);
				for (int j = 0; j < s->frames; j++)
				{
					const void *data = DICOM_raw_image(s, inst, inst->frame);
					size_t result = nifti_write_buffer(outfp, data, size);
					if (result != size)
					{
						SSC_ERROR("Failed to write nifti data, frame %d", j);
					}
					inst = inst->next_instance;
				}
				retval++;
			}
		}
		znzclose(outfp);

		// Export bval / bvec data for actual processed UID
		if ( bval.size() > 19 ) // DTI data should contain several different diffusion bval
		{
			char bvalFilename[PATH_MAX];
			ofstream bvalStream;
			char bvecFilename[PATH_MAX];
			ofstream bvecStream;
			ssprintf(bvalFilename, "%s/%s.bval", dir, usedUid.toStdString().c_str() );
			ssprintf(bvecFilename, "%s/%s.bvec", dir, usedUid.toStdString().c_str());

			bvalStream.open(bvalFilename, ios::out);
			bvecStream.open(bvecFilename, ios::out);

			if ( bvalStream.is_open() )
			{
				bvalStream << bval;
				bvecStream << bvecX << endl;
				bvecStream << bvecY << endl;
				bvecStream << bvecZ;
			}

			bvalStream.close();
			bvecStream.close();
			bval.clear();
			bvecX.clear();
			bvecY.clear();
			bvecZ.clear();
			SSC_LOG( "Nifti data for %s written",usedUid.toStdString().c_str() );
		}
	}
	return retval;
}
