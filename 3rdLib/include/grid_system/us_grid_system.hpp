#ifndef __US_GRID_SYSTEM_HPP__
#define __US_GRID_SYSTEM_HPP__

#if defined _WIN32 || defined __CYGWIN__
	#ifdef _US_GRID_SYSTEM_LIB
		#ifdef __GNUC__
			#define _US_GRID_SYSTEM_DLL __attribute__ ((dllexport))
		#else
			#define _US_GRID_SYSTEM_DLL __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define _US_GRID_SYSTEM_DLL __attribute__ ((dllimport))
		#else
			#define _US_GRID_SYSTEM_DLL __declspec(dllimport)
		#endif
	#endif
	#define _US_GRID_SYSTEM_LOCAL
#else
	#if __GNUC__ >= 4
		#define _US_GRID_SYSTEM_DLL    __attribute__ ((visibility ("default")))
		#define _US_GRID_SYSTEM_LOCAL  __attribute__ ((visibility ("hidden")))
	#else
		#define _US_GRID_SYSTEM_DLL
		#define _US_GRID_SYSTEM_LOCAL
	#endif
#endif


// 
#if !defined(__US_GRID_TYPE_HPP__) && \
	!defined(__US_GRID_ID_HPP__) && \
	!defined(__US_GRID_BASE_HPP__) && \
	!defined(__US_GRID_METER_HPP__) && \
	!defined(__US_GRID_DEGREE_HPP__)

#include "us_grid_type.hpp"
#include "us_grid_id.hpp"
#include "us_grid_base.hpp"
#include "us_grid_meter.hpp"
#include "us_grid_degree.hpp"

#endif



#endif //!__US_GRID_SYSTEM_HPP__