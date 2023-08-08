//
//  SetupMacro.h
//  
//
//  Created by Isaac Roberts on 5/31/22.
//

/**
If you include this class it will expect constants defined in advance
	NAME = name of enum
	ENUM = list of elements
	et(x) around each element
	optional: TO_STR_FUNC, default= {NAME}ToStr
*/ /*
Example usage:
#define NAME Fruits
#define ENUM \
	et(Apple) et(Orange) et(Banana) \
    et(Apricot) et(Mango)
*/
#ifdef ENUM

#ifndef NAME
#error "Macro must have name"
#endif



#define CAT_I(a,b) a##b
#define CAT(a,b) CAT_I(a, b)
//#define QUX(x) CAT(foo, BAZ) ## (x)


#define nullParam CAT(NAME, _Null)


#define et(x) x,
typedef enum { ENUM nullParam } NAME;
#undef et
#define et(x) case x: return #x;

#ifndef TO_STR_FUNC
#define TO_STR_FUNC CAT(NAME, ToStr)
#endif
//static const char *str##NAME[] = { MACRO };

inline const char* TO_STR_FUNC (NAME n)
{
	switch(n)
	{
			ENUM
		case nullParam: return "-NULL-";
	}
	return "-INVALID-ENUM-";
}
inline const char* TO_STR_FUNC (int n) { return TO_STR_FUNC((NAME)(n)); }


#undef et
#define et(x) +1

constexpr int CAT(NAME, Amt) = 0 ENUM;

#undef TO_STR_FUNC
#undef ENUM
#undef NAME
#undef et
#undef CAT_I
#undef CAT
#undef nullParam


#endif

//#endif /* SetupMacro_h */
