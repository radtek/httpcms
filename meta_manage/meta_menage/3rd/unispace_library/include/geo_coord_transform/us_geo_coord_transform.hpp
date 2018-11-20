#ifndef __US_GEO_COORD_TRANSFORM_HPP__
#define __US_GEO_COORD_TRANSFORM_HPP__

#if defined _WIN32 || defined __CYGWIN__
	#ifdef _US_GEO_COORD_TRANSFORM_LIB
		#ifdef __GNUC__
			#define _US_GEO_COORD_TRANSFORM_DLL __attribute__ ((dllexport))
		#else
			#define _US_GEO_COORD_TRANSFORM_DLL __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define _US_GEO_COORD_TRANSFORM_DLL __attribute__ ((dllimport))
		#else
			#define _US_GEO_COORD_TRANSFORM_DLL __declspec(dllimport)
		#endif
	#endif
	#define _US_GEO_COORD_TRANSFORM_LOCAL
#else
	#if __GNUC__ >= 4
		#define _US_GEO_COORD_TRANSFORM_DLL    __attribute__ ((visibility ("default")))
		#define _US_GEO_COORD_TRANSFORM_LOCAL  __attribute__ ((visibility ("hidden")))
	#else
		#define _US_GEO_COORD_TRANSFORM_DLL
		#define _US_GEO_COORD_TRANSFORM_LOCAL
	#endif
#endif


#if !defined(__US_SPATIAL_REFERENCE_HPP__)

#include "us_spatial_reference.hpp"

#endif

#endif