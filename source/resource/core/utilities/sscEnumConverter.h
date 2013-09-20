#ifndef ENUMCONVERTER_H_
#define ENUMCONVERTER_H_

#include <string>
#include <boost/array.hpp>
#include <boost/algorithm/string.hpp>
#include <QString>
#include "sscTypeConversions.h"

/**
 * \addtogroup sscUtility
 * @{
 */

/**\brief Class for easy conversion between an enum and a QString.
 * 
 * NOTE: Use the helper macros instead of the description below.
 * 
    Macro example:
 
		namespace vm 
		{
			enum VIEW_ZONE_TYPE
			{
			  vtACS_3D_LAYOUT = 0,
			  vtANYPLANE_LAYOUT = 1,
			  vtDUALANYPLANE_LAYOUT = 2,
			  vtCOUNT
			};
		} // namespace vm

	Macro in header file (right after enum def, outside namespace):

		SNW_DECLARE_ENUM_STRING_CONVERTERS(vm, VIEW_ZONE_TYPE);

	Macro in cpp file (outside namespace):

		SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(vm, VIEW_ZONE_TYPE, vtCOUNT) 
		{
			"ACS_3D_LAYOUT",
			"ANYPLANE_LAYOUT",
			"DUALANYPLANE_LAYOUT",
		} 
		SNW_DEFINE_ENUM_STRING_CONVERTERS_END(vm, VIEW_ZONE_TYPE, vtCOUNT)

	End macro example.

 * 
 * 
 * 
 * Usage: Use the two nonmember functions enum2string() and string2enum()
 * to convert between enum and string and back.
 * 
 * Implementation for a new enum:
 * We use this one as an example: 
	namespace vm {
	enum VIEW_ZONE_TYPE
	{
	  vtACS_3D_LAYOUT = 0,
	  vtANYPLANE_LAYOUT = 1,
	  vtDUALANYPLANE_LAYOUT = 2,
	  vtCOUNT
	};
	} // vm 
 *  
 * Define the mText array for your enum, for example
 * 
	template<>
	boost::array<QString, vm::vtCOUNT> EnumConverter<vm::VIEW_ZONE_TYPE,vm::vtCOUNT>::mText =
	{
		{
			"ACS_3D_LAYOUT",
			"ANYPLANE_LAYOUT",
			"DUALANYPLANE_LAYOUT",
		}};
 * 
 * Then implement the two nonmember functions to call their respective member functions, as follows:
 * 
	template<> QString enum2string<vm::VIEW_ZONE_TYPE>(const vm::VIEW_ZONE_TYPE& val)
	{
		return EnumConverter<vm::VIEW_ZONE_TYPE, vm::vtCOUNT>::enum2string(val);
	}
	template<> vm::VIEW_ZONE_TYPE string2enum<vm::VIEW_ZONE_TYPE>(const QString& val)
	{
		return EnumConverter<vm::VIEW_ZONE_TYPE, vm::vtCOUNT>::string2enum(val);
	}
 *
 * Remember to do everything in the global namespace, and in a .cpp file.
 * 
 * You can also define operators << and >> based on these functions. 
 * 
 */
template<class ENUM, unsigned COUNT>
class EnumConverter
{
public:
	static boost::array<QString, COUNT> mText;

	static QString enum2string(const ENUM& val)
	{
		if (unsigned(val)<mText.size())
			return mText[val];
		return "UNDEFINED";
	}

	static ENUM string2enum(const QString& val)
	{
		for (unsigned i=0; i<mText.size(); ++i)
			if (mText[i].toUpper()==val.toUpper())
				return static_cast<ENUM>(i);
		return static_cast<ENUM>(COUNT);
	}
};

template<class ENUM>
ENUM string2enum(const QString& val);

template<class ENUM>
QString enum2string(const ENUM& val);



/** Helper macro for declaring the enum2string template specializations.
 *  Use this in an header file following the enum declarations. (outside the namespace).
 */
#define SNW_DECLARE_ENUM_STRING_CONVERTERS(NS, ENUM_NAME)            \
namespace NS                                                         \
{                                                                    \
	std::ostream& operator<<(std::ostream& s, const ENUM_NAME& val); \
}                                                                    \
template<>                                                           \
QString enum2string<NS::ENUM_NAME>(const NS::ENUM_NAME& val);    \
template<>                                                           \
NS::ENUM_NAME string2enum<NS::ENUM_NAME>(const QString& val);    \
//-----------------------------------------------------------------

/**Use these macros to generate string<-->enum converter functions 
 * for an enum that has been instantiated with EnumConverter.
 * Use in the cpp file after EnumConverter implementations, outside the namespace.
 * 
 * Example:
 * 
 * SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(vm, DUALPLANE_TYPE, dpCOUNT) 
 * {
 * 	"SIDE_VIEW",
 * 	"RADIAL_VIEW"
 * } 
 * SNW_DEFINE_ENUM_STRING_CONVERTERS_END(vm, DUALPLANE_TYPE, dpCOUNT)
 *  
 */
#define SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(NS, ENUM_NAME, ENUM_SIZE)                  \
template<>                                                                                 \
boost::array<QString, NS::ENUM_SIZE> EnumConverter<NS::ENUM_NAME,NS::ENUM_SIZE>::mText = \
{                                                                                          \

#define SNW_DEFINE_ENUM_STRING_CONVERTERS_END(NS, ENUM_NAME, ENUM_SIZE)     \
};                                                                          \
template<> QString enum2string<NS::ENUM_NAME>(const NS::ENUM_NAME& val) \
{                                                                           \
	return EnumConverter<NS::ENUM_NAME, NS::ENUM_SIZE>::enum2string(val);   \
}                                                                           \
template<> NS::ENUM_NAME string2enum<NS::ENUM_NAME>(const QString& val) \
{                                                                           \
	return EnumConverter<NS::ENUM_NAME, NS::ENUM_SIZE>::string2enum(val);   \
}                                                                           \
namespace NS                                                                \
{                                                                           \
	std::ostream& operator<<(std::ostream& s, const ENUM_NAME& val)         \
	{                                                                       \
		s << enum2string(val);                                              \
		return s;                                                           \
	}                                                                       \
}                                                                           \
// --------------------------------------------------------

/**
 * @}
 */

#endif /*ENUMCONVERTER_H_*/
