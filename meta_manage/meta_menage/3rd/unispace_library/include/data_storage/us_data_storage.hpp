#ifndef __US_DATA_STORAGE_HPP__
#define __US_DATA_STORAGE_HPP__

#if defined _WIN32 || defined __CYGWIN__
	#ifdef _US_DATA_STORAGE_LIB
		#ifdef __GNUC__
			#define _US_DATA_STORAGE_DLL __attribute__ ((dllexport))
		#else
			#define _US_DATA_STORAGE_DLL __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define _US_DATA_STORAGE_DLL __attribute__ ((dllimport))
		#else
			#define _US_DATA_STORAGE_DLL __declspec(dllimport)
		#endif
	#endif
	#define _US_DATA_STORAGE_LOCAL
#else
	#if __GNUC__ >= 4
		#define _US_DATA_STORAGE_DLL    __attribute__ ((visibility ("default")))
		#define _US_DATA_STORAGE_LOCAL  __attribute__ ((visibility ("hidden")))
	#else
		#define _US_DATA_STORAGE_DLL
		#define _US_DATA_STORAGE_LOCAL
	#endif
#endif


#if !defined(__US_PATH_HPP__) && \
	!defined(__US_URL_HPP__) && \
	!defined(__US_DATA_STORAGE_MANAGER_HPP__)

#include "us_path.hpp"
#include "us_url.hpp"
#include "us_data_storage_manager.hpp"

#endif


#endif // !__US_DATA_STORAGE_HPP__

