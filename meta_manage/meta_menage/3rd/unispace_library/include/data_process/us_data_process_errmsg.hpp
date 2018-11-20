#ifndef __US_DATA_PROCESS_ERRMSG_HPP__
#define __US_DATA_PROCESS_ERRMSG_HPP__


namespace unispace
{
	inline const char* get_errmsg_get_grid_image_rgba_buffer(int64_t errcode)
	{
		switch(errcode){
			case -1: return u8"影像列表为空";
			case -2: return u8"outBuf或outUV为空";
			case -3: return u8"outW或outH无效";
			case -4: return u8"网格无效";
			case -5: return u8"读取数据返回错误";
			case -6: return u8"计算出的UV无效";
			case -7: return u8"数据集影像或者网格的空间参考无效";
			default: return u8"未知错误";
		}
	}

	inline const char* get_errmsg_get_grid_dataset_rgba_buffer(int64_t errcode)
	{
		switch(errcode){
			case -1: return u8"影像列表为空";
			case -2: return u8"outBuf或outUV为空";
			case -3: return u8"outW或outH无效";
			case -4: return u8"网格无效";
			case -5: return u8"读取数据返回错误";
			case -6: return u8"计算出的UV无效";
			case -7: return u8"数据集影像或者网格的空间参考无效";
			case -8: return u8"网格占数据集金字塔比重太小，请从原始影像读取数据";
			default: return u8"未知错误";
		}
	}
	
	inline const char* get_errmsg_image_polygon_clip(int64_t errcode)
	{
		switch(errcode){
			case -1: return u8"裁剪信息无效";
			case -2: return u8"有的影像数据不正确";
			case -3: return u8"ChunkAndWarpImage失败效";
			case -4: return u8"创建输出文件失败";
			case -5: return u8"创建WarpOptions失败";
			case -6: return u8"打开输入影像文件失败";
			case -7: return u8"实际影像文件波段数不匹配";
			case -8: return u8"创建坐标转换参数失败";
			default: return u8"未知错误";
		}
	}
}

#endif //!__US_DATA_PROCESS_ERRMSG_HPP__
