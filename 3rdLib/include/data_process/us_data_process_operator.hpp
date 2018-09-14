/**********************************************************************//**
 * @file us_data_process_operator.hpp
 *
 * 声明 建金字塔/读取影像元数据/投影和影像行列坐标转换四个函数.
 *************************************************************************/

#ifndef __US_DATA_PROCESS_OPERATOR_HPP__
#define __US_DATA_PROCESS_OPERATOR_HPP__


#ifndef __US_IMAGE_HPP__
#include "us_image.hpp"
#endif


#ifndef __US_DATA_PROCESS_HPP__
#include "us_data_process.hpp"
#endif

namespace unispace
{
	class us_image_basic_meta;
	class us_ustring;
	class us_grid_base;
	class us_grid_uv;
	class us_grid_id;
	class us_uuid;

	/**********************************************************************//**
	 * @brief	初始化data process库.
	 *			执行以下初始化动作，包括GDAL的初始化.
	 * @author yimin.liu@unispace-x.com
	 * @date 2016/3/25
	 *************************************************************************/
	_US_DATA_PROCESS_DLL void initialization_data_process();

	_US_DATA_PROCESS_DLL bool generate_image_meta(const us_ustring& path,
												  us_image_basic_meta* out_meta,
												  bool update_uuid = true);

	/**********************************************************************//**
	 * @brief	影像数据冷转热.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2016/12/26
	 * @param	meta	影像的元数据.
	 * @return	成功返回0，失败返回负数.
	 *************************************************************************/
	_US_DATA_PROCESS_DLL int create_image_heating_data(
		const us_image_basic_meta& meta);

	/**********************************************************************//**
	 * @brief	影像数据热转冷.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2016/12/26
	 * @param	meta	影像的元数据.
	 * @return	成功返回0，失败返回负数.
	 *************************************************************************/
	_US_DATA_PROCESS_DLL int remove_image_heating_data(
		const us_image_basic_meta& meta);

	/**********************************************************************//**
	 * @brief	获取指定路径的影像的网格UV.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2016/6/28
	 * @param	path			    影像文件的路径.
	 * @param	grid
	 * @param	[in,out] out_uv If non-null, the out meta.
	 * @return true if it succeeds, false if it fails.
	 *************************************************************************/
	_US_DATA_PROCESS_DLL bool generate_image_grid_uv(const us_ustring& path,
													 const us_grid_base& grid,
													 us_grid_uv* out_uv);


	_US_DATA_PROCESS_DLL bool generate_image_grid_uv(const us_ustring& path,
													 const us_grid_id& gid,
													 us_grid_uv* out_uv);

	_US_DATA_PROCESS_DLL bool generate_image_grid_uv(const us_ustring& path,
													 const char* grid_str,
													 us_grid_uv* out_uv);

	_US_DATA_PROCESS_DLL bool generate_image_grid_uv(const us_image_basic_meta& meta,
													 const char* grid_str,
													 us_grid_uv* out_uv);

	
	/**********************************************************************//**
	 * @brief	根据已有点对(GCP地面控制点)进行坐标转换计算.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2018/6/27
	 * @param	gcp_count	点对数量.
	 * @param	gcplist1	点对中与待转换坐标统一平面的点列表.
	 * @param	gcplist2	点对中与目标坐标统一平面的点列表.
	 * @param	pointcount	待转换点数量.
	 * @param	points		待转换点数组.
	 * @return  成功转换返回true失败返回false.
	 *************************************************************************/
	_US_DATA_PROCESS_DLL bool us_gcp_transform(
		int gcp_count, Point2lf* gcplist1, Point2lf* gcplist2,
		int pointcount, Point2lf* points);

}

#ifndef __US_IMAGE_CACHE_HPP__
#include "us_image_cache.hpp"
#endif

#ifndef __US_DATASET_CACHE_HPP__
#include "us_dataset_cache.hpp"
#endif

#ifndef __US_IMAGE_ENCODE_FORMAT_HPP__
#include "us_image_encode_format.hpp"
#endif

#endif //!__US_DATA_PROCESS_OPERATOR_HPP__
