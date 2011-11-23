/*
 * igtl_sonix_status.h
 *
 *  Created on: Nov 23, 2011
 *      Author: olevs
 */

#ifndef IGTL_SONIX_STATUS_H_
#define IGTL_SONIX_STATUS_H_

#include "igtl_win32header.h"
#include "igtl_types.h"
#include "igtl_win32header.h"

#define IGTL_SONIX_STATUS_HEADER_SIZE         25

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

typedef struct {
	igtl_float64 oi;//Origin
	igtl_float64 oj;//Origin
	igtl_float64 ok;//Origin
	igtl_uint8 status;// 0 = not changed, 1 = changed
} igtl_sonix_status_message;

#pragma pack()

/*
 * String data size
 *
 * This function calculates size of the pixel array, which will be
 * transferred with the specified header.
 */

//igtl_uint32 igtl_export igtl_sonix_status_get_length(igtl_sonix_status_header * header);

/*
 * Byte order conversion
 *
 * This function converts endianness from host byte order to network byte order,
 * or vice versa.
 * NOTE: It is developer's responsibility to have the string body with BOM
 * (byte order mark) or in big endian ordrer.
 */

void igtl_export igtl_sonix_status_convert_byte_order(igtl_sonix_status_message * message);

/*
 * CRC calculation
 *
 * This function calculates CRC of image data body including header
 * and array of pixel data.
 *
 */

igtl_uint64 igtl_export igtl_sonix_status_get_crc(igtl_sonix_status_message * message);

#ifdef __cplusplus
}
#endif

#endif /* IGTL_SONIX_STATUS_H_ */
