#include <assert.h>
#include "../identify.h"
#include "../convert.c"

static struct conv_global global;

void setRepresentation(void *buffer, int bytes, bool isSigned, int samples)
{
	global.dicomimage = buffer;
	global.srcbytes = bytes;
	global.dstbytes = bytes;
	global.slice_size = global.size_new[0] * global.size_new[1] * bytes * global.samples;
	global.encoding = bytes + isSigned * 4;
	global.samples = samples;
}

int main(void)
{
	int result, i;
	char sbuf8[8];
	short sbuf16[8];
	unsigned char ubuf8[8];
	unsigned short ubuf16[8];
	unsigned char ubuf888[8 * 8 * 3];
	unsigned char store[16];
	unsigned short store16[16];
	unsigned char store888[8 * 8 * 3];

	SSC_Logging_Init_Default( "com.sonowand.test" );
	result = DICOMLib_Init();
	assert( result == 0 );

	DICOMLib_INTERNAL_TEST();	// should not crash

	// Set test parameters
	global.index = 0;
	global.count = 1;
	global.size_old[0] = 8;
	global.size_old[1] = 1;
	global.size_old[2] = 1;
	global.size_new[0] = 8;
	global.size_new[1] = 1;
	global.size_new[2] = 1;
	global.samples = 1;
	for (i = 0; i < 8; i++)
	{
		sbuf8[i] = i * 8;
		sbuf16[i] = i * 32;
		ubuf8[i] = i * 16;
		ubuf16[i] = i * 64;
	}
	for (i = 0; i < 8; i++)
	{
		int j;

		for (j = 0; j < 8; j++)
		{
			ubuf888[i * 3 + j * 8 * 3 + 0] = i + j * 8;
			ubuf888[i * 3 + j * 8 * 3 + 1] = i + j * 8;
			ubuf888[i * 3 + j * 8 * 3 + 2] = i + j * 8;
		}
	}
	memset(store, 0, sizeof(store));
	memset(store16, 0, sizeof(store16));

	setRepresentation(sbuf8, 1, true, 1);
//	assert(readbuf(&global, 0, 0, 0) == 0 + INT8_MAX + 1);
//	assert(readbuf(&global, 1, 0, 0) == 8 + INT8_MAX + 1);

	setRepresentation(sbuf16, 2, true, 1);
//	assert(readbuf(&global, 0, 0, 0) == 0 + INT16_MAX + 1);
//	assert(readbuf(&global, 1, 0, 0) == 32 + INT16_MAX + 1);

	setRepresentation(ubuf8, 1, false, 1);
	assert(readbuf(&global, 0, 0, 0) == 0);
	assert(readbuf(&global, 1, 0, 0) == 16);
	assert(readbuf(&global, 2, 0, 0) == 32);

	setRepresentation(ubuf16, 2, false, 1);
	assert(readbuf(&global, 0, 0, 0) == 0);
	assert(readbuf(&global, 1, 0, 0) == 64);
	assert(readbuf(&global, 2, 0, 0) == 128);

	setRepresentation(ubuf888, 1, false, 3);
	global.size_old[1] = 8;
	assert(readbuf(&global, 0, 0, 0) == 0);
	assert(readbuf(&global, 0, 0, 1) == 0);
	assert(readbuf(&global, 0, 0, 2) == 0);
	// Commented out because we need a smarter test after I fixed
	// the RGB function so that it now correctly uses planar mode.
/*	assert(readbuf(&global, 1, 0, 0) == 1);
	assert(readbuf(&global, 1, 0, 1) == 1);
	assert(readbuf(&global, 1, 0, 2) == 1);
	assert(readbuf(&global, 2, 0, 0) == 2);
	assert(readbuf(&global, 2, 0, 1) == 2);
	assert(readbuf(&global, 2, 0, 2) == 2);
	assert(readbuf(&global, 3, 0, 0) == 3);
	assert(readbuf(&global, 0, 1, 0) == 8);
	assert(readbuf(&global, 1, 1, 0) == 9);
	assert(readbuf(&global, 4, 4, 0) == 36);*/
	global.size_old[1] = 1;

	setRepresentation(ubuf8, 1, false, 1);
	global.buffer = (char *)store;
	setnewbuf(&global, 0, 0, 0, 0, 0);
	assert(store[0] == 0);
	setnewbuf(&global, 1, 0, 0, 64, 0);
	setnewbuf(&global, 2, 0, 0, 0, 0);
	assert(store[0] == 0);
	assert(store[1] == 64);
	assert(store[2] == 0);

	setRepresentation(ubuf16, 2, false, 1);
	global.buffer = (char *)store16;
	setnewbuf(&global, 0, 0, 0, 0, 0);
	assert(store16[0] == 0);
	setnewbuf(&global, 1, 0, 0, 64, 0);
	setnewbuf(&global, 2, 0, 0, 0, 0);
	assert(store16[0] == 0);
	assert(store16[1] == 64);
	assert(store16[2] == 0);

	setRepresentation(ubuf888, 1, false, 3);
	global.buffer = (char *)store888;
	setnewbuf(&global, 0, 0, 0, 7, 0);
	assert(store888[0] == 7);
	setnewbuf(&global, 0, 0, 0, 7, 1);
	assert(store888[1] == 7);
	setnewbuf(&global, 0, 0, 0, 7, 2);
	assert(store888[2] == 7);

	result = DICOMLib_Done();
	assert( result == 0 );

	SSC_Logging_Done();

	return 0;
}
