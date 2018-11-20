#ifndef __US_IMAGE_CACHE_HPP__
#define __US_IMAGE_CACHE_HPP__

#ifndef __US_UUID_HPP__
#include <util/uuid/us_uuid.hpp>
#endif

#ifndef __US_DATA_PROCESS_HPP__
#include "us_data_process.hpp"
#endif

namespace unispace
{
	class us_image_basic_meta;

	/**********************************************************************//**
	 * @brief	创建影像金字塔.
	 *			1/(4^n)抽稀至宽或者高小于512截止(n为1,2,3,...).<br>
	 *			输出的金字塔影像放置在 pyramid_root_dir 目录下，以影像URL路径为
	 *			相同路径的子目录中.<br>
	 *			如果pyramid_root_dir字符串为空，则使用配置文件中指定的金字塔根路径.
	 *			如果其不为空，则必须使用'/'结尾.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2016/6/7
	 * @modify	2018/7/12
	 * @param [in,out]	in_out_meta	要创建金字塔的影像元信息.
	 * @param			fmt			输出影像格式(弃用，仅使用gtiff).
	 * @param	pyramid_root_dir	输出金字塔的根路径(弃用).
	 * @param	replace				是否替换已有金字塔.
	 *************************************************************************/
	_US_DATA_PROCESS_DLL bool creat_image_pyramid(us_image_basic_meta* in_out_meta,
												  us_image::IMAGE_FORMAT fmt,
												  const us_ustring& pyramid_root_dir,
												  bool replace = true);

	/**********************************************************************//**
	 * @brief	删除影像金字塔.
	 *			如果pyramid_root_dir字符串为空，则使用配置文件中指定的金字塔根路径.
	 *			如果其不为空，则必须使用'/'结尾.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2016/8/10
	 * @param [in]	uuid				影像的uuid.
	 * @param [in]	pyramid_root_dir	金字塔根目录.
	 * @return		成功返回true,失败返回false.
	 ***********************************************************************/
	_US_DATA_PROCESS_DLL bool remove_image_pyramid(const us_uuid& uuid,
												   const us_ustring& pyramid_root_dir);


	/**********************************************************************//**
	 * @brief	创建影像金字塔.
	 *			1/(4^n)抽稀至宽或者高小于512截止(n为1,2,3,...).<br>
	 *			输出的金字塔影像放置在 pyramid_root_dir 目录下，以影像的原本路径
	 *          作为子级的目录中.金字塔文件仅支持GeoTiff格式.<br>
	 *			如果pyramid_root_dir字符串为空，则使用配置文件中指定的金字塔根路径.
	 *			如果其不为空，则必须使用'/'结尾.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2017/10/26
	 * @modify  2018/7/12
	 * @param 	imageurl	要创建金字塔的影像路径.
	 * @param	pyramid_root_dir	输出金字塔的根路径.
	 * @param	pfnProgress			进度处理回调函数.
	 * @param	pProgressArg		进度处理附加参数.
	 * @param	replace				是否替换已有金字塔.
	 *************************************************************************/
	_US_DATA_PROCESS_DLL bool creat_image_pyramid(
        const us_ustring& imageurl, const us_ustring& pyramid_root_dir,
        int(*pfnProgress)(double, const char*, void*), void* pProgressArg,
		bool replace = true);

	/**********************************************************************//**
	 * @brief	删除影像金字塔.
	 *			如果pyramid_root_dir字符串为空，则使用配置文件中指定的金字塔根路径.
	 *			如果其不为空，则必须使用'/'结尾.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2017/10/20
	 * @param [in]	imageurl			影像的路径.
	 * @param [in]	pyramid_root_dir	金字塔根目录.
	 * @return		成功返回true,失败返回false.
	 ***********************************************************************/
	_US_DATA_PROCESS_DLL bool remove_image_pyramid(
        const us_ustring& imageurl, const us_ustring& pyramid_root_dir);

    /**********************************************************************//**
	 * @brief	获取影像金字塔文件所在目录.
	 *			如果pyramid_root_dir字符串为空，则使用配置文件中指定的金字塔根路径.
	 *			如果其不为空，则必须使用'/'结尾.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2018/01/03
	 * @param [in]	imageurl			影像的路径.
	 * @param [in]	pyramid_root_dir	金字塔根目录.
	 * @return		返回影像金字塔文件所在目录路径(无论存在与否).
	 ***********************************************************************/
	_US_DATA_PROCESS_DLL us_ustring get_image_pyramid_dir(
        const us_ustring& imageurl, const us_ustring& pyramid_root_dir);

	/**********************************************************************//**
	 * @brief	读取影像元数据.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2016/3/23
	 * @param	path			影像路径(统一后的路径).
	 * @param	[in,out]		传入传出影像meta信息.
	 * @param	update_uuid	    为true时更新uuid.
	 * @return	成功返回true，失败返回false.
	 *************************************************************************/

}
#endif //!__US_IMAGE_CACHE_HPP__
