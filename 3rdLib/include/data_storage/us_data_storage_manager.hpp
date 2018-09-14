#ifndef __US_DATA_STORAGE_MANAGER_HPP__
#define __US_DATA_STORAGE_MANAGER_HPP__

#ifndef __US_STRING_HPP__
#include "util/common/us_string.hpp"
#endif

#ifndef __US_DATA_STORAGE_HPP__
#include "us_data_storage.hpp"
#endif // !__US_DATA_STORAGE_HPP__

#include <vector>
#include <map>


namespace unispace
{

	class _US_DATA_STORAGE_DLL us_data_storage_manager
	{
		us_data_storage_manager();
		~us_data_storage_manager();
		static us_data_storage_manager s_instance;
	public:
		/**********************************************************************//**
		 * @brief	获取数据存储管理器单例对象.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/12/27
		 * @return	管理器.
		 *************************************************************************/
		static us_data_storage_manager& get_instance();
	public:
		/**********************************************************************//**
		 * @brief	重新读取配置文件.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/12/27
		 * @return	读取成功返回true，失败返回false.
		 *************************************************************************/
		bool reload_config();
		
		/**********************************************************************//**
		 * @brief	保存当前配置到配置文件.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2017/11/06
		 * @return	成功返回true，失败返回false.
		 *************************************************************************/
		bool save_config();

		/**********************************************************************//**
		 * @brief	添加一个挂载目录到配置中.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2017/11/06
		 * @param	mount_point	挂载点名称.
		 * @param   target_dir  目标目录.
		 * @return	成功返回0,失败返回负值.
		 *          -1 挂载点名称不可用;
		 *          -2 目标目录不存在;
		 *          -3 写入配置文件中失败;
		 *************************************************************************/
		int mount_dir(us_ustring mount_point, us_ustring target_dir);

		/**********************************************************************//**
		 * @brief	添加多个挂载目录到配置中.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2017/11/06
		 * @param	mount_entrys 挂载项目数组.
		 * @param   out_err_msg  传出错误信息.
		 * @return	成功返回0,失败返回负值.
		 *          -1 挂载点名称不可用;
		 *          -2 目标目录不存在;
		 *          -3 写入配置文件中失败;
		 *************************************************************************/
		int mount_dir(std::map<us_ustring,us_ustring> mount_entrys, us_ustring* out_err_msg);

		/**********************************************************************//**
		 * @brief	管理器是否有效(各个路径是否都存在).
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/12/27
		 * @return	有效返回true，无效返回false.
		 *************************************************************************/
		bool is_valid();

		/**********************************************************************//**
		 * @brief	获取金字塔根目录.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/12/27
		 * @return	金字塔根目录.
		 *************************************************************************/
		const us_ustring& get_pyramid_root()const;

		/**********************************************************************//**
		 * @brief	获取热数据根目录.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/12/27
		 * @return	热数据根目录.
		 *************************************************************************/
		const us_ustring& get_hotdata_root()const;

		/**********************************************************************//**
		 * @brief	获取影像数据所有根目录.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/12/27
		 * @return	影像数据的所有根目录名称和路径映射.
		 *************************************************************************/
		const std::vector<std::pair<us_ustring, us_ustring> >& get_mnt_roots()const;

		/**********************************************************************//**
		 * @brief	获取影像数据的实际路径.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/12/27
		 * @param	image_url	影像的url
		 * @return	影像数据实际路径.
		 *************************************************************************/
		us_ustring get_image_real_path(const us_ustring& image_url)const;
		/**********************************************************************//**
		 * @brief	获取影像文件的url.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/12/27
		 * @param	abspath	影像文件的绝对路径.
		 * @return	影像文件的url.
		 *************************************************************************/
		us_ustring get_image_file_url(const us_ustring& abspath)const;

	private:
		bool		m_valid;		///< 是否有效(各个路径是否都存在)
		us_ustring	m_pyramid_root;	///< 金字塔路径
		us_ustring	m_hotdata_root;	///< 热数据路径
		std::vector<std::pair<us_ustring, us_ustring> > m_mnt_root;	///< 挂载的根目录

	};


	/////////////////////////////////////////////////////////////////////
	inline bool unispace::us_data_storage_manager::is_valid()
	{
		return m_valid;
	}
	inline const us_ustring&
		unispace::us_data_storage_manager::get_pyramid_root() const
	{
		return m_pyramid_root;
	}
	inline const us_ustring&
		unispace::us_data_storage_manager::get_hotdata_root() const
	{
		return m_hotdata_root;
	}
	inline const std::vector<std::pair<us_ustring, us_ustring> >&
		unispace::us_data_storage_manager::get_mnt_roots() const
	{
		return m_mnt_root;
	}
}

#endif // !__US_DATA_STORAGE_MANAGER_HPP__
