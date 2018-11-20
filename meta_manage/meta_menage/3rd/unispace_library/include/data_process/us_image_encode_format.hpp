#ifndef __US_IMAGE_ENCODE_FORMAT_HPP__
#define __US_IMAGE_ENCODE_FORMAT_HPP__

#ifndef __US_DATA_PROCESS_HPP__
#include "us_data_process.hpp"
#endif

#include <vector>

namespace unispace
{
	/**********************************************************************//**
	 * @brief	编码RGBA8888排列的的影像像素数据为PNG格式数据.
	 *			当前只支持RGBA8888排列的像素数据，不支持其他形式.
	 *			输出的PNG数据为无损压缩形式.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2016/12/08
	 * @param	pRgbaData	像素数据地址.
	 * @param	nWidth		像素数据的横向像素数.
	 * @param	nHeight		像素数据的纵向像素数.
	 * @param	pPngBuffer	目标png数据缓存区.
	 * @return	成功返回true,失败返回false.
	 *************************************************************************/
	_US_DATA_PROCESS_DLL bool encode_rgba8888_to_png32(
		const void * pRgbaData, int nWidth, int nHeight,
		std::vector<uint8_t>* pPngBuffer);

	/**********************************************************************//**
	 * @brief	编码RGBA8888排列的的影像像素数据为JPEG格式数据.
	 *			当前只支持RGBA8888排列的像素数据，不支持其他形式.
	 *			输出的JPEG数据为有损压缩形式，压缩质量为75.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2018/06/04
	 * @param	pRgbaData	像素数据地址.
	 * @param	nWidth		像素数据的横向像素数.
	 * @param	nHeight		像素数据的纵向像素数.
	 * @param	pJpegBuffer	目标jpeg数据缓存区.
	 * @param	quality		编码采用的质量参数(50-99).
	 * @return	成功返回true,失败返回false.
	 *************************************************************************/
	_US_DATA_PROCESS_DLL bool encode_rgba8888_to_jpeg(
		const void * pRgbaData, int nWidth, int nHeight,
		std::vector<uint8_t>* pJpegBuffer, float quality = 75.0);

	
	/**********************************************************************//**
	 * @brief	编码RGBA8888排列的的影像像素数据为WebP格式数据.
	 *			当前只支持RGBA8888排列的像素数据，不支持其他形式.
	 *			输出的WebP数据为有损压缩形式，压缩质量为75.
	 * @author	yimin.liu@unispace-x.com
	 * @date	2018/06/04
	 * @param	pRgbaData	像素数据地址.
	 * @param	nWidth		像素数据的横向像素数.
	 * @param	nHeight		像素数据的纵向像素数.
	 * @param	pWebpBuffer	目标Webp数据缓存区.
	 * @param	quality		编码采用的质量参数(50-99).
	 * @return	成功返回true,失败返回false.
	 *************************************************************************/
	_US_DATA_PROCESS_DLL bool encode_rgba8888_to_webp(
		const void * pRgbaData, int nWidth, int nHeight,
		std::vector<uint8_t>* pWebpBuffer, float quality = 75.0);
}

#endif // !__US_IMAGE_ENCODE_FORMAT_HPP__
