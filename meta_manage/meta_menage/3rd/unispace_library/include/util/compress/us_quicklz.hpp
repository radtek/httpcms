#ifndef __US_QUICKLZ_HPP__
#ifndef __US_UTIL_HPP__
#include "../us_util.hpp"
#endif

namespace unispace
{
	class _US_UTIL_DLL us_quick_lz {
	public:
		/// ********************************************************************************
		/// <summary>
		/// 使用quicklz算法压缩数据.
		/// </summary>
		/// <param name="src_data">待压缩数据首地址</param>
		/// <param name="src_len">待压缩数据长度</param>
		/// <returns>成功返回压缩后数据，失败结果为empty</returns>
		/// <created>solym@sohu.com,2018/9/20</created>
		/// ********************************************************************************
		static std::string compress(const void* src_data, size_t src_len);
		/// ********************************************************************************
		/// <summary>
		/// 使用quicklz算法压缩数据.
		/// </summary>
		/// <param name="src_data">待压缩数据首地址</param>
		/// <param name="src_len">待压缩数据长度</param>
		/// <param name="out_qlz">传出压缩后数据</param>
		/// <returns>成功返回true，失败返回false</returns>
		/// <created>solym@sohu.com,2018/9/20</created>
		/// ********************************************************************************
		static bool compress(const void* src_data, size_t src_len,std::string* out_qlz);

		/// ********************************************************************************
		/// <summary>
		/// 解压使用quicklz算法压缩的数据.
		/// </summary>
		/// <param name="qlz_data">压缩的数据首地址</param>
		/// <param name="qlz_len">压缩数据的长度</param>
		/// <returns>成功返回解压后的数据，失败结果为empty</returns>
		/// <created>solym@sohu.com,2018/9/20</created>
		/// ********************************************************************************
		static std::string decompress(const void* qlz_data, size_t qlz_len);
		/// ********************************************************************************
		/// <summary>
		/// 解压使用quicklz算法压缩的数据.
		/// </summary>
		/// <param name="qlz_data">压缩的数据首地址</param>
		/// <param name="qlz_len">压缩数据的长度</param>
		/// <param name="out_data">传出解压后的数据</param>
		/// <returns>成功返回true，失败返回false</returns>
		/// <created>solym@sohu.com,2018/9/20</created>
		/// ********************************************************************************
		static bool decompress(const void* qlz_data, size_t qlz_len, std::string* out_data);
	};
}


#endif // !__US_QUICKLZ_HPP__
