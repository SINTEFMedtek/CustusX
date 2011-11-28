/*
 * igtl_sonix_status.c
 *
 *  Created on: Nov 23, 2011
 *      Author: olevs
 */

#include "igtl_sonix_status.h"
#include "igtl_util.h"

void igtl_export igtl_sonix_status_convert_byte_order(igtl_sonix_status_message * message)
{
	igtl_uint64* tmp = (igtl_uint64*)message;
	igtl_uint32* tmp32 = (igtl_uint32*)message;

	if (igtl_is_little_endian())
	{
		int i;
		for (i = 0; i < 3; i++)//Update number when adding more variables to message
			tmp[i] = BYTE_SWAP_INT64(tmp[i]);
	}
	
	tmp32[12] = BYTE_SWAP_INT32(tmp32[12]);
}

igtl_uint64 igtl_export igtl_sonix_status_get_crc(igtl_sonix_status_message * message)
{
	igtl_uint64 crc = crc64(0, 0, 0);

	crc = crc64((unsigned char*)message, IGTL_SONIX_STATUS_HEADER_SIZE, crc);
	return crc;
}
