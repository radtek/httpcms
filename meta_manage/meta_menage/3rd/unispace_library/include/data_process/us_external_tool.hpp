#ifndef __US_EXTERNAL_TOOL_HPP__
#define __US_EXTERNAL_TOOL_HPP__

#ifndef __US_OPERATOR_RESULT_HPP__
#include <util/common/us_operator_result.hpp>
#endif

#ifndef __US_DATA_PROCESS_HPP__
#include <data_process/us_data_process.hpp>
#endif

namespace unispace
{
	/// ********************************************************************************
	/// <summary>
	/// 执行外部程序进行相关的操作.
	/// </summary>
	/// <param name="toolname">外部工具名称</param>
	/// <param name="toolparam">用于传递给外部工具的参数，使用JSON字符串格式</param>
	/// <param name="out_result">外部工具执行的结果返回，使用JSON字符串格式</param>
	/// <param name="pfnProgress)">进度处理回调函数</param>
	/// <param name="pfnProgressArg">进度处理函数参数</param>
	/// <returns>操作结果</returns>
	/// <created>solym@sohu.com,2018/8/23</created>
	/// ********************************************************************************
	_US_DATA_PROCESS_DLL us_operator_result us_exec_external_tool(
		const std::string& toolname, const std::string& param, std::string* out_result,
		int(*pfnProgress)(double, const char*, void*), void* pfnProgressArg);
}


#endif // !__US_EXTERNAL_TOOL_HPP__
