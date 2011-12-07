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
	igtl_int64* tmp64 = (igtl_int64*)message;
	igtl_int32* tmp32 = (igtl_int32*)message;
	igtl_uint16* tmp16 = (igtl_uint16*)message;

	if (igtl_is_little_endian())
	{
		int i;
		for (i = 0; i < 3; i++)//Update number when adding more variables to message
			tmp64[i] = BYTE_SWAP_INT64(tmp64[i]);

		for (i = 0; i < 11; i ++)
			tmp32[6 + i] = BYTE_SWAP_INT32(tmp32[6 + i]);//3*64 bit adressed as 32 bit = 6

		tmp16[34] = BYTE_SWAP_INT16(tmp16[34]);//3*64 bit + 8*32 bit (adressed as 16 bit = 28)
	}
	
}

igtl_uint64 igtl_export igtl_sonix_status_get_crc(igtl_sonix_status_message * message)
{
	igtl_uint64 crc = crc64(0, 0, 0);

	crc = crc64((unsigned char*)message, IGTL_SONIX_STATUS_HEADER_SIZE, crc);
	return crc;
}
