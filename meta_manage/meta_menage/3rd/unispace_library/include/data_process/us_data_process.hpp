#ifndef __US_DATA_PROCESS_HPP__
#define __US_DATA_PROCESS_HPP__


// 为保证正确的DLL导入导出，请不要修改如下定义
#if defined _WIN32 || defined __CYGWIN__
	#ifdef _US_DATA_PROCESS_LIB
		#ifdef __GNUC__
			#define _US_DATA_PROCESS_DLL __attribute__ ((dllexport))
		#else
			#define _US_DATA_PROCESS_DLL __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define _US_DATA_PROCESS_DLL __attribute__ ((dllimport))
		#else
			#define _US_DATA_PROCESS_DLL __declspec(dllimport)
		#endif
	#endif
	#define _US_DATA_PROCESS_LOCAL
#else
	#if __GNUC__ >= 4
		#define _US_DATA_PROCESS_DLL    __attribute__ ((visibility ("default")))
		#define _US_DATA_PROCESS_LOCAL  __attribute__ ((visibility ("hidden")))
	#else
		#define _US_DATA_PROCESS_DLL
		#define _US_DATA_PROCESS_LOCAL
	#endif
#endif




#if !defined(__US_IMAGE_HPP__) && \
	!defined(__US_DATA_PROCESS_OPERATOR_HPP__) && \
	!defined(__US_IMAGE_POLYGON_CLIP_HPP__) && \
	!defined(__US_POLY_CLIP_PARAM_HPP__) && \
	!defined(__US_IMAGE_READ_DATA_HPP__)

#include "us_image.hpp"
#include "us_data_process_operator.hpp"
#include "us_poly_clip_param.hpp"
#include "us_image_polygon_clip.hpp"
#include "us_image_read_data.hpp"

#endif






#endif	//! __US_DATA_PROCESS_HPP__


