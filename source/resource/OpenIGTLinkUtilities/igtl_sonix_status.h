/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#ifndef IGTL_SONIX_STATUS_H_
#define IGTL_SONIX_STATUS_H_

#include "igtl_win32header.h"
#include "igtl_types.h"
#include "igtl_win32header.h"

#define IGTL_SONIX_STATUS_HEADER_SIZE        70 // 8*3 + 4*3 + 4*8 + 2

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

typedef struct {
	igtl_float64 spacingX;
	igtl_float64 spacingY;
	igtl_float64 spacingZ;
	igtl_float32 oi;//Origin
	igtl_float32 oj;//Origin
	igtl_float32 ok;//Origin
	igtl_int32 ulx;
	igtl_int32 uly;
	igtl_int32 urx;
	igtl_int32 ury;
	igtl_int32 brx;
	igtl_int32 bry;
	igtl_int32 blx;
	igtl_int32 bly;
	igtl_uint16 status;// 0 = not changed, 1 = changed
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
