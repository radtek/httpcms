#ifndef __US_DATASET_CACHE_HPP__
#define __US_DATASET_CACHE_HPP__

#ifndef __US_BASIC_UV_HPP__
#include <metadata/us_basic_uv.hpp>
#endif
#ifndef __US_UUID_HPP__
#include <util/uuid/us_uuid.hpp>
#endif
#ifndef __US_IMAGE_BASIC_META_HPP__
#include "metadata/us_image_basic_meta.hpp"
#endif

#ifndef __US_DATA_PROCESS_HPP__
#include "us_data_process.hpp"
#endif

#include <vector>

namespace unispace
{
	class us_ustring;
	class us_image_basic_meta;
	class us_grid_id;
	class us_basic_uv;
	class us_spatial_reference;
	
	/**********************************************************************//**
	 * @brief	创建数据集金字塔.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2017/8/7
	 * @param	dsetid	数据集的ID.
	 * @param	metalist 数据集中影像的元数据.
	 * @param	replace 是否替换掉已经存在的金字塔.
	 * @return	成功返回true,失败返回false.
	 *************************************************************************/
	_US_DATA_PROCESS_DLL bool creat_dataset_pyramid(
		const us_uuid& dsetid,
		const std::vector<us_image_basic_meta>& metalist,
		bool replace);

	/**********************************************************************//**
	 * @brief	创建数据集金字塔(使用 rmp 压缩包方式).
	 * @author	yimin.liu@unispace-x.com
	 * @date	2018/06/21
	 * @param	dsetid	数据集的ID.
	 * @param	metalist 数据集中影像的元数据.
	 * @param	r_band	 作为红色波段的波段序号.
	 * @param	g_band	 作为绿色波段的波段序号.
	 * @param	b_band	 作为蓝色波段的波段序号.
	 * @param	invalid_rgb  无效的RGB值(>0x01000000则使用).
	 * @param	pfnProgress  进度处理回调函数.
	 * @param	pProgressArg 进度处理函数附加参数.
	 * @return	成功返回true,失败返回false.
	 *************************************************************************/
	_US_DATA_PROCESS_DLL bool creat_dataset_pyramid_rgba_rmp(
		const us_uuid& dsetid,
		const std::vector<us_image_basic_meta>& metalist,
		int r_band, int g_band, int b_band, uint32_t invalid_rgb,
		bool replace,
		int(*pfnProgress)(double, const char *, void *) = NULL,
		void *pProgressArg = NULL);

	/**********************************************************************//**
	 * @brief	删除数据集金字塔.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2017/8/7
	 * @param [in]	dsetid 数据集的ID.
	 * @return		成功返回true,失败返回false.
	 ***********************************************************************/
	_US_DATA_PROCESS_DLL bool remove_dataset_pyramid(const us_uuid& dsetid);

	/**********************************************************************//**
	 * @brief	创建数据集全级别 rmp 压缩包.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2018/04/12
	 * @param	dsetid	数据集的ID.
	 * @param	metalist 数据集中影像的元数据.
	 * @param	r_band	 作为红色波段的波段序号.
	 * @param	g_band	 作为绿色波段的波段序号.
	 * @param	b_band	 作为蓝色波段的波段序号.
	 * @param	invalid_rgb  无效的RGB值(>0x01000000则使用).
	 * @param	pfnProgress  进度处理回调函数.
	 * @param	pProgressArg 进度处理函数附加参数.
	 * @return	成功返回true,失败返回false.
	 *************************************************************************/
	_US_DATA_PROCESS_DLL bool creat_dataset_rgba_rmp(
		const us_uuid& dsetid,
		const std::vector<us_image_basic_meta>& metalist,
		int r_band, int g_band, int b_band, uint32_t invalid_rgb,
		bool replace,
		int(*pfnProgress)(double, const char *, void *) = NULL,
		void *pProgressArg = NULL);

	/**********************************************************************//**
	 * @brief	数据集全级别 rmp 压缩包.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2018/04/12
	 * @param [in]	dsetid 数据集的ID.
	 * @return		成功返回true,失败返回false.
	 ***********************************************************************/
	_US_DATA_PROCESS_DLL bool remove_dataset_rgba_rmp(const us_uuid& dsetid);

	/**********************************************************************//**
	 * @brief	从数据集全级别 rmp 压缩包中读取RGBA数据.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2018/04/12
	 * @param [in]	dsetid 数据集的ID.
	 * @param	gridGeoCorner	读取数据的四个顶点地理坐标(左上、右上、右下、左下顺序).
	 * @param	pData		读取数据输出缓存.
	 * @param	nBufXSize	读取数据输出宽度.
	 * @param	nBufYSize	读取数据输出高度.
	 * @return  失败返回负值(当文件不存在时候返回特殊值-9999991).
	 ***********************************************************************/
	_US_DATA_PROCESS_DLL int64_t get_grid_dataset_rgba_buffer_by_pyramid_rmp(
		const unispace::us_uuid & dsetid,
		const unispace::us_vec2<double>* gridGeoCorner,
		void* pData, int nBufXSize, int nBufYSize);

	/**********************************************************************//**
	 * @brief	从数据集金字塔( rmp 压缩包格式)中读取RGBA数据.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2018/06/21
	 * @param [in]	dsetid 数据集的ID.
	 * @param	gridGeoCorner	读取数据的四个顶点地理坐标(左上、右上、右下、左下顺序).
	 * @param	pData		读取数据输出缓存.
	 * @param	nBufXSize	读取数据输出宽度.
	 * @param	nBufYSize	读取数据输出高度.
	 * @return  失败返回负值(当文件不存在时候返回特殊值-9999991).
	 ***********************************************************************/
	_US_DATA_PROCESS_DLL int64_t get_grid_dataset_rgba_buffer_by_rmp_cache(
		const unispace::us_uuid & dsetid,
		const unispace::us_vec2<double>* gridGeoCorner,
		void* pData, int nBufXSize, int nBufYSize);

	/**********************************************************************//**
	* @brief	获取网格范围数据集金字塔影像RGBA像素数据.
	* 			从数据集金字塔的影像中获取网格对应区域的像素数据。
	*			将根据网格和影像各自的空间参考进行坐标转换.
	*			的地理坐标是一致的.
	* @author	yimin.liu@unispace-x.com
	* @date		2017/08/08
	* @param	dsetid		数据集ID.
	* @param	img_spr		影像的坐标参考.
	* @param	gid			取图的网格.
	* @param	spr			网格的坐标参考.
	* @param	[out]outBuf	传出影像像素数据保存的buffer.
	* @param	outW		设置取图输出宽.
	* @param	outH		设置取图输出高.
	* @param	[out]outUV	传出取出的位置的中网格的UV.
	* @param	doPerspectiveTransform 是否进行透视变换(重采样)
	* @param	rband		Red波段序号.
	* @param	gband		Green波段序号.
	* @param	bband		Blue波段序号.
	* @param	invalid_rgb	无效值设置.
	* @return	读取成功返回保存到缓冲区的数据量(字节数),失败返回一个负数.
	* 			|返回值|含义|
	* 			|:---|:---|
	* 			|-1|影像列表为空|
	* 			|-2|outBuf或outUV为空|
	* 			|-3|outW或outH无效|
	* 			|-4|网格无效|
	* 			|-5|读取数据返回错误|
	*			|-6|计算出的UV无效|
	*			|-7|数据集影像或者网格的空间参考无效|
	*			|-8|网格占数据集金字塔比重太小，请从原始影像读取数据|
	*************************************************************************/
	_US_DATA_PROCESS_DLL int64_t get_grid_dataset_rgba_buffer(
		const us_uuid& dsetid,
		const us_spatial_reference& img_spr,
		const unispace::us_grid_id& gid,
		const us_spatial_reference& grid_spr,
		std::vector<uint8_t>* outBuf, int* inOutW, int* inOutH,
		unispace::us_basic_uv* outUV,
		bool doPerspectiveTransform,
		int rband = 3, int gband = 2, int bband = 1,
		uint32_t invalid_rgb = 0);

	_US_DATA_PROCESS_DLL int64_t get_grid_dataset_rgba_buffer_aux(
		const us_uuid& dsetid,
		const unispace::us_vec2<double>* gridGeoCorner,
		std::vector<uint8_t>* outBuf, int* inOutW, int* inOutH,
		unispace::us_basic_uv* outUV,
		bool doPerspectiveTransform,
		int rband = 3, int gband = 2, int bband = 1,
		uint32_t invalid_rgb = 0);

}
#endif //!__US_DATASET_CACHE_HPP__
