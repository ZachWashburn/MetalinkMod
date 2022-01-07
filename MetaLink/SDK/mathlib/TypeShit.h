#pragma once 

#include <math.h>
#include <float.h>


typedef float vec_t;
#ifdef _WIN32
typedef __int32 vec_t_as_gpr;
#endif



inline unsigned long& FloatBits(vec_t& f)
{
	return *reinterpret_cast<unsigned long*>(&f);
}

inline unsigned long const& FloatBits(vec_t const& f)
{
	return *reinterpret_cast<unsigned long const*>(&f);
}

inline vec_t BitsToFloat(unsigned long i)
{
	return *reinterpret_cast<vec_t*>(&i);
}

inline bool IsFinite(const vec_t& f)
{
	return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
}

#define FLOAT32_NAN_BITS     (uint32)0x7FC00000	// not a number!
#define FLOAT32_NAN          BitsToFloat( FLOAT32_NAN_BITS )

#define VEC_T_NAN FLOAT32_NAN

#define MIN min
#define MAX max
// In win32 try to use the intrinsic fabs so the optimizer can do it's thing inline in the code
#pragma intrinsic( fabs )
// Also, alias float make positive to use fabs, too
// NOTE:  Is there a perf issue with double<->float conversion?

inline float FloatMakeNegative(vec_t f)
{
	return -fabsf(f);// was since 2002: BitsToFloat( FloatBits(f) | 0x80000000 ); fixed in 2010
}

inline float FloatMakePositive(vec_t f)
{
	return fabsf(f);
}

inline float FloatNegate(vec_t f)
{
	return -f; //BitsToFloat( FloatBits(f) ^ 0x80000000 );
}
