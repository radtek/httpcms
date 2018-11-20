#ifndef __US_RASTER_MAP_PACKAGE_HPP__
#define __US_RASTER_MAP_PACKAGE_HPP__

#ifndef __US_IMAGE_BASIC_META_HPP__
#include <metadata/us_image_basic_meta.hpp>
#endif
#ifndef __US_DATA_PROCESS_HPP__
#include "us_data_process.hpp"
#endif
namespace unispace
{

class _US_DATA_PROCESS_DLL us_raster_map_package
{
	void* m_rmp_file;
	/* 禁止拷贝(每次使用的时候直接去打开，无需考虑效率文件) */
	us_raster_map_package(const us_raster_map_package&);
	void operator=(us_raster_map_package&);
public:
	us_raster_map_package();
	us_raster_map_package(const us_ustring& path);
	~us_raster_map_package();

	/**********************************************************************//**
	* @brief	打开 rmp 压缩包文件(用于读取RGBA数据).
	* @author	yimin.liu@unispace-x.com
	* @date     2018/4/12
	* @return  成功返回true，失败返回false.
	*************************************************************************/
	bool open(const us_ustring& path);

	inline bool is_open() const
	{
		return m_rmp_file != NULL;
	}
	inline operator bool()
	{
		return is_open();
	}

	double get_resolution_h() const;
	double get_resolution_v() const;
	int32_t get_pixel_h() const;
	int32_t get_pixel_v() const;
	bool get_geo_transform(double* pdfGeoTransform) const;
	us_geo_rect get_geo_rect() const;

	/**********************************************************************//**
		* @brief	从 rmp 压缩包读取RGBA数据.
		* @author	yimin.liu@unispace-x.com
		* @date	2018/4/12
		* @param	nXOff	读取窗口横向起点位置.
		* @param   nYOff	读取窗口纵向起点位置.
		* @param   nXSize  读取窗口横向像素数.
		* @param   nYSize  读取窗口纵向像素数.
		* @param   pData	读取数据输出缓冲区.
		* @param   nBufXSize  输出窗口横向像素数.
		* @param   nBufYSize  输出窗口纵向像素数.
		* @return  成功返回正值，失败返回负值.
		*************************************************************************/
	int64_t read_rgba_data(
		int nXOff, int nYOff, int nXSize, int nYSize,
		void *pData, int nBufXSize, int nBufYSize) const;

	/**********************************************************************//**
	* @brief	从 rmp 压缩包读取RGBA数据.
	* @author	yimin.liu@unispace-x.com
	* @date		2018/4/12
	* @param	geo_corner	读取窗口四个角点地理坐标(左上、右上、右下、左下顺序)
	* @param    pData	读取数据输出缓冲区.
	* @param    nBufXSize  输出窗口横向像素数.
	* @param    nBufYSize  输出窗口纵向像素数.
	* @return  成功返回正值，失败返回负值.
	*************************************************************************/
	int64_t read_rgba_data(
		const unispace::us_vec2<double> *geo_corner,
		void *pData, int nBufXSize, int nBufYSize);
public:
	/**********************************************************************//**
	* @brief	创建从 rmp 压缩包.
	*			设置分辨率模式时，低分辨率值模式得到文件最大(分辨率值越低，单像素地理范围越小)
	*			默认按照最大分辨率值模式进行压缩，得到的文件最小。如果使用用户传入的值，则
	*			传入值不应该小于影像列表中影像分辨率的最小值.
	* @author	yimin.liu@unispace-x.com
	* @date		2018/4/12
	* @param	out_path	rmp压缩包保存位置
	* @param    image_list	用于创建压缩包的影像列表.
	 * @param	r_band	 作为红色波段的波段序号.
	 * @param	g_band	 作为绿色波段的波段序号.
	 * @param	b_band	 作为蓝色波段的波段序号.
	 * @param	invalid_rgb  无效的RGB值(>0x01000000则使用).
	 * @param	resolution_mode  分辨率模式(1:低分辨率值模式，2平均分辨率值模式，3用户传入分辨率值模式，默认高分辨率值模式).
	 * @param	resolution_h	 rmp压缩包横向分辨率(仅在resolution_mode为3时使用).
	 * @param	resolution_v	 rmp压缩包纵向分辨率(仅在resolution_mode为3时使用).
	 * @param	pfnProgress  进度处理回调函数.
	 * @param	pProgressArg 进度处理函数附加参数.
	* @return  成功返回正值，失败返回负值.
	*************************************************************************/
    static int create_raster_map_package(
         unispace::us_ustring out_path,
         const std::vector<unispace::us_image_basic_meta> &image_list,
         int r_band, int g_band, int b_band, uint32_t invalid_rgb,
         int resolution_mode, double resolution_h, double resolution_v,
		 float quality_factor,
         int (*pfnProgress)(double, const char *, void *), void *pProgressArg);
};

}

#endif // __US_RASTER_MAP_PACKAGE_HPP__
