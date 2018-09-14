#ifndef __US_WMTS_TILE_REGULATION_HPP__
#define __US_WMTS_TILE_REGULATION_HPP__

#ifndef __US_GEO_RECT_HPP__
#include "util/math/us_geo_rect.hpp"
#endif
#ifndef __US_STRING_HPP__
#include "util/common/us_string.hpp"
#endif

#ifndef __US_METADATA_HPP__
#include "us_metadata.hpp"
#endif

namespace unispace
{
	/**********************************************************************//**
	 * @class	us_wmts_tile_regulation
	 * @brief	WMTS瓦片规则类.
	 * 			瓦片规则用于计算瓦片数量范围，生成tile matrix描述信息等.
	 * @author	xuefeng.dai@unispace-x.com
	 * @modify	yimin.liu@unispace-x.com
	 * @date	2017/3/29
	 *************************************************************************/
	class _US_METADATA_DLL us_wmts_tile_regulation {
	public:
		enum WMTS_TYPE {
			UNISPACE_DEGREE	   = 0,  ///< 内部度网格
			UNISPACE_METER	   = 1,  ///< 内部米网格
			GOOGLE_LATLNG	   = 2,  ///< 经纬度，顶级为东西半球各一个网格
			WEB_MERCATOR       = 3,  ///< Web墨卡托，顶级一个方网格
            GA_MT_LATLNG       = 4   ///< 公安警用地理信息 地图配图规范
		};
        // DPI模式(因经纬度与米单位转换系数不同也统一转换为DPI不同)
        enum WMTS_DPI_MODE {
            DPI_ARCGIS_MODE    = 0,  ///< ArcGIS 模式 90.714
            DPI_GMAP_MODE      = 1,  ///< Google地图兼容 模式 90.815
            DPI_TIANDITU_MODE  = 2   ///< 天地图模式 96.0
        };

		WMTS_TYPE			m_type;
		double				m_scale_denom;	// 顶级比例尺参数；
		double				m_resolution;	// 顶级分辨率
		us_geo_rect			m_rect_base;	// 总范围
		us_size<int>		m_rank_base;	// 顶级块数

	public:
		/**********************************************************************//**
		* @brief	设置瓦片规则.
		* @author	xuefeng.dai@unispace-x.com
		* @modify	yimin.liu@unispace-x.com
		* @date		2017/3/29
		* @param	type	    瓦片规则.
        * @param    dpimode     DPI模式.
        * @param    tilepixel   瓦片横向像素数.
		*************************************************************************/
        void set_tile_type(WMTS_TYPE type,
            WMTS_DPI_MODE dpimode = DPI_ARCGIS_MODE, uint32_t tilepixel = 256);

		/**********************************************************************//**
		* @brief	给定Rect获取指定level下的行列范围.
		* @author	xuefeng.dai@unispace-x.com
		* @date		2017/3/29
		* @param	level	网格级别.
		* @param	row		横向偏移.
		* @param	col		纵向偏移.
		* @return	返回该级别下行列范围。rect的x为列索引，y为行索引
		*************************************************************************/
		us_rect<int> get_rank_range(int level, us_geo_rect in_rect);
		

		/**********************************************************************//**
		* @brief	判断行列在该级别是否有效.
		* @author	xuefeng.dai@unispace-x.com
		* @date		2017/3/29
		* @param	level	网格级别.
		* @param	row		横向偏移.
		* @param	col		纵向偏移.
		* @return	有效返回true.
		*************************************************************************/
		bool is_vaild_rank(int level, int row, int col, us_geo_rect in_rect);
		
		/**********************************************************************//**
		* @brief	获取TileMatrixSet瓦片矩阵集xml描述.
		* @author	xuefeng.dai@unispace-x.com
		* @modify	yimin.liu@unispace-x.com
		* @date		2017/3/29
		* @param	tms_name		tms图层名称.
		* @param	level_min		最小网格级别.
		* @param	level_max		最大网格级别.
		* @param	tile_pixel_h	瓦片横向像素数.
		* @param	tile_pixel_v	瓦片纵向像素数.
		* @return	xml字符串.
		*************************************************************************/
		us_ustring get_tile_matrix_set_xml(
			us_ustring matrix_set_identifier,us_uuid sprid,
			int level_min, int level_max, int tile_pixel_h, int tile_pixel_v);
		
	};

}
#endif //!__US_WMTS_TILE_REGULATION_HPP__