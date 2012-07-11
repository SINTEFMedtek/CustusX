#include "sscMD5Check.h"

#ifndef WIN32
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <unistd.h>
#include <stdbool.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <iostream>

namespace ssc
{

/**
 * @param digest (out) The digest constructed from the content of the file
 * @param pathname (in) the name of a file that we want to find the digest file for.
 * @return true if digest created, false otherwise.
 */
bool CreateFileDigest( unsigned char digest[ MD5_DIGEST_LENGTH ], const char* const pathname );

/**
 * @param digest (out) The digest constructed from the content of the file
 * @param data (in) Data buffer to MD5 to generate checksum from
 * @param size (in) buffer size
 * @return true if digest created, false otherwise.
 */
bool CreateArrayDigest( unsigned char digest[ MD5_DIGEST_LENGTH ], const unsigned char* const data, int size );

/**
 * @param digest_name (out) The name of the digest file of pathname
 * @param size (in) The available space in digest_name
 * @param pathname (in) the name of a file that we want to find the digest file for.
 * @return true if file name created, false otherwise
 */
bool DetermineDigestFileName( char* const digest_name, const unsigned int size, const char* const pathname );

/**
 * @param digest (in) The digest to write into pathname
 * @param sourcename (in) The name of the file that this is a digest for. Used to write the file name into the digest file to allow convenient MD5 checking with md5sum. If NULL, nothing will be written. Notice: This must be a zero terminated string.
 * @param pathname (in) the name of a file that we want to write a digest into
 * @return true if digest file created, false otherwise
 */
bool WriteDigestToFile( unsigned char digest[ MD5_DIGEST_LENGTH ], const char* const sourcename, const char* const pathname );

/**
 * @param digest (out) The digest read from the file
 * @param pathname (in) the name of a file that we want to read a digest from
 * @return true if digest read, false otherwise.
 */
bool ReadDigestFromFile( unsigned char digest[ MD5_DIGEST_LENGTH ], const char* const pathname );

#define FORMATTED_MD5_DIGEST_LENGTH MD5_DIGEST_LENGTH * 2

/**
 * Helper function used to convert a 32 byte character string of hex values into a 16 byte digest.
 */
bool CharToDigest( unsigned char out[ MD5_DIGEST_LENGTH ], char in[ FORMATTED_MD5_DIGEST_LENGTH ] )
{
#if MD5_DIGEST_LENGTH != 16
#error Portability issue! Attempting to compile on a system where MD5_DIGEST_LENGTH != 16. This requires that the sscanf line below receives a bit of attention, it is hardcoded to 16B (128b) digests. Attempting to run without fixing this will probably lead to a crash.
#endif
	unsigned int item_0 = 0, item_1 = 0, item_2 = 0, item_3 = 0, item_4 = 0, item_5 = 0, item_6 = 0, item_7 = 0, item_8 = 0, item_9 = 0, item_10 = 0, item_11 = 0, item_12 = 0, item_13 = 0, item_14 = 0, item_15 = 0;
	int sscanf_res = sscanf( in, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", &item_0, &item_1, &item_2, &item_3, &item_4, &item_5, &item_6, &item_7, &item_8, &item_9, &item_10, &item_11, &item_12, &item_13, &item_14, &item_15 );
	if ( sscanf_res != MD5_DIGEST_LENGTH )
	{
		std::cout << "Failed to format the file content, formatted " <<  sscanf_res <<  " bytes." << std::endl;
		return false;
	}
	out[0]  = item_0;
	out[1]  = item_1;
	out[2]  = item_2;
	out[3]  = item_3;
	out[4]  = item_4;
	out[5]  = item_5;
	out[6]  = item_6;
	out[7]  = item_7;
	out[8]  = item_8;
	out[9]  = item_9;
	out[10] = item_10;
	out[11] = item_11;
	out[12] = item_12;
	out[13] = item_13;
	out[14] = item_14;
	out[15] = item_15;
	return true;
}

/**
 * Helper function used to convert a 16 byte digest into a 32 byte character string of hex values.
 */
bool DigestToChar( char out[ FORMATTED_MD5_DIGEST_LENGTH ], unsigned char in[ MD5_DIGEST_LENGTH ] )
{
#if MD5_DIGEST_LENGTH != 16
#error Portability issue! Attempting to compile on a system where MD5_DIGEST_LENGTH != 16. This requires that the printing lines below receives a bit of attention, it is hardcoded to 16B (128b) digests. Attempting to run without fixing this will probably lead to a crash.
#endif
	char tmp[ FORMATTED_MD5_DIGEST_LENGTH + 1 ]; // Needed because snprintf appends a trailing 0.
	char sprintf_res = snprintf( tmp, FORMATTED_MD5_DIGEST_LENGTH + 1, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", in[0], in[1], in[2], in[3], in[4], in[5], in[6], in[7], in[8], in[9], in[10], in[11], in[12], in[13], in[14], in[15] );
	if ( sprintf_res != FORMATTED_MD5_DIGEST_LENGTH )
	{
		std::cout << "Error formatting the digest into a hex string, sprintf returned" << sprintf_res <<  "bytes written, " << FORMATTED_MD5_DIGEST_LENGTH << " expected" << std::endl;
		return false;
	}
	memcpy( out, tmp, FORMATTED_MD5_DIGEST_LENGTH );
	return true;
}

bool CreateFileDigest( unsigned char digest[ MD5_DIGEST_LENGTH ], const char* const pathname )
{
	// Open input file.
	int input_file = open( pathname, O_RDONLY );
	if ( input_file < 0 )
	{
		std::cout << "Can't generate MD5 sum for file " << pathname << ", unable to open input file. Errno:" <<errno<<": "<<strerror(errno)<<""<<std::endl;
		return false;
	}

	// Prepare for md5-ing
	MD5_CTX md5_context;
	MD5_Init( &md5_context );

	// Traverse input file, generate MD5
	const int block_max_length = 1024;
	unsigned char file_block[ block_max_length ];
	int block_length = read( input_file, file_block, block_max_length );
	while ( block_length != 0 )
	{
		if ( block_length < 0 )
		{
			std::cout << "Can't generate MD5 sum for file "<<pathname<<", error reading input file. Errno: "<<errno<<": "<<strerror( errno )<<""<<std::endl;
			close( input_file );
			return false;
		}
		else
		{
			MD5_Update( &md5_context, file_block, block_length );
		}
		block_length = read( input_file, file_block, block_max_length );
	}

	// Close input file
	close( input_file );

	MD5_Final( digest, &md5_context );
	return true;
}

bool CreateArrayDigest( unsigned char digest[ MD5_DIGEST_LENGTH ], const unsigned char* const data, int size )
{
	// Prepare for md5-ing
	MD5_CTX md5_context;
	MD5_Init( &md5_context );

	// Traverse input file, generate MD5
	const int block_max_length = 1024;


	const unsigned char* file_block = data;
	for ( file_block=data; file_block<data+size; file_block+=block_max_length )
	{
		int block_length = block_max_length;
		if (file_block+block_length > data+size)
		{
		//	block_length = size % block_length;
			block_length = data+size - file_block; 
		}
		if (block_length==0)
		{
			break;
		}

		MD5_Update( &md5_context, file_block, block_length );
	}

	MD5_Final( digest, &md5_context );
	return true;
}


bool DetermineDigestFileName( char* const digest_name, const unsigned int size, const char* const pathname )
{
	// Determine output file name
	if ( strlen( pathname ) + 4 + 1 > size )
	{
		std::cout << "Can't generate MD5 sum for file "<<pathname<<", there is not enough room in the buffer for the output file name." << std::endl ;
		return false;
	}
	strcpy( digest_name, pathname ); // Not using strncpy because the size is already checked.
	strcat( digest_name, ".md5" ); // Not using strncat because the size is already checked.
	return true;
}

bool WriteDigestToFile( unsigned char digest[ MD5_DIGEST_LENGTH ], const char* const sourcename, const char* const pathname )
{
	// Format the digest
	char formatted_digest[ FORMATTED_MD5_DIGEST_LENGTH ];
	bool res_tochar = DigestToChar( formatted_digest, digest );
	if ( res_tochar == false )
	{
		std::cout << "Can't create MD5 sum, failed to format the digest into a hex string." << std::endl;
		return false;
	}

	// Open file
	int output_file = open( pathname, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH );
	if ( output_file < 0 )
	{
		std::cout << "Can't create MD5 sum file "<<pathname<<", the file can't be opened for writing. Errno: "<<errno<<": "<<strerror( errno )<<""<<std::endl;
		return false;
	}

	// Write output file
	int written_bytes = write( output_file, formatted_digest, FORMATTED_MD5_DIGEST_LENGTH );
	if ( written_bytes < 0 )
	{
		std::cout << "Can't write MD5 sum into the file "<<pathname<<", write returned error. Errno: "<<errno<<": "<<strerror( errno )<<""<<std::endl;
		close( output_file );
		return false;
	}
	else if ( written_bytes < FORMATTED_MD5_DIGEST_LENGTH )
	{
		std::cout << "Can't write MD5 sum into the file "<<pathname<<", write indicates not all data written. Bytes written: " << written_bytes<<std::endl ;
		close( output_file );
		return false;
	}

	// Write additional file name if relevant. Make the file end with a line break, convenient when cat-ing.
	// Don't check these calls for error, as this is not essential.
	if ( sourcename != NULL )
	{
		write( output_file, "  ", 2 );
		write( output_file, sourcename, strlen( sourcename ) );
	}
	write( output_file, "\n", 1 );

	// Close output file
	int close_return = close( output_file );
	if ( close_return < 0 ) 
	{
		std::cout << "There was a problem closing MD5 file "<<pathname<<". File might be incorrect. Errno: "<<errno<<": "<<strerror( errno )<< std::endl;
		return false;
	}
	return true;
}

bool ReadDigestFromFile( unsigned char digest[ MD5_DIGEST_LENGTH ], const char* const pathname )
{
	int digest_file = open( pathname, O_RDONLY );
	if ( digest_file < 0 )
	{
		std::cout << "Can't read MD5 sum from file "<<pathname<<", unable to open. Errno: "<<errno<<": "<<strerror( errno )<< std::endl;
		return false;
	}

	char formatted_digest_buffer[ FORMATTED_MD5_DIGEST_LENGTH ];
	// Initialize buffer
	int i;
	for ( i = 0; i < FORMATTED_MD5_DIGEST_LENGTH; i++ )
	{
		formatted_digest_buffer[ i ] = '\0';
	}
	int digest_length = read( digest_file, formatted_digest_buffer, FORMATTED_MD5_DIGEST_LENGTH );
	if ( digest_length < 0 )
	{
		std::cout << "Can't read MD5 sum for file "<<pathname<<", error reading from file. Errno: "<<errno<<": "<<strerror( errno )<< std::endl;
		close( digest_file );
		return false;
	}
	else if ( digest_length != FORMATTED_MD5_DIGEST_LENGTH )
	{
		std::cout << "Error reading MD5 sum for file "<<pathname<<", failed to read the correct number of bytes. Bytes read: "<<digest_length<<", bytes expected: " << FORMATTED_MD5_DIGEST_LENGTH  << std::endl;
		close( digest_file );
		return false;
	}
	close( digest_file );

	// convert the string checksum to an unsigned char array.
	bool res_todigest = CharToDigest( digest, formatted_digest_buffer );
	if ( res_todigest == false )
	{
		std::cout << "Error reading MD5 sum for file "<<pathname<<", failed to format the file content." << std::endl;
		return false;
	}

	return true;
}


bool GenerateMD5( const char* const pathname )
{
	unsigned char md5_buffer[ MD5_DIGEST_LENGTH ];
	if ( CreateFileDigest( md5_buffer, pathname ) == false )
	{
		std::cout << "Unable to generate MD5 of "<< pathname << std::endl;
		return false;
	}
	char md5_pathname[ PATH_MAX ];
	if ( DetermineDigestFileName( md5_pathname, PATH_MAX, pathname ) == false )
	{
		std::cout << "Unable to generate MD5 of "<<pathname<<", can't create pathname for digest." << std::endl;
		return false;
	}
	if ( WriteDigestToFile( md5_buffer, pathname, md5_pathname ) == false )
	{
		std::cout << "Unable to generate MD5 of "<<pathname<<", can't write to target file "<<md5_pathname<<"." << std::endl;
		return false;
	}
	std::cout << "MD5 sum for file "<<pathname<<", written to file "<<md5_pathname<<"." << std::endl;
	return true;
}

bool CheckMD5( const char* const pathname )
{
	unsigned char md5_buffer[ MD5_DIGEST_LENGTH ];
	if ( CreateFileDigest( md5_buffer, pathname ) == false )
	{
		std::cout << "Unable to check MD5 of "<<pathname<<", can't checksum original file." << std::endl;
		return false;
	}
	char md5_pathname[ PATH_MAX ];
	if ( DetermineDigestFileName( md5_pathname, PATH_MAX, pathname ) == false )
	{
		std::cout << "Unable to check MD5 of "<<pathname<<", can't determine pathname for digest." << std::endl;
		return false;
	}
	unsigned char md5_from_file[ MD5_DIGEST_LENGTH ];
	if ( ReadDigestFromFile( md5_from_file, md5_pathname ) == false )
	{
		std::cout << "Unable to check MD5 of "<<pathname<<", can't read stored digest." << std::endl;
		return false;
	}
	if ( memcmp( md5_buffer, md5_from_file, MD5_DIGEST_LENGTH ) != 0 )
	{
		std::cout << "MD5 of "<<pathname<<" is not correct." << std::endl;
		return false;
	}
	return true;
}

bool CheckMD5InMemory( const char* const pathname, const unsigned char* const data, int size )
{
	unsigned char md5_buffer[ MD5_DIGEST_LENGTH ];
	if ( CreateArrayDigest( md5_buffer, data, size ) == false )
	{
		std::cout << "Unable to check MD5 of "<<pathname<<", can't checksum original file." << std::endl;
		return false;
	}
	char md5_pathname[ PATH_MAX ];
	if ( DetermineDigestFileName( md5_pathname, PATH_MAX, pathname ) == false )
	{
		std::cout << "Unable to check MD5 of "<<pathname<<", can't determine pathname for digest." << std::endl;
		return false;
	}
	unsigned char md5_from_file[ MD5_DIGEST_LENGTH ];
	if ( ReadDigestFromFile( md5_from_file, md5_pathname ) == false )
	{
		std::cout << "Unable to check MD5 of "<<pathname<<", can't read stored digest." << std::endl;
		return false;
	}
	if ( memcmp( md5_buffer, md5_from_file, MD5_DIGEST_LENGTH ) != 0 )
	{
		std::cout << "MD5 of "<<pathname<<" is not correct." << std::endl;
		return false;
	}
	return true;
}

}
#endif

