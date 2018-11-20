#ifndef __US_EPSG_COMPACT_DATABASE_HPP__
#define __US_EPSG_COMPACT_DATABASE_HPP__

#ifndef __US_GEO_RECT_HPP__
#include "util/math/us_geo_rect.hpp"
#endif


#ifndef __US_GEO_COORD_TRANSFORM_HPP__
#include "us_geo_coord_transform.hpp"
#endif

namespace unispace
{
	class us_epsg_coord_system_info {
	public:
		uint32_t    m_epsg_code;   ///< EPSG代码
		Point2lf    m_center;      ///< 中心点坐标(可能是投影，也可能是经纬度)
		us_geo_rect m_wgs84_bound; ///< WGS84经纬度范围
		us_geo_rect m_proj_bound;  ///< 投影范围(非投影坐标系此值无效)
		std::string m_gml_code;    ///< GML描述代码
		std::string m_wkt_code;    ///< OGC WKT描述代码
	};

	/// ********************************************************************************
	/// <summary>
	/// 查询EPSG坐标系统定义相关信息.
	/// </summary>
	/// <param name="epsgcode">EPSG代号</param>
	/// <param name="is_ok">传出是否成功找到（可以为NULL）</param>
	/// <returns>EPSG坐标系信息对象，如未找到，对象的EPSG代号为0</returns>
	/// <created>solym@sohu.com,2018/9/20</created>
	/// ********************************************************************************
	_US_GEO_COORD_TRANSFORM_DLL const us_epsg_coord_system_info&
		us_epsg_coord_system_query(uint32_t epsgcode,bool* is_ok);
}


#endif // !__US_EPSG_COMPACT_DATABASE_HPP__
