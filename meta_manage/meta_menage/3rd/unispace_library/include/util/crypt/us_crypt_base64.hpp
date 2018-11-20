#ifndef __US_CRYPT_BASE64_HPP__
#define __US_CRYPT_BASE64_HPP__

#ifndef __US_UTIL_HPP__
#include "../us_util.hpp"
#endif

#include <cstddef>
#include <string>
#include <vector>
#include <cctype>

namespace unispace
{
	class _US_UTIL_DLL us_crypt_base64
	{
	public:
		/**
		* \brief          编码一个buffer到base64格式
		*
		* \param dst      目标 buffer
		* \param dlen     目标buffer大小
		* \param src      源 buffer
		* \param slen     要编码的数据量
		*
		* \return         成功返回0，失败返回非0值.
		*
		* \note           调用时候传入的 *dlen = 0 用于获取适当的*dlen大小.
		*/
		static int encode(unsigned char *dst, int *dlen,
			unsigned char *src, int  slen);

		/**
		* \brief          解码一个base64格式数据
		*
		* \param dst      目标 buffer
		* \param dlen     目标buffer大小
		* \param src      源 buffer
		* \param slen     要解码的数据量
		*
		* \return         成功返回0，失败返回非0值.
		*
		* \note           调用时候传入的 *dlen = 0 用于获取适当的*dlen大小.
		*/
		static int decode(unsigned char *dst, int *dlen,
			unsigned char *src, int  slen);

	};


	inline std::string us_crypt_encode_base64(const void* data, std::size_t in_len)
	{
		const std::string chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		const unsigned char* bytes_to_encode = static_cast<const unsigned char*>(data);
		unsigned int i = 0;
		unsigned int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];

		std::string out;
		out.reserve(in_len / 5 * 8 + 4);
		while (in_len--) {
			char_array_3[i++] = *(bytes_to_encode++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for (i = 0; (i < 4); i++)
					out += chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i) {
			for (j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (j = 0; (j < i + 1); j++)
				out += chars[char_array_4[j]];

			while ((i++ < 3))
				out += '=';
		}

		return out;
	}

	template <typename T>
	inline std::vector<T> us_crypt_decode_base64(const char* encoded, std::size_t in_len)
	{
		const std::string chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		unsigned int i = 0;
		unsigned int j = 0;
		unsigned int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];

		std::vector<T> out;
		out.reserve(in_len / 8 * 5 + 2);
		while (in_len-- && (encoded[in_] != '=') && (isalnum(encoded[in_]) ||
			encoded[in_] == '+' || encoded[in_] == '/')) {
			char_array_4[i++] = encoded[in_]; in_++;
			if (i == 4) {
				for (i = 0; i < 4; i++)
					char_array_4[i] = static_cast<unsigned char>(chars.find(char_array_4[i]));

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
					out.push_back(char_array_3[i]);
				i = 0;
			}
		}

		if (i) {
			for (j = i; j < 4; j++)
				char_array_4[j] = 0;

			for (j = 0; j < 4; j++)
				char_array_4[j] = static_cast<unsigned char>(chars.find(char_array_4[j]));

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (j = 0; (j < i - 1); j++) out.push_back(char_array_3[j]);
		}

		return out;
	}
}
#endif // !__US_CRYPT_BASE64_HPP__
