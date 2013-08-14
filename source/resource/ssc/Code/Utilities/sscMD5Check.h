#ifndef SSCMD5CHECK_H_
#define SSCMD5CHECK_H_

namespace ssc
{

/**
 * \addtogroup sscUtility
 * \{
 */

bool GenerateMD5( const char* const pathname ); ///< Generate MD5 sum of specified file and store it in a file with the same name with .md5 appended.
bool CheckMD5( const char* const pathname ); ///< Locate the file that has the same name as filename with .md5 appended and check that the sum is correct.
bool CheckMD5InMemory( const char* const pathname, const unsigned char* const data, int size ); ///< as SW_CheckMD5(filename), but the file is already loaded.

/**
 * \}
 */

}

#endif /*SSCMD5CHECK_H_*/
