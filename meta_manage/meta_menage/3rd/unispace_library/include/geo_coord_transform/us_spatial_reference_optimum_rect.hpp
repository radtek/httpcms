#ifndef __US_SPATIAL_REFERENCE_OPTIMUM_RECT_HPP__
#define __US_SPATIAL_REFERENCE_OPTIMUM_RECT_HPP__

#ifndef __US_GEO_RECT_HPP__
#include "util/math/us_geo_rect.hpp"
#endif
#ifndef __US_UUID_HPP__
#include "util/uuid/us_uuid.hpp"
#endif

#ifndef __US_GEO_COORD_TRANSFORM_HPP__
#include "us_geo_coord_transform.hpp"
#endif

namespace unispace
{
	// ********************************************************************************
	/// <summary>
	/// 获取坐标系最适外包框范围(仅支持北京54、西安80、国家2000高斯投影坐标系)
	/// </summary>
	/// <param name="sprid">空间参考ID</param>
	/// <param name="outlatlng">是否返回经纬度范围</param>
	/// <returns>外包框范围</returns>
	/// <created>solym@sohu.com,2018/7/31</created>
	// ********************************************************************************
	_US_GEO_COORD_TRANSFORM_DLL us_geo_rect
		us_spatial_reference_optimum_rect(const us_uuid& sprid, bool outlatlng);
}

#endif //!__US_SPATIAL_REFERENCE_OPTIMUM_RECT_HPP__