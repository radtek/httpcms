#ifndef __US_TILE_WATERMARK_HPP__
#define __US_TILE_WATERMARK_HPP__

#include "us_data_process.hpp"

namespace unispace
{

    // ********************************************************************************
    /// <summary>
    /// 给256*256的RGBA瓦片添加内部水印.
    /// </summary>
    /// <param name="tilebuffer">RGBA瓦片数据，必须是256*256大小的</param>
    /// <created>solym@sohu.com,2018/1/31</created>
    // ********************************************************************************
    _US_DATA_PROCESS_DLL void us_rabg_tile_add_internal_watermark_256(std::vector<uint8_t>& tilebuffer);

}
#endif // !__US_TILE_WATERMARK_HPP__
