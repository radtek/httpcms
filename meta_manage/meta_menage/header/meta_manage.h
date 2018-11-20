#pragma once
#ifndef __META_MANAGE_H_
#define __META_MANAGE_H_

#if defined _WIN32 || defined __CYGWIN__
#define FUNC_EXPORT __declspec(dllexport)
#else
#define FUNC_EXPORT
#endif //_WIN32 || __CYGWIN__

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
 
	namespace meta_manage
	{

		/**********************************************************************//**
		* @brief	获取数据管理器.
		* @param	configJson	连接数据库的配置信息
		* @return	非空	成功.
		*			NULL	失败.
		*************************************************************************/
		FUNC_EXPORT void* get_manager(const char* configJson);


		/**********************************************************************//**
		* @brief	读取所有image元数据.
		* @param	pMng		数据管理器.
		*			out_count	元数据数据个数
		* @return	非空	成功.
		*			NULL	失败. 失败信息可通过getError()获取
		*************************************************************************/
		FUNC_EXPORT const char* get_img_meta(const void* pMng, long int* out_count);

		/**********************************************************************//**
		* @brief	image元数据个数.
		* @param	pMng		数据管理器.
		* @return	返回元数据个数.
		*			<0		失败. 失败信息可通过getError()获取
		*************************************************************************/
		FUNC_EXPORT long int get_img_count(const void* pMng);

		/**********************************************************************//**
		* @brief	读取所有image元数据所有的id和url.
		* @param	pMng		数据管理器.
		*			from		开始读取的元数据项索引
		*			size		读取条数
		* @return	非空	成功.
		*			NULL	失败. 失败信息可通过getError()获取
		*************************************************************************/
		FUNC_EXPORT const char* get_img_idurls(const void* pMng, long int from, long int size);

		/**********************************************************************//**
		* @brief	读取一个image元数据详细信息.
		* @param	pMng		数据管理器.
		*			id			image的id
		* @return	非空	成功.
		*			NULL	失败. 失败信息可通过getError()获取
		*************************************************************************/
		FUNC_EXPORT const char* get_aimg_meta(const void* pMng, const char* id);

		/**********************************************************************//**
		* @brief	更新image元数据.
		* @param	pMng		数据管理器.
		*			updateJson	更新image的json字符串.
		*			sJson		返回的失败信息
		* @return	<0  失败. 失败信息可通过getError()获取
		*			0	成功.
		*************************************************************************/
		FUNC_EXPORT int update_img_meta(const void* pMng, const char* updateJson);



		/**********************************************************************//**
		* @brief	读取dataset元数据.
		* @param	pMng		数据管理器.
		*			len			获取的数据长度
		* @return	非空	成功.
		*			NULL	失败. 失败信息可通过getError()获取
		*************************************************************************/
		FUNC_EXPORT const char* get_dataset_meta(const void* pMng, int* len);

		/**********************************************************************//**
		* @brief	dataset元数据个数.
		* @param	pMng		数据管理器.
		* @return	返回元数据个数.
		*			<0		失败. 失败信息可通过getError()获取
		*************************************************************************/
		FUNC_EXPORT long int get_dataset_count(const void* pMng);

		/**********************************************************************//**
		* @brief	读取所有dataset元数据所有的id和name.
		* @param	pMng		数据管理器.
		*			from		开始读取的元数据项索引
		*			size		读取条数
		* @return	非空	成功.
		*			NULL	失败. 失败信息可通过getError()获取
		*************************************************************************/
		FUNC_EXPORT const char* get_dataset_idnames(const void* pMng, long int from, long int size);

		/**********************************************************************//**
		* @brief	读取一个dataset元数据详细信息.
		* @param	pMng		数据管理器.
		*			id			dataset的id
		* @return	非空	成功.
		*			NULL	失败. 失败信息可通过getError()获取
		*************************************************************************/
		FUNC_EXPORT const char* get_adataset_meta(const void* pMng, const char* id);

		/**********************************************************************//**
		* @brief	更新dataset元数据.
		* @param	pMng		数据管理器.
		*			updateJson	更新dataset的json字符串.
		*			sJson		返回的失败信息
		* @return	<0  失败.失败信息可通过getError()获取
		*			0	成功.
		*************************************************************************/
		FUNC_EXPORT int update_dataset_meta(const void* pMng, const char* updateJson);




		/**********************************************************************//**
		* @brief	获取wmts元数据.
		* @param	pMng		数据管理器.
		*			len			获取的数据长度
		* @return	非空	成功.
		*			NULL	失败. 失败信息可通过getError()获取
		*************************************************************************/
		FUNC_EXPORT const char* get_wmts_meta(const void* pMng, int* len);

		/**********************************************************************//**
		* @brief	wmts元数据个数.
		* @param	pMng		数据管理器.
		* @return	返回元数据个数.
		*			<0		失败. 失败信息可通过getError()获取
		*************************************************************************/
		FUNC_EXPORT long int get_wmts_count(const void* pMng);

		/**********************************************************************//**
		* @brief	读取所有dataset元数据所有的id和layer.
		* @param	pMng		数据管理器.
		*			from		开始读取的元数据项索引
		*			size		读取条数
		* @return	非空	成功.
		*			NULL	失败. 失败信息可通过getError()获取
		*************************************************************************/
		FUNC_EXPORT const char* get_wmts_idlayers(const void* pMng, long int from, long int size);

		/**********************************************************************//**
		* @brief	读取一个wmts元数据详细信息.
		* @param	pMng		数据管理器.
		*			id			wmts的id
		* @return	非空	成功.
		*			NULL	失败. 失败信息可通过getError()获取
		*************************************************************************/
		FUNC_EXPORT const char* get_awmts_meta(const void* pMng, const char* id);

		/**********************************************************************//**
		* @brief	更新wmts元数据.
		* @param	pMng		数据管理器.
		*			updateJson	更新wmts的json字符串.
		*			sJson		返回的失败信息
		* @return	<0  失败.
		*			0	成功.
		*************************************************************************/
		FUNC_EXPORT int update_wmts_meta(const void* pMng, const char* updateJson);



		/**********************************************************************//**
		* @brief	获取失败信息.
		* @param	
		* @return	失败信息
		*************************************************************************/
		FUNC_EXPORT const char* getError();



		/**********************************************************************//**
		* @brief	销毁manager.
		* @param	pMng	数据管理器.
		* @return	
		*************************************************************************/
		FUNC_EXPORT void destroy_manager(const void* pMng);
	}


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // !__META_MANAGE_H_
