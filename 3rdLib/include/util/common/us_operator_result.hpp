#ifndef __US_OPERATOR_RESULT_HPP__
#define __US_OPERATOR_RESULT_HPP__

#include <stdlib.h>
#include <string.h>

namespace unispace
{
	// --------------------------------------------------------------------------------
	/// <summary>
	/// @class	us_operator_result
	/// @brief	操作结果类.
	/// @author	yimin.liu@unispace-x.com
	/// @date	2018/5/25
	/// </summary>
	// --------------------------------------------------------------------------------
	class us_operator_result {
		int      m_status;		///< 操作结果状态码
		int      m_msg_len;		///< m_message分配内存大小
		char*	 m_message;		///< 操作结果消息字符串

	public:
		inline us_operator_result(
			int status = 0,const char* msg = 0,
			int msglen = -1,bool needAllocMem = true)
			:m_status(status), m_msg_len(0), m_message(0)
		{
			set_msg(msg, msglen, needAllocMem);
		}
		inline us_operator_result(us_operator_result&& other)
			: m_status(other.m_status),
			m_msg_len(other.m_msg_len), m_message(other.m_message)
		{
			other.m_message = NULL; other.m_msg_len = 0;
		}
		inline us_operator_result(const us_operator_result& other)
			: m_status(other.m_status), m_msg_len(0), m_message(0)
		{
			set_msg(other.m_message, other.m_msg_len);
		}
		inline ~us_operator_result()
		{
			if (m_message == NULL || m_msg_len == -5) { return; }
			free(m_message);
		}

		////////////////////////////////////////////////////////////
		inline void set_msg(const char* msg, int msglen = -1, bool needAllocMem = true)
		{
			if (msg == 0) { return; }
			if (!needAllocMem) {
				m_msg_len = -5;
				m_message = (char*)msg;
				return;
			}
			if (msglen == -1) { msglen = (int)strlen(msg); }

			if (m_message != NULL) {
				if (m_msg_len > msglen) {
					memcpy(m_message, msg, msglen);
					m_message[msglen] = '\0';
					return;
				}
				if (m_msg_len != -5) { free(m_message); }
			}

			m_msg_len = (msglen + 63) & (~63);	// 对齐到64Byte
			m_message = (char*)malloc((size_t)m_msg_len);
			memcpy(m_message, msg, msglen);
			m_message[msglen] = '\0';
		}

		////////////////////////////////////////////////////////////
		inline int get_status() const
		{
			return m_status;
		}
		inline const char* get_msg() const
		{
			return m_message;
		}
		inline const char* get_safe_msg()const
		{
			return m_message == NULL ? "" : m_message;
		}
		inline operator bool() const
		{
			return m_status == 0;
		}
	};

}

#endif // !__US_OPERATOR_RESULT_HPP__
