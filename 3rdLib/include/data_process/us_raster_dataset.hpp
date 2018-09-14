#ifndef __US_RASTER_DATASET_HPP__
#define __US_RASTER_DATASET_HPP__

#ifndef __US_STRING_HPP__
#include <util/common/us_string.hpp>
#endif // !__US_STRING_HPP__

#ifndef __US_GEO_RECT_HPP__
#include "util/math/us_geo_rect.hpp"
#endif
#ifndef __US_OPERATOR_RESULT_HPP__
#include "util/common/us_operator_result.hpp"
#endif

#ifndef __US_DATA_PROCESS_HPP__
#include "us_data_process.hpp"
#endif

#include <memory>


namespace unispace
{
#if defined(_MSC_VER) && _MSC_VER < 1900
#define us_raster_dataset_impl int
#else
	class us_raster_dataset_impl;
#endif

	class us_image_basic_meta;

	class _US_DATA_PROCESS_DLL us_raster_dataset {
		std::shared_ptr<us_raster_dataset_impl> m_impl;
	public:
		explicit us_raster_dataset();
		~us_raster_dataset();
		
		// 要么只读打开
		us_operator_result open(const us_ustring& path);
		// 要么只用于写入
		us_operator_result save(us_ustring save_as_path);

		bool add_image(const us_image_basic_meta& imagemeta);
		bool add_image(const std::vector<us_image_basic_meta>& imagemetas);
		bool set_image(const std::vector<us_image_basic_meta>& imagemetas);


		void set_desc(const unispace::us_ustring& desc);
		void set_rgb_band(int rband, int gband, int bband);
		void set_invalid_rgb(uint32_t inrgb);
		void set_resolution(double resh, double resv);
		void set_spatial_reference(const unispace::us_uuid& sprid);

	public:
		/**********************************************************************//**
		 * @brief	获取逻辑图像宽度(横向像素数).
		 * @author	yimin.liu@unispace-x.com
		 * @date	2018/6/6
		 * @return	逻辑图像横向像素数.
		 *************************************************************************/
		int get_width()const;

		/**********************************************************************//**
		 * @brief	获取逻辑图像高度(纵向像素数).
		 * @author	yimin.liu@unispace-x.com
		 * @date	2018/6/6
		 * @return	逻辑图像纵向像素数.
		 *************************************************************************/
		int get_height()const;

		/**********************************************************************//**
		 * @brief	获取影像波段数.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/4/5
		 * @return	波段数.
		 *************************************************************************/
		int get_band_count()const;

		/**********************************************************************//**
		 * @brief	获取影像数据类型编号.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/4/13
		 * @return	获取成功返回像素数据类型编号，失败(获取波段失败)返回-1.
		 * 			|与GDAL中对应的数据类型|含义|
		 * 			|:---|:---|
		 *			|GDT_Unknown  = 0	|	未知或未指定类型|
		 *			|GDT_Byte     = 1	|	8bit无符号整数|
		 *			|GDT_UInt16   = 2	|	16bit无符号整数|
		 *			|GDT_Int16    = 3	|	16bit有符号整数|
		 *			|GDT_UInt32   = 4	|	32bit无符号整数|
		 *			|GDT_Int32    = 5	|	32bit有符号整数|
		 *			|GDT_Float32  = 6	|	32bit有符号浮点数|
		 *			|GDT_Float64  = 7	|	64bit有符号浮点数|
		 *			|GDT_CInt16   = 8	|	16bit有符号整型复数|
		 *			|GDT_CInt32   = 9	|	32bit有符号整型复数|
		 *			|GDT_CFloat32 = 10	|	32bit有符号浮点型复数|
		 *			|GDT_CFloat64 = 11	|	64bit有符号浮点型复数|
		 *			|GDT_TypeCount = 12	|	最大类型编号 + 1 |
		 *************************************************************************/
		int get_data_type()const;

		/**********************************************************************//**
		 * @brief	获取逻辑影像的仿射变换参数.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/4/27
		 * @param	[out] outTransform 传出仿射变换参数.
		 * @return	成返回true.
		 *************************************************************************/
		//bool get_geo_transfrom(double* outTransform)const;

		/**********************************************************************//**
		 * @brief	获取实际关联的影像的仿射变换参数.
		 *			注意，返回投影参考的生命周期与调用此接口的对象一致.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2017/8/8
		 * @return	成功返回影像投影参考指针(const char*),失败返回NULL.
		 *************************************************************************/
		//const char* get_projection_ref()const;

		/**********************************************************************//**
		 * @brief	获取逻辑图像的地理范围.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/4/27
		 * @return	逻辑图像的地理范围.
		 *************************************************************************/
		us_geo_rect get_geo_rect()const;

		int64_t get_rabg_buffer(
			const unispace::us_vec2<double>* gridGeoCorner,
			void* pData, int nBufXSize, int nBufYSize);
	};

}
#endif // !__US_RASTER_DATASET_HPP__
