/**********************************************************************//**
 * @file us_util.hpp
 *
 * 对外提供对所有Util模块中头文件的Include.
 *************************************************************************/

#ifndef __US_UTIL_HPP__
#define __US_UTIL_HPP__

#if defined _WIN32 || defined __CYGWIN__
	#ifdef _US_UTIL_LIB
		#ifdef __GNUC__
			#define _US_UTIL_DLL __attribute__ ((dllexport))
		#else
			#define _US_UTIL_DLL __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define _US_UTIL_DLL __attribute__ ((dllimport))
		#else
			#define _US_UTIL_DLL __declspec(dllimport)
		#endif
	#endif
	#define _US_UTIL_LOCAL
#else
	#if __GNUC__ >= 4
		#define _US_UTIL_DLL    __attribute__ ((visibility ("default")))
		#define _US_UTIL_LOCAL  __attribute__ ((visibility ("hidden")))
	#else
		#define _US_UTIL_DLL
		#define _US_UTIL_LOCAL
	#endif
#endif

// 定义最大可读取配置文件大小(默认16MB)
#ifndef MAX_CONFIG_FILE_SIZE
#define MAX_CONFIG_FILE_SIZE (1024*1024*16)
#endif


#if !defined(__US_STRING_HPP__) && \
	!defined(__US_VEC2_HPP__) && \
	!defined(__US_VEC3_HPP__) && \
	!defined(__US_VEC4_HPP__) && \
	!defined(__US_MATRIX4_HPP__) && \
	!defined(__US_SIZE_HPP__) && \
	!defined(__US_RECT_HPP__) && \
	!defined(__US_GEO_RECT_HPP__) && \
	!defined(__US_LATLNG_RECT_HPP__) && \
	!defined(__US_UUID_HPP__) && \
	!defined(__US_UUID_FACTORY_HPP__) && \
	!defined(__US_CONFIG_VALUE_HPP__) && \
	!defined(__US_KEY_VALUE_CONFIG_HPP__) && \
	!defined(__US_INI_CONFIG_HPP__)	&& \
	!defined(__US_FILESYSTEM_HPP__) && \
	!defined(__US_LOG_HPP__)

#include "common/us_string.hpp"
#include "math/us_vec2.hpp"
#include "math/us_vec3.hpp"
#include "math/us_vec4.hpp"

#include "math/us_matrix4.hpp"

#include "math/us_size.hpp"
#include "math/us_rect.hpp"
#include "math/us_geo_rect.hpp"
#include "math/us_latlng_rect.hpp"

#include "uuid/us_uuid.hpp"
#include "uuid/us_uuid_factory.hpp"

#include "config/us_config_value.hpp"
#include "config/us_key_value_config.hpp"
#include "config/us_ini_config.hpp"

#include "filesys/us_filesystem.hpp"

#include "log/us_log.hpp"

#endif

#endif	//! __US_UTIL_HPP__
