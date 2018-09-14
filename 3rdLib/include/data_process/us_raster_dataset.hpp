#ifndef __US_RASTER_DATASET_HPP__
#define __US_RASTER_DATASET_HPP__

#ifndef __US_STRING_HPP__
#include <util/common/us_string.hpp>
#endif // !__US_STRING_HPP__

#ifndef __US_GEO_RECT_HPP__
#include "util/math/us_geo_rect.hpp"
#endif
#ifndef __US_OPERATOR_RESULT_HPP__
#include "util/common/us_operator_result.hpp"
#endif

#ifndef __US_DATA_PROCESS_HPP__
#include "us_data_process.hpp"
#endif

#include <memory>


namespace unispace
{
#if defined(_MSC_VER) && _MSC_VER < 1900
#define us_raster_dataset_impl int
#else
	class us_raster_dataset_impl;
#endif

	class us_image_basic_meta;

	class _US_DATA_PROCESS_DLL us_raster_dataset {
		std::shared_ptr<us_raster_dataset_impl> m_impl;
	public:
		explicit us_raster_dataset();
		~us_raster_dataset();
		
		// Ҫôֻ����
		us_operator_result open(const us_ustring& path);
		// Ҫôֻ����д��
		us_operator_result save(us_ustring save_as_path);

		bool add_image(const us_image_basic_meta& imagemeta);
		bool add_image(const std::vector<us_image_basic_meta>& imagemetas);
		bool set_image(const std::vector<us_image_basic_meta>& imagemetas);


		void set_desc(const unispace::us_ustring& desc);
		void set_rgb_band(int rband, int gband, int bband);
		void set_invalid_rgb(uint32_t inrgb);
		void set_resolution(double resh, double resv);
		void set_spatial_reference(const unispace::us_uuid& sprid);

	public:
		/**********************************************************************//**
		 * @brief	��ȡ�߼�ͼ����(����������).
		 * @author	yimin.liu@unispace-x.com
		 * @date	2018/6/6
		 * @return	�߼�ͼ�����������.
		 *************************************************************************/
		int get_width()const;

		/**********************************************************************//**
		 * @brief	��ȡ�߼�ͼ��߶�(����������).
		 * @author	yimin.liu@unispace-x.com
		 * @date	2018/6/6
		 * @return	�߼�ͼ������������.
		 *************************************************************************/
		int get_height()const;

		/**********************************************************************//**
		 * @brief	��ȡӰ�񲨶���.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/4/5
		 * @return	������.
		 *************************************************************************/
		int get_band_count()const;

		/**********************************************************************//**
		 * @brief	��ȡӰ���������ͱ��.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/4/13
		 * @return	��ȡ�ɹ����������������ͱ�ţ�ʧ��(��ȡ����ʧ��)����-1.
		 * 			|��GDAL�ж�Ӧ����������|����|
		 * 			|:---|:---|
		 *			|GDT_Unknown  = 0	|	δ֪��δָ������|
		 *			|GDT_Byte     = 1	|	8bit�޷�������|
		 *			|GDT_UInt16   = 2	|	16bit�޷�������|
		 *			|GDT_Int16    = 3	|	16bit�з�������|
		 *			|GDT_UInt32   = 4	|	32bit�޷�������|
		 *			|GDT_Int32    = 5	|	32bit�з�������|
		 *			|GDT_Float32  = 6	|	32bit�з��Ÿ�����|
		 *			|GDT_Float64  = 7	|	64bit�з��Ÿ�����|
		 *			|GDT_CInt16   = 8	|	16bit�з������͸���|
		 *			|GDT_CInt32   = 9	|	32bit�з������͸���|
		 *			|GDT_CFloat32 = 10	|	32bit�з��Ÿ����͸���|
		 *			|GDT_CFloat64 = 11	|	64bit�з��Ÿ����͸���|
		 *			|GDT_TypeCount = 12	|	������ͱ�� + 1 |
		 *************************************************************************/
		int get_data_type()const;

		/**********************************************************************//**
		 * @brief	��ȡ�߼�Ӱ��ķ���任����.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/4/27
		 * @param	[out] outTransform ��������任����.
		 * @return	�ɷ���true.
		 *************************************************************************/
		//bool get_geo_transfrom(double* outTransform)const;

		/**********************************************************************//**
		 * @brief	��ȡʵ�ʹ�����Ӱ��ķ���任����.
		 *			ע�⣬����ͶӰ�ο���������������ô˽ӿڵĶ���һ��.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2017/8/8
		 * @return	�ɹ�����Ӱ��ͶӰ�ο�ָ��(const char*),ʧ�ܷ���NULL.
		 *************************************************************************/
		//const char* get_projection_ref()const;

		/**********************************************************************//**
		 * @brief	��ȡ�߼�ͼ��ĵ���Χ.
		 * @author	yimin.liu@unispace-x.com
		 * @date	2016/4/27
		 * @return	�߼�ͼ��ĵ���Χ.
		 *************************************************************************/
		us_geo_rect get_geo_rect()const;

		int64_t get_rabg_buffer(
			const unispace::us_vec2<double>* gridGeoCorner,
			void* pData, int nBufXSize, int nBufYSize);
	};

}
#endif // !__US_RASTER_DATASET_HPP__
