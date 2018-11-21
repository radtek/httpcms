#include "meta_manage.h"
#include "config.h"
#include "metadata/us_meta_json_convert.hpp"
#include "data_manager/us_image_index_list.hpp"
#include "data_manager/us_image_manager.hpp"
#include "metadata/us_image_basic_meta.hpp"
#include "data_manager/us_dataset_index_list.hpp"
#include "data_manager/us_dataset_manager.hpp"
#include "metadata/us_dataset_basic_meta.hpp"
#include "data_manager/us_wmts_index_list.hpp"
#include "data_manager/us_wmts_manager.hpp"
#include "metadata/us_wmts_basic_meta.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include <string>


static std::string strError;
static std::string sResult;

void* meta_manage::get_manager(const char* configJson)
{
	if (!meta_manage::config::init(configJson)) {
		strError = "mysql connect failed";
		return NULL;
	}

	unispace::us_data_manager* ret = unispace::us_data_manager::get_manager_by_mysql(
		meta_manage::config::get_mysql_host(), 
		meta_manage::config::get_mysql_user(),
		meta_manage::config::get_mysql_psw(),
		meta_manage::config::get_mysql_database(), 
		meta_manage::config::get_mysql_port());

	if (ret == NULL){
		strError = "mysql connect failed";
		return NULL;
	}

	return ret;
}

//获取失败信息
const char* meta_manage::getError()
{
	return strError.c_str();
}

//获取image元数据
const char* meta_manage::get_img_meta(const void* pMng, long int* out_count)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng == NULL || out_count == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng or len error\"}";
		*out_count = 0;
		return NULL;
	}

	unispace::us_image_manager* pimgMng = mng->get_image_manager();
	if (pimgMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		*out_count = 0;
		return NULL;
	}

	//获取当前数据库总的影像数
	unispace::DM_RESULT ret = pimgMng->get_image_count(out_count);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":3,\"msg\":\"get_image_count failed\"}";
		*out_count = 0;
		return NULL;
	}

	//从数据库中获取image索引
	unispace::us_image_index_list imgIdx;
	ret = pimgMng->get_image_list_by_range(0, *out_count, &imgIdx);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":4,\"msg\":\"get_image_list_by_range failed\"}";
		*out_count = 0;
		return NULL;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

	writer.StartObject();
	writer.Key("counts");
	writer.Int64(*out_count);

	writer.Key("metas");
	writer.StartArray();

	//获取数据集元数据
	for (std::vector<unispace::IMG_INDEX>::const_iterator itr = imgIdx.begin();
		itr != imgIdx.end(); ++itr)
	{
		//根据uuid获取影像元数据
		unispace::us_image_basic_meta out_meta;
		ret = pimgMng->find_image_meta(itr->uuid, &out_meta);
		if (ret != unispace::DM_RESULT::SUCCESS) {
			strError = "{\"status\":5,\"msg\":\"find_image_meta failed\"}";
			*out_count = 0;
			return NULL;
		}

		//把影像元数据写入json
		unispace::us_ustring retJson;
		if (!unispace::us_image_meta_to_json(out_meta, &retJson))
		{
			strError = "{\"status\":6,\"msg\":\"meta to json failed\"}";
			*out_count = 0;
			return NULL;
		}

		writer.RawValue(retJson.c_str(), retJson.size(), rapidjson::kObjectType);
	}

	writer.EndArray();
	writer.EndObject();

	sResult.assign(strbuf.GetString(), strbuf.GetSize());

	return sResult.c_str();
}

//iamge元数据个数
long int meta_manage::get_img_count(const void* pMng)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng error\"}";
		return -1;
	}

	unispace::us_image_manager* pimgMng = mng->get_image_manager();
	if (pimgMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		return -2;
	}

	//获取当前数据库总的影像数
	long int out_count;
	unispace::DM_RESULT ret = pimgMng->get_image_count(&out_count);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":3,\"msg\":\"get_image_count failed\"}";
		return -3;
	}

	return out_count;
}

//获取image元数据的id和url
const char* meta_manage::get_img_idurls(const void* pMng, long int from, long int size)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng error\"}";
		return NULL;
	}

	unispace::us_image_manager* pimgMng = mng->get_image_manager();
	if (pimgMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		return NULL;
	}

	//从数据库中获取image索引
	unispace::us_image_index_list imgIdx;
	unispace::DM_RESULT ret = pimgMng->get_image_list_by_range(from, size, &imgIdx);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":3,\"msg\":\"get_image_list_by_range failed\"}";
		return NULL;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	writer.StartObject();
	writer.Key("primary_keys");
	writer.StartArray();
	for (std::vector<unispace::IMG_INDEX>::const_iterator itr = imgIdx.begin();
		itr != imgIdx.end(); ++itr)
	{
		writer.StartObject();
		writer.Key("id");
		writer.String(itr->uuid.to_brief_string().c_str());
		writer.Key("str");
		writer.String(itr->url.c_str());
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();

	sResult.assign(strbuf.GetString(), strbuf.GetSize());

	return sResult.c_str();
}

//读取一个image的元数据
const char* meta_manage::get_aimg_meta(const void* pMng, const char* id)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng == NULL || id == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng or id error\"}";
		return NULL;
	}

	unispace::us_image_manager* pimgMng = mng->get_image_manager();
	if (pimgMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		return NULL;
	}

	//根据uuid获取影像元数据
	unispace::us_image_basic_meta out_meta;
	unispace::us_uuid uuid;
	if (!uuid.from_string(std::string(id, strlen(id)))) {
		strError = "{\"status\":3,\"msg\":\"param id error\"}";
		return NULL;
	}
	unispace::DM_RESULT ret = pimgMng->find_image_meta(uuid, &out_meta);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":4,\"msg\":\"find_image_meta failed\"}";
		return NULL;
	}

	//把影像元数据写入json
	unispace::us_ustring retJson;
	if (!unispace::us_image_meta_to_json(out_meta, &retJson))
	{
		strError = "{\"status\":5,\"msg\":\"meta to json failed\"}";
		return NULL;
	}

	sResult.assign(retJson.c_str(), retJson.size());
	return sResult.c_str();
}

//更新image元数据.
int meta_manage::update_img_meta(const void* pMng, const char* updateJson)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng == NULL || updateJson == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng or updateJson error\"}";
		return -1;
	}

	unispace::us_image_manager* pimgMng = mng->get_image_manager();
	if (pimgMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		return -1;
	}

	//解析json字符串
	rapidjson::Document doc;
	doc.Parse(updateJson);
	if (doc.HasParseError()) {
		strError = "{\"status\":3,\"msg\":\"param updateJson error\"}";
		return -1;
	}

	//检验参数正确性（是否存在options和update参数）
	rapidjson::Document::ConstMemberIterator iter = doc.FindMember("options");
	if (iter == doc.MemberEnd() || !iter->value.IsObject()) {
		strError = "{\"status\":4,\"\msg\":\"param updateJson_options error\"}";
		return -1;
	}
	auto optionObj = iter->value.GetObject();

	iter = doc.FindMember("update");
	if (iter == doc.MemberEnd() || !iter->value.IsObject()) {
		strError = "{\"status\":5,\"\msg\":\"param updateJson_update error\"}";
		return -1;
	}
	auto updateObj = iter->value.GetObject();

	std::vector<std::string> fails; /*修改失败的id或者url*/

	//获取要修改的id
	std::vector<unispace::us_uuid> ids;
	iter = optionObj.FindMember("ids");
	if (iter != optionObj.MemberEnd() && iter->value.IsArray()) {
		auto idAry = iter->value.GetArray();
		unispace::us_uuid tmpid;
		std::string strid;
		for (auto i = 0; i < idAry.Size(); ++i) {
			if (!idAry[i].IsString()) { continue; }
			strid.assign(idAry[i].GetString(), idAry[i].GetStringLength());
			if (!tmpid.from_string(strid)) {
				fails.push_back(strid);
				continue;
			}
			ids.push_back(tmpid);
		}
	}

	//获取要修改的url
	iter = optionObj.FindMember("urls");
	if (iter != optionObj.MemberEnd() && iter->value.IsArray()) {
		auto urlAry = iter->value.GetArray();
		unispace::us_uuid tmpid;
		for (auto i = 0; i < urlAry.Size(); ++i) {
			if (!urlAry[i].IsString()) { continue; }
			//通过url找到uuid并放在ids中
			if (pimgMng->find_image_uuid(unispace::us_ustring(urlAry[i].GetString(),
				urlAry[i].GetStringLength()), &tmpid) != unispace::DM_RESULT::SUCCESS) {
				fails.push_back(std::string(urlAry[i].GetString(),
					urlAry[i].GetStringLength()));
				continue;
			}
			//不能有重复
			if (std::find(ids.begin(), ids.end(), tmpid) != ids.end()) {
				ids.push_back(tmpid);
			}
		}//end for
	}

	//批量修改(ids和urls没有列出时，才使用批量修改)
	if (ids.empty()) {
		int from = 0;
		iter = optionObj.FindMember("from");
		if (iter != optionObj.MemberEnd() && iter->value.IsInt()) {
			from = iter->value.GetInt();
		}
		int size = 0;
		iter = optionObj.FindMember("size");
		if (iter != optionObj.MemberEnd() && iter->value.IsInt()) {
			size = iter->value.GetInt();
		}
		unispace::us_image_index_list imgList;
		pimgMng->get_image_list_by_range(from, size, &imgList);

		for (auto itr = imgList.begin(); itr != imgList.end(); ++itr) {
			ids.push_back(itr->uuid);
		}
	}

	if (ids.empty()) {
		strError = "{\"status\":6,\"msg\":\"param id or url error\"}";
		return -2;
	}

	//获取要修改的字段
	//sprid
	unispace::us_uuid sprid;
	iter = updateObj.FindMember("sprid");
	if (iter != updateObj.MemberEnd() && iter->value.IsString()) {
		sprid.from_string(iter->value.GetString());
	}

	//geo_rect
	double geo_rect[4];/*西，东，南，北*/
	iter = updateObj.FindMember("geo_rect");
	if (iter != updateObj.MemberEnd() && iter->value.IsArray()) {
		auto geoAry = iter->value.GetArray();
		if (geoAry.Size() != 4) {
			strError = "{\"status\":7,\"msg\":\"param geo_rect error\"}";
			return -3;
		}
		if (geoAry[0].IsDouble()) {
			geo_rect[0] = geoAry[0].GetDouble();
		}
		else if (geoAry[0].IsInt()) {
			geo_rect[0] = geoAry[0].GetInt();
		}

		if (geoAry[1].IsDouble()) {
			geo_rect[1] = geoAry[1].GetDouble();
		}
		else if (geoAry[1].IsInt()) {
			geo_rect[1] = geoAry[1].GetInt();
		}

		if (geoAry[2].IsDouble()) {
			geo_rect[2] = geoAry[2].GetDouble();
		}
		else if (geoAry[2].IsInt()) {
			geo_rect[2] = geoAry[2].GetInt();
		}

		if (geoAry[3].IsDouble()) {
			geo_rect[3] = geoAry[3].GetDouble();
		}
		else if (geoAry[3].IsInt()) {
			geo_rect[3] = geoAry[3].GetInt();
		}
	}

	unispace::us_image_basic_meta imgMeta;
	for (auto itr = ids.begin(); itr != ids.end(); ++itr)
	{
		//获取元数据
		if (pimgMng->find_image_meta(*itr, &imgMeta) != unispace::DM_RESULT::SUCCESS) {
			fails.push_back(itr->to_brief_string());
			continue;
		}

		//修改元数据
		imgMeta.m_spr_uuid = sprid;
		imgMeta.m_geo_rect.set_west(geo_rect[0]);
		imgMeta.m_geo_rect.set_east(geo_rect[1]);
		imgMeta.m_geo_rect.set_south(geo_rect[2]);
		imgMeta.m_geo_rect.set_north(geo_rect[3]);

		//更新image元数据
		if (pimgMng->update_image_meta(imgMeta) != unispace::DM_RESULT::SUCCESS) {
			fails.push_back(itr->to_brief_string());
			continue;
		}
	}//end for

	 //输出修改失败的id
	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	writer.StartObject();
	writer.Key("status");
	writer.Int(0);
	writer.Key("fails");
	writer.StartArray();
	for (auto itr : fails) {
		writer.String(itr.c_str(), itr.size());
	}
	writer.EndArray();
	writer.EndObject();

	strError = std::string(strbuf.GetString(), strbuf.GetSize());

	return 0;
}




//获取dataset元数据
const char* meta_manage::get_dataset_meta(const void* pMng, int* len)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng == NULL || len == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng or len error\"}";
		*len = 0;
		return NULL;
	}

	unispace::us_dataset_manager* pdsetMng = mng->get_dataset_manager();
	if (pdsetMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		*len = 0;
		return NULL;
	}

	//获取数据库中数据集的总数
	long dtsetCount;
	unispace::DM_RESULT ret = pdsetMng->get_dataset_count(&dtsetCount);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":3,\"msg\":\"get_dataset_count failed\"}";
		*len = 0;
		return NULL;
	}

	//获取数据集索引列表
	unispace::us_dataset_index_list dtsetList;
	ret = pdsetMng->get_dataset_index_list_by_range(0, dtsetCount, &dtsetList);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":4,\"msg\":\"get_dataset_index_list_by_range failed\"}";
		*len = 0;
		return NULL;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

	writer.StartObject();
	writer.Key(u8"counts");
	writer.Int64(dtsetCount);

	writer.Key(u8"metas");
	writer.StartArray();

	//获取数据集元数据
	for (std::vector<unispace::DATASET_INDEX>::const_iterator itr = dtsetList.begin();
		itr != dtsetList.end(); ++itr)
	{
		//根据uuid获取数据集元数据
		unispace::us_dataset_basic_meta dtsetMeta;
		ret = pdsetMng->find_dataset(itr->uuid, &dtsetMeta);
		if (ret != unispace::DM_RESULT::SUCCESS) {
			strError = "{\"status\":5,\"msg\":\"find_dataset failed\"}";
			*len = 0;
			return NULL;
		}

		//把影像元数据写入json
		unispace::us_ustring retJson;
		if (!unispace::us_dataset_meta_to_json(dtsetMeta, &retJson, 1))
		{
			strError = "{\"status\":6,\"msg\":\"us_dataset_meta_to_json failed\"}";
			*len = 0;
			return NULL;
		}

		writer.RawValue(retJson.c_str(), retJson.size(), rapidjson::kObjectType);
	}

	writer.EndArray();
	writer.EndObject();

	sResult.assign(strbuf.GetString(), strbuf.GetSize());

	*len = sResult.size();
	return sResult.c_str();
}

//dataset元数据个数
long int meta_manage::get_dataset_count(const void* pMng)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng error\"}";
		return -1;
	}

	unispace::us_dataset_manager* pdsetMng = mng->get_dataset_manager();
	if (pdsetMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		return -2;
	}

	//获取当前数据库总的dataset数
	long int out_count;
	unispace::DM_RESULT ret = pdsetMng->get_dataset_count(&out_count);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":3,\"msg\":\"get_dataset_count failed\"}";
		return -3;
	}

	return out_count;
}

//获取dataset元数据的id和name
const char* meta_manage::get_dataset_idnames(const void* pMng, long int from, long int size)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng error\"}";
		return NULL;
	}

	unispace::us_dataset_manager* pdsetMng = mng->get_dataset_manager();
	if (pdsetMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		return NULL;
	}

	//从数据库中获取image索引
	unispace::us_dataset_index_list dsetIdx;
	unispace::DM_RESULT ret = pdsetMng->get_dataset_index_list_by_range(from, size, &dsetIdx);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":3,\"msg\":\"get_dataset_index_list_by_range failed\"}";
		return NULL;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	writer.StartObject();
	writer.Key("primary_keys");
	writer.StartArray();

	for (std::vector<unispace::DATASET_INDEX>::const_iterator itr = dsetIdx.begin();
		itr != dsetIdx.end(); ++itr)
	{
		writer.StartObject();
		writer.Key("id");
		writer.String(itr->uuid.to_brief_string().c_str());
		writer.Key("str");
		writer.String(itr->name.c_str());
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();

	sResult.assign(strbuf.GetString(), strbuf.GetSize());

	return sResult.c_str();
}

//读取一个dataset的元数据
const char* meta_manage::get_adataset_meta(const void* pMng, const char* id)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng == NULL || id == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng or id error\"}";
		return NULL;
	}

	unispace::us_dataset_manager* pdsetMng = mng->get_dataset_manager();
	if (pdsetMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		return NULL;
	}

	//根据uuid获取dataset元数据
	unispace::us_dataset_basic_meta out_meta;
	unispace::us_uuid uuid;
	if (!uuid.from_string(std::string(id, strlen(id)))) {
		strError = "{\"status\":3,\"msg\":\"param id error\"}";
		return NULL;
	}
	unispace::DM_RESULT ret = pdsetMng->find_dataset(uuid, &out_meta);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":4,\"msg\":\"find_dataset failed\"}";
		return NULL;
	}

	//把dataset元数据写入json
	unispace::us_ustring retJson;
	if (!unispace::us_dataset_meta_to_json(out_meta, &retJson, 1))
	{
		strError = "{\"status\":5,\"msg\":\"meta to json failed\"}";
		return NULL;
	}

	sResult.assign(retJson.c_str(), retJson.size());
	return sResult.c_str();
}

//更新dataset元数据.
int meta_manage::update_dataset_meta(const void* pMng, const char* updateJson)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (pMng == NULL || updateJson == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng or updateJson error\"}";
		return -1;
	}

	unispace::us_dataset_manager* pdsetMng = mng->get_dataset_manager();
	if (pdsetMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		return -1;
	}

	//解析json字符串
	rapidjson::Document doc;
	doc.Parse(updateJson);
	if (doc.HasParseError()) {
		strError = "{\"status\":3,\"msg\":\"param updateJson error\"}";
		return -1;
	}

	//检验参数正确性（是否存在options和update参数）
	rapidjson::Document::ConstMemberIterator iter = doc.FindMember("options");
	if (iter == doc.MemberEnd() || !iter->value.IsObject()) {
		strError = "{\"status\":4,\"msg\":\"param updateJson-options error\"}";
		return -1;
	}
	auto optionObj = iter->value.GetObject();

	iter = doc.FindMember("update");
	if (iter == doc.MemberEnd() || !iter->value.IsObject()) {
		strError = "{\"status\":5,\"msg\":\"param updateJson-update error\"}";
		return -1;
	}
	auto updateObj = iter->value.GetObject();

	std::vector<std::string> failids;

	//获取要修改的id
	std::vector<unispace::us_uuid> ids;
	iter = optionObj.FindMember("ids");
	if (iter != optionObj.MemberEnd() && iter->value.IsArray()) {
		auto idAry = iter->value.GetArray();
		unispace::us_uuid tmpid;
		for (auto i = 0; i < idAry.Size(); ++i) {
			if (!idAry[i].IsString()) { continue; }
			if (!tmpid.from_string(std::string(idAry[i].GetString(),
				idAry[i].GetStringLength()))) {
				failids.push_back(std::string(idAry[i].GetString(),
					idAry[i].GetStringLength()));
				continue;
			}
			ids.push_back(tmpid);
		}
	}

	//获取要修改的name
	iter = optionObj.FindMember("names");
	if (iter != optionObj.MemberEnd() && iter->value.IsArray()) {
		auto urlAry = iter->value.GetArray();
		unispace::us_uuid tmpid;
		for (auto i = 0; i < urlAry.Size(); ++i) {
			if (!urlAry[i].IsString()) { continue; }
			//通过url找到uuid并放在ids中
			if (pdsetMng->find_dataset_uuid(unispace::us_ustring(urlAry[i].GetString(),
				urlAry[i].GetStringLength()), &tmpid) != unispace::DM_RESULT::SUCCESS) {
				failids.push_back(std::string(urlAry[i].GetString(),
					urlAry[i].GetStringLength()));
				continue;
			}
			//不能有重复
			if (std::find(ids.begin(), ids.end(), tmpid) != ids.end()) {
				ids.push_back(tmpid);
			}
		}//end for
	}

	//批量修改(ids和urls没有列出时，才使用批量修改)
	if (ids.empty()) {
		int from = 0;
		iter = optionObj.FindMember("from");
		if (iter != optionObj.MemberEnd() && iter->value.IsInt()) {
			from = iter->value.GetInt();
		}
		int size = 0;
		iter = optionObj.FindMember("size");
		if (iter != optionObj.MemberEnd() && iter->value.IsInt()) {
			size = iter->value.GetInt();
		}
		unispace::us_dataset_index_list dsList;
		pdsetMng->get_dataset_index_list_by_range(from, size, &dsList);

		for (auto itr = dsList.begin(); itr != dsList.end(); ++itr) {
			ids.push_back(itr->uuid);
		}
	}

	if (ids.empty()) {
		strError = "{\"status\":6,\"msg\":\"param options id or name error\"";
		return -2;
	}

	//获取修改项
	//sprid,数据集的空间参考uuid
	unispace::us_uuid sprid;
	iter = updateObj.FindMember("sprid");
	if (iter != updateObj.MemberEnd() && iter->value.IsString()) {
		sprid.from_string(iter->value.GetString());
	}

	//desc，数据集描述
	unispace::us_ustring desc;
	iter = updateObj.FindMember("desc");
	if (iter != updateObj.MemberEnd() && iter->value.IsString()) {
		desc.assign(iter->value.GetString(), iter->value.GetStringLength());
	}

	//min_level,可显示最小网格层级
	int32_t min_level;
	iter = updateObj.FindMember("min_level");
	if (iter != updateObj.MemberEnd() && iter->value.IsInt()) {
		min_level = iter->value.GetInt();
	}

	//max_level,可显示最大网格层级
	int32_t max_level;
	iter = updateObj.FindMember("max_level");
	if (iter != updateObj.MemberEnd() && iter->value.IsInt()) {
		max_level = iter->value.GetInt();
	}

	//rgb_band,用于取瓦片定为RGB三个波段的波段序号
	int32_t rgb_band[3];
	iter = updateObj.FindMember("rgb_band");
	if (iter != updateObj.MemberEnd() && iter->value.IsArray()) {
		auto bandAry = iter->value.GetArray();
		if (bandAry.Size() != 3) {
			strError = "{\"status\":7,\"msg\":\"param bandAry error\"}";
			return -3;
		}
		for (int i = 0; i < 3; ++i)
		{
			if (bandAry[i].IsInt()) {
				int32_t band = bandAry[i].GetInt();
				if (band < 1 || band > 3) {
					strError = "{\"status\":8,\"msg\":\"param bandAry error\"}";
					return -3;
				}
				rgb_band[i] = bandAry[i].GetInt();
			}
			else {
				strError = "{\"status\":9,\"msg\":\"param bandAry error\"}";
				return -3;
			}
		}
	}

	//inrgb, 无效像素值
	uint32_t inrgb;
	iter = updateObj.FindMember("inrgb");
	if (iter != updateObj.MemberEnd() && iter->value.IsUint()) {
		inrgb = iter->value.GetUint();
	}

	//has_hot,冷热标识
	int32_t has_hot;
	iter = updateObj.FindMember("has_hot");
	if (iter != updateObj.MemberEnd() && iter->value.IsInt()) {
		has_hot = iter->value.GetInt();
	}

	//geo_rect
	double geo_rect[4];/*西，东，南，北*/
	iter = updateObj.FindMember("geo_rect");
	if (iter != updateObj.MemberEnd() && iter->value.IsArray()) {
		auto geoAry = iter->value.GetArray();
		if (geoAry.Size() != 4) {
			strError = "{\"status\":10,\"msg\":\"param geo_rect error\"}";
			return -4;
		}
		if (geoAry[0].IsDouble()) {
			geo_rect[0] = geoAry[0].GetDouble();
		}
		else if (geoAry[0].IsInt()) {
			geo_rect[0] = geoAry[0].GetInt();
		}

		if (geoAry[1].IsDouble()) {
			geo_rect[1] = geoAry[1].GetDouble();
		}
		else if (geoAry[1].IsInt()) {
			geo_rect[1] = geoAry[1].GetInt();
		}

		if (geoAry[2].IsDouble()) {
			geo_rect[2] = geoAry[2].GetDouble();
		}
		else if (geoAry[2].IsInt()) {
			geo_rect[2] = geoAry[2].GetInt();
		}

		if (geoAry[3].IsDouble()) {
			geo_rect[3] = geoAry[3].GetDouble();
		}
		else if (geoAry[3].IsInt()) {
			geo_rect[3] = geoAry[3].GetInt();
		}
	}

	//修改数据集元数据
	unispace::us_dataset_basic_meta dsetMeta;
	for (auto itr = ids.begin(); itr != ids.end(); ++itr)
	{
		//获取元数据
		unispace::DM_RESULT rt = pdsetMng->find_dataset(*itr, &dsetMeta);
		if (rt != unispace::DM_RESULT::SUCCESS) {
			failids.push_back(itr->to_brief_string());
			continue;
		}

		//修改元数据
		dsetMeta.m_spr_uuid = sprid;					/*空间参考id*/
		dsetMeta.m_desc = desc;							/*数据集描述*/
		dsetMeta.m_min_level = min_level;				/*可显示最小网格层级*/
		dsetMeta.m_max_level = max_level;				/*可显示最大网格层级*/
		dsetMeta.m_rgb_band[0] = rgb_band[0];			/*用于取瓦片定为RGB三个波段的波段序号*/
		dsetMeta.m_rgb_band[1] = rgb_band[1];
		dsetMeta.m_rgb_band[2] = rgb_band[2];
		dsetMeta.m_inrgb = inrgb;						/*无效像素值*/
		dsetMeta.m_has_hot = has_hot;					/*冷热标识*/
		dsetMeta.m_geo_rect.set_west(geo_rect[0]);		/*地理范围*/
		dsetMeta.m_geo_rect.set_east(geo_rect[1]);
		dsetMeta.m_geo_rect.set_south(geo_rect[2]);
		dsetMeta.m_geo_rect.set_north(geo_rect[3]);

		unispace::us_uuid tmpMeta;
		rt = pdsetMng->find_dataset_uuid(dsetMeta.get_name(), &tmpMeta);

		//更新dataset元数据
		unispace::DM_RESULT ret = pdsetMng->update_dataset(dsetMeta);
		if (ret != unispace::DM_RESULT::SUCCESS) {
			failids.push_back(itr->to_brief_string());
			continue;
		}
	}//end for

	 //输出修改失败的id
	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	writer.StartObject();
	writer.Key("status");
	writer.Int(0);
	writer.Key("failids");
	writer.StartArray();
	for (auto itr : failids) {
		writer.String(itr.c_str(), itr.size());
	}
	writer.EndArray();
	writer.EndObject();

	strError = std::string(strbuf.GetString(), strbuf.GetSize());

	return 0;
}




//获取wmts元数据
const char* meta_manage::get_wmts_meta(const void* pMng, int* len)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng == NULL || len == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng or len error\"}";
		*len = 0;
		return NULL;
	}

	unispace::us_wmts_manager* pwmtsMng = mng->get_wmts_manager();
	if (pwmtsMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		*len = 0;
		return NULL;
	}

	//获取数据库中wmts的总数
	long wmtsCount;
	unispace::DM_RESULT ret = pwmtsMng->get_wmts_count(&wmtsCount);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":3,\"msg\":\"get_wmts_count failed\"}";
		*len = 0;
		return NULL;
	}

	//获取数据集索引列表
	unispace::us_wmts_index_list wmtsList;
	ret = pwmtsMng->get_wmts_list_by_range(0, wmtsCount, &wmtsList);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":4,\"msg\":\"get_wmts_list_by_range failed\"}";
		*len = 0;
		return NULL;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

	writer.StartObject();
	writer.Key("counts");
	writer.Int64(wmtsCount);

	writer.Key("metas");
	writer.StartArray();

	//获取数据集元数据
	for (std::vector<unispace::WMTS_INDEX>::const_iterator itr = wmtsList.begin();
		itr != wmtsList.end(); ++itr)
	{
		//根据uuid获取数据集元数据
		unispace::us_wmts_basic_meta wmtsMeta;
		ret = pwmtsMng->find_wmts(itr->uuid, &wmtsMeta);
		if (ret != unispace::DM_RESULT::SUCCESS) {
			strError = "{\"status\":5,\"msg\":\"find_wmts failed\"}";
			*len = 0;
			return NULL;
		}

		//把影像元数据写入json
		unispace::us_ustring retJson;
		if (!unispace::us_wmts_meta_to_json(wmtsMeta, &retJson))
		{
			strError = "{\"status\":6,\"msg\":\"us_wmts_meta_to_json failed\"}";
			*len = 0;
			return NULL;
		}

		writer.RawValue(retJson.c_str(), retJson.size(), rapidjson::kObjectType);
	}

	writer.EndArray();
	writer.EndObject();

	sResult.assign(strbuf.GetString(), strbuf.GetSize());
	*len = sResult.size();

	return sResult.c_str();
}

//wmts元数据个数
long int meta_manage::get_wmts_count(const void* pMng)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng error\"}";
		return -1;
	}

	unispace::us_wmts_manager* pwmtsMng = mng->get_wmts_manager();
	if (pwmtsMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		return -2;
	}

	//获取当前数据库总的wmts数
	long int out_count;
	unispace::DM_RESULT ret = pwmtsMng->get_wmts_count(&out_count);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":3,\"msg\":\"get_wmts_count failed\"}";
		return -3;
	}

	return out_count;
}

//获取wmts元数据的id和layer
const char* meta_manage::get_wmts_idlayers(const void* pMng, long int from, long int size)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng error\"}";
		return NULL;
	}

	unispace::us_wmts_manager* pwmtsMng = mng->get_wmts_manager();
	if (pwmtsMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		return NULL;
	}

	//从数据库中获取wmts索引
	unispace::us_wmts_index_list wmtsIdx;
	unispace::DM_RESULT ret = pwmtsMng->get_wmts_list_by_range(from, size, &wmtsIdx);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":3,\"msg\":\"get_wmts_list_by_range failed\"}";
		return NULL;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	writer.StartObject();
	writer.Key("primary_keys");
	writer.StartArray();

	for (std::vector<unispace::WMTS_INDEX>::const_iterator itr = wmtsIdx.begin();
		itr != wmtsIdx.end(); ++itr)
	{
		writer.StartObject();
		writer.Key("id");
		writer.String(itr->uuid.to_brief_string().c_str());
		writer.Key("str");
		writer.String(itr->layer.c_str());
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();

	sResult.assign(strbuf.GetString(), strbuf.GetSize());

	return sResult.c_str();
}

//读取一个wmts的元数据
const char* meta_manage::get_awmts_meta(const void* pMng, const char* id)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng == NULL || id == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng or id error\"}";
		return NULL;
	}

	unispace::us_wmts_manager* pwmtsMng = mng->get_wmts_manager();
	if (pwmtsMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		return NULL;
	}

	//根据uuid获取wmts元数据
	unispace::us_wmts_basic_meta out_meta;
	unispace::us_uuid uuid;
	if (!uuid.from_string(std::string(id, strlen(id)))) {
		strError = "{\"status\":3,\"msg\":\"param id error\"}";
		return NULL;
	}
	unispace::DM_RESULT ret = pwmtsMng->find_wmts(uuid, &out_meta);
	if (ret != unispace::DM_RESULT::SUCCESS) {
		strError = "{\"status\":4,\"msg\":\"find_wmts failed\"}";
		return NULL;
	}

	//把wmts元数据写入json
	unispace::us_ustring retJson;
	if (!unispace::us_wmts_meta_to_json(out_meta, &retJson))
	{
		strError = "{\"status\":5,\"msg\":\"meta to json failed\"}";
		return NULL;
	}

	sResult.assign(retJson.c_str(), retJson.size());
	return sResult.c_str();
}

//更新wmts元数据.
int meta_manage::update_wmts_meta(const void* pMng, const char* updateJson)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (pMng == NULL || updateJson == NULL) {
		strError = "{\"status\":1,\"msg\":\"param pMng or updateJson error\"}";
		return -1;
	}

	unispace::us_wmts_manager* pwmtsMng = mng->get_wmts_manager();
	if (pwmtsMng == NULL) {
		strError = "{\"status\":2,\"msg\":\"param pMng error\"}";
		return -1;
	}

	//解析json字符串
	rapidjson::Document doc;
	doc.Parse(updateJson);
	if (doc.HasParseError()) {
		strError = "{\"status\":3,\"msg\":\"param updateJson error\"}";
		return -1;
	}

	//检验参数正确性（是否存在options和update参数）
	rapidjson::Document::ConstMemberIterator iter = doc.FindMember("options");
	if (iter == doc.MemberEnd() || !iter->value.IsObject()) {
		strError = "{\"status\":4,\"msg\":\"param options error\"}";
		return -1;
	}
	auto optionObj = iter->value.GetObject();

	iter = doc.FindMember("update");
	if (iter == doc.MemberEnd() || !iter->value.IsObject()) {
		strError = "{\"status\":5,\"msg\":\"param options error\"}";
		return -1;
	}
	auto updateObj = iter->value.GetObject();

	//获取要修改的id
	std::vector<unispace::us_uuid> ids;
	iter = optionObj.FindMember("ids");
	if (iter != optionObj.MemberEnd() && iter->value.IsArray()) {
		auto idAry = iter->value.GetArray();
		unispace::us_uuid tmpid;
		for (auto i = 0; i < idAry.Size(); ++i) {
			if (!idAry[i].IsString()) { continue; }
			if (!tmpid.from_string(std::string(idAry[i].GetString(),
				idAry[i].GetStringLength()))) {
				continue;
			}
			ids.push_back(tmpid);
		}
	}

	//获取要修改的layers
	iter = optionObj.FindMember("layers");
	if (iter != optionObj.MemberEnd() && iter->value.IsArray()) {
		auto layerAry = iter->value.GetArray();
		unispace::us_uuid tmpid;
		for (auto i = 0; i < layerAry.Size(); ++i) {
			if (!layerAry[i].IsString()) { continue; }
			//通过url找到uuid并放在ids中
			if (pwmtsMng->find_wmts_uuid(unispace::us_ustring(layerAry[i].GetString(),
				layerAry[i].GetStringLength()), &tmpid) != unispace::DM_RESULT::SUCCESS) {
				continue;
			}
			//不能有重复
			if (std::find(ids.begin(), ids.end(), tmpid) != ids.end()) {
				ids.push_back(tmpid);
			}
		}//end for
	}

	//批量修改(ids和layers没有列出时，才使用批量修改)
	if (ids.empty()) {
		int from = 0;
		iter = optionObj.FindMember("from");
		if (iter != optionObj.MemberEnd() && iter->value.IsInt()) {
			from = iter->value.GetInt();
		}
		int size = 0;
		iter = optionObj.FindMember("size");
		if (iter != optionObj.MemberEnd() && iter->value.IsInt()) {
			size = iter->value.GetInt();
		}
		unispace::us_wmts_index_list wmtsList;
		pwmtsMng->get_wmts_list_by_range(from, size, &wmtsList);

		for (auto itr = wmtsList.begin(); itr != wmtsList.end(); ++itr) {
			ids.push_back(itr->uuid);
		}
	}

	if (ids.empty()) {
		strError = "{\"status\":6,\"msg\":\"param options error\"}";
		return -2;
	}

	//获取修改项
	//sprid,数据集的空间参考uuid
	unispace::us_uuid sprid;
	iter = updateObj.FindMember("sprid");
	if (iter != updateObj.MemberEnd() && iter->value.IsString()) {
		sprid.from_string(iter->value.GetString());
	}

	//desc,描述信息
	unispace::us_ustring desc;
	iter = updateObj.FindMember("desc");
	if (iter != updateObj.MemberEnd() && iter->value.IsString()) {
		desc.assign(iter->value.GetString(), iter->value.GetStringLength());
	}

	//tile_pix_h,瓦片横向像素数
	int32_t tile_pix_h;
	iter = updateObj.FindMember("tile_pix_h");
	if (iter != updateObj.MemberEnd() && iter->value.IsInt()) {
		tile_pix_h = iter->value.GetInt();
	}

	//tile_pix_v,瓦片纵向像素数
	int32_t tile_pix_v;
	iter = updateObj.FindMember("tile_pix_v");
	if (iter != updateObj.MemberEnd() && iter->value.IsInt()) {
		tile_pix_v = iter->value.GetInt();
	}

	//tile_type,瓦片规则
	int32_t tile_type;
	iter = updateObj.FindMember("tile_type");
	if (iter != updateObj.MemberEnd() && iter->value.IsInt()) {
		tile_type = iter->value.GetInt();
	}

	//min_level,最小级别
	int32_t min_level;
	iter = updateObj.FindMember("min_level");
	if (iter != updateObj.MemberEnd() && iter->value.IsInt()) {
		min_level = iter->value.GetInt();
	}

	//max_level,最大级别
	int32_t max_level;
	iter = updateObj.FindMember("max_level");
	if (iter != updateObj.MemberEnd() && iter->value.IsInt()) {
		max_level = iter->value.GetInt();
	}

	//rgb_band,用于取瓦片定为RGB三个波段的波段序号
	int32_t rgb_band[3];
	iter = updateObj.FindMember("rgb_band");
	if (iter != updateObj.MemberEnd() && iter->value.IsArray()) {
		auto rgbAry = iter->value.GetArray();
		if (rgbAry.Size() != 3) {
			strError = "{\"status\":7,\"msg\":\"param rgb_band error\"}";
			return -3;
		}
		for (auto i = 0; i < rgbAry.Size(); ++i)
		{
			if (rgbAry[i].IsInt()) {
				int32_t band = rgbAry[i].GetInt();
				if (band < 1 || band > 3) {
					strError = "{\"status\":8,\"msg\":\"param rgb_band error\"}";
					return -3;
				}
				rgb_band[i] = band;
			}
			else {
				strError = "{\"status\":9,\"msg\":\"param rgb_band error\"}";
				return -3;
			}
		} //end for
	}//end if

	 //dpi_mode,DPI模式
	uint32_t dpi_mode;
	iter = updateObj.FindMember("dpi_mode");
	if (iter != updateObj.MemberEnd() && iter->value.IsUint()) {
		dpi_mode = iter->value.GetUint();
	}

	//geo_rect,地理范围
	double geo_rect[4];/*西，东，南，北*/
	iter = updateObj.FindMember("geo_rect");
	if (iter != updateObj.MemberEnd() && iter->value.IsArray()) {
		auto geoAry = iter->value.GetArray();
		if (geoAry.Size() != 4) {
			strError = "param geo_rect error";
			return -4;
		}
		if (geoAry[0].IsDouble()) {
			geo_rect[0] = geoAry[0].GetDouble();
		}
		else if (geoAry[0].IsInt()) {
			geo_rect[0] = geoAry[0].GetInt();
		}

		if (geoAry[1].IsDouble()) {
			geo_rect[1] = geoAry[1].GetDouble();
		}
		else if (geoAry[1].IsInt()) {
			geo_rect[1] = geoAry[1].GetInt();
		}

		if (geoAry[2].IsDouble()) {
			geo_rect[2] = geoAry[2].GetDouble();
		}
		else if (geoAry[2].IsInt()) {
			geo_rect[2] = geoAry[2].GetInt();
		}

		if (geoAry[3].IsDouble()) {
			geo_rect[3] = geoAry[3].GetDouble();
		}
		else if (geoAry[3].IsInt()) {
			geo_rect[3] = geoAry[3].GetInt();
		}
	}

	//修改wmts元数据
	unispace::us_wmts_basic_meta wmtsMeta;
	std::vector<std::string> failids;
	for (auto itr = ids.begin(); itr != ids.end(); ++itr)
	{
		//获取元数据
		unispace::DM_RESULT rt = pwmtsMng->find_wmts(*itr, &wmtsMeta);
		if (rt != unispace::DM_RESULT::SUCCESS) {
			failids.push_back(itr->to_brief_string());
			continue;
		}

		//修改元数据
		wmtsMeta.m_spr_uuid = sprid;					/*空间参考id*/
		wmtsMeta.m_desc = desc;							/*数据集描述*/
		wmtsMeta.m_tile_pixel_h = tile_pix_h;			/*瓦片横向像素值*/
		wmtsMeta.m_tile_pixel_v = tile_pix_v;			/*瓦片纵向像素值*/
		wmtsMeta.m_tile_type = tile_type;				/*瓦片规则*/
		wmtsMeta.m_min_level = min_level;				/*可显示最小网格层级*/
		wmtsMeta.m_max_level = max_level;				/*可显示最大网格层级*/
		wmtsMeta.m_rgb_band[0] = rgb_band[0];			/*用于取瓦片定为RGB三个波段的波段序号*/
		wmtsMeta.m_rgb_band[1] = rgb_band[1];
		wmtsMeta.m_rgb_band[2] = rgb_band[2];
		wmtsMeta.m_dpi_mode = dpi_mode;					/*DPI模式*/
		wmtsMeta.m_geo_rect.set_west(geo_rect[0]);		/*地理范围*/
		wmtsMeta.m_geo_rect.set_east(geo_rect[1]);
		wmtsMeta.m_geo_rect.set_south(geo_rect[2]);
		wmtsMeta.m_geo_rect.set_north(geo_rect[3]);

		unispace::us_uuid tmpMeta;

		//更新wmts元数据
		//先删除后添加
		if (pwmtsMng->delete_wmts(wmtsMeta.m_uuid) != unispace::DM_RESULT::SUCCESS) {
			failids.push_back(itr->to_brief_string());
			continue;
		}

		if (pwmtsMng->add_wmts_meta(wmtsMeta) != unispace::DM_RESULT::SUCCESS) {
			failids.push_back(itr->to_brief_string());
			continue;
		}
	}//end for

	 //输出修改失败的id
	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	writer.StartObject();
	writer.Key("status");
	writer.Int(0);
	writer.Key("failids");
	writer.StartArray();
	for (auto itr : failids) {
		writer.String(itr.c_str(), itr.size());
	}
	writer.EndArray();
	writer.EndObject();

	strError = std::string(strbuf.GetString(), strbuf.GetSize());

	return 0;
}



//销毁manager
void meta_manage::destroy_manager(const void* pMng)
{
	unispace::us_data_manager* mng = (unispace::us_data_manager*)pMng;
	if (mng != NULL)
	{
		mng->destory(mng);
	}
}


