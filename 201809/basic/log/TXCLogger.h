// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

/*
 ============================================================================
 Name		: txclogger.h
 ============================================================================
 */

#ifndef TXCLOGGER_H
#define TXCLOGGER_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/cdefs.h>
#include <stdio.h>


#include "txg_log_base.h"
#include "txg_preprocessor.h"

#ifdef TXCLOGGER_DISABLE
#define  txf_logger_is_enabled_for(_level)	(false)
#define  txf_logger_assert_p(...) 		 	((void)0)
#define  txf_logger_assert(...)			((void)0)
#define  txf_logger_vprint(...)			((void)0)
#define  txf_logger_print(...)				((void)0)
#define  txf_logger_write(...)				((void)0)
#endif

#ifdef __cplusplus

#include <string>

template<bool x>
struct TXCLOGGER_STATIC_ASSERTION_FAILURE;
template<>
struct TXCLOGGER_STATIC_ASSERTION_FAILURE<true> {
	enum {
		value = 1
	};
};
template<int x>
struct txclogger_static_assert_test {
};


#define TXCLOGGER_STATIC_ASSERT(...) typedef ::txclogger_static_assert_test<\
                                        sizeof(::TXCLOGGER_STATIC_ASSERTION_FAILURE< ((__VA_ARGS__) == 0 ? false : true) >)>\
                                        PP_CAT(boost_static_assert_typedef_, __LINE__)


template<unsigned char base, typename T>
char *txclogger_itoa(T value, char *result, bool upper_case = true) {
	TXCLOGGER_STATIC_ASSERT(2 <= base && base <= 36);

	char *ptr_right = result, *ptr_left = result;
	T tmp_value = value;
	const char *num_mapping;

	if (upper_case)
		num_mapping = "ZYXWVUTSRQPONMLKJIHGFEDCBA9876543210123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	else
		num_mapping = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz";

	do {
		T quotient = tmp_value / base;
		*(ptr_right++) = num_mapping[35 + tmp_value - quotient * base];
		tmp_value = quotient;
	} while (tmp_value);


#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
	if (value < 0) *(ptr_right++) = '-';
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

	*(ptr_right--) = '\0';

	while (ptr_left < ptr_right) {
		char tmp_char = *ptr_right;
		*(ptr_right--) = *ptr_left;
		*(ptr_left++) = tmp_char;
	}
	return result;
}

class TXCVariant {
public:
	TXCVariant(bool aBool) : m_value(NULL) {
		if (aBool) m_value = "true"; else m_value = "false";
		m_valuecache[0] = 0;
	}

	TXCVariant(char aChar) : m_value(NULL) {
		m_valuecache[0] = aChar;
		m_valuecache[1] = '\0';
		m_value = m_valuecache;
	}

	TXCVariant(signed char aSChar) : m_value(NULL) {
		m_valuecache[0] = (char) aSChar;
		m_valuecache[1] = '\0';
		m_value = m_valuecache;
	}

	TXCVariant(unsigned char aUChar) : m_value(NULL) {
		txclogger_itoa<10>(aUChar, m_valuecache);
		m_value = m_valuecache;
	}

	TXCVariant(short aShort) : m_value(NULL) {
		txclogger_itoa<10>(aShort, m_valuecache);
		m_value = m_valuecache;
	}

	TXCVariant(unsigned short aUShort) : m_value(NULL) {
		txclogger_itoa<10>(aUShort, m_valuecache);
		m_value = m_valuecache;
	}

	TXCVariant(int aInt) : m_value(NULL) {
		txclogger_itoa<10>(aInt, m_valuecache);
		m_value = m_valuecache;
	}

	TXCVariant(unsigned int aUInt) : m_value(NULL) {
		txclogger_itoa<10>(aUInt, m_valuecache);
		m_value = m_valuecache;
	}

	TXCVariant(long aLong) : m_value(NULL) {
		txclogger_itoa<10>(aLong, m_valuecache);
		m_value = m_valuecache;
	}

	TXCVariant(unsigned long aULong) : m_value(NULL) {
		txclogger_itoa<10>(aULong, m_valuecache);
		m_value = m_valuecache;
	}

	TXCVariant(long long aLongLong) : m_value(NULL) {
		txclogger_itoa<10>(aLongLong, m_valuecache);
		m_value = m_valuecache;
	}

	TXCVariant(unsigned long long aULongLong) : m_value(NULL) {
		txclogger_itoa<10>(aULongLong, m_valuecache);
		m_value = m_valuecache;
	}

	TXCVariant(float aFloat) : m_value(NULL) {
		snprintf(m_valuecache, sizeof(m_valuecache), "%E", aFloat);
		m_value = m_valuecache;
	}

	TXCVariant(double aDouble) : m_value(NULL) {
		snprintf(m_valuecache, sizeof(m_valuecache), "%E", aDouble);
		m_value = m_valuecache;
	}

	TXCVariant(long double aLongDouble) : m_value(NULL) {
		snprintf(m_valuecache, sizeof(m_valuecache), "%LE", aLongDouble);
		m_value = m_valuecache;
	}

	TXCVariant(const void *aVoidPtr) : m_value(NULL) {
		m_valuecache[0] = '0';
		m_valuecache[1] = 'x';
		txclogger_itoa<16>((uintptr_t) aVoidPtr, m_valuecache + 2);
		m_value = m_valuecache;
	}

	TXCVariant(const char *aCharPtr) : m_value(NULL) {
		m_value = (const char *) aCharPtr;
		m_valuecache[0] = '\0';
	}

	TXCVariant(char *_ptr) : m_value(NULL) {
		m_value = (const char *) _ptr;
		m_valuecache[0] = '\0';
	}

	TXCVariant(const unsigned char *aUCharPtr) : m_value(NULL) {
		m_value = (const char *) aUCharPtr;
		m_valuecache[0] = '\0';
	}

	TXCVariant(const std::string &aValue) : m_value(NULL) {
		m_value = aValue.c_str();
		m_valuecache[0] = '\0';
	}

	~TXCVariant() {}

	const char *ToString() const { return m_value; }

private:
	TXCVariant(const TXCVariant &);

	TXCVariant &operator=(const TXCVariant &);

private:
	const char *m_value;
	char m_valuecache[65];
};


const struct TXSTypeSafeFormat {
	TXSTypeSafeFormat() {}
} __tsf__;
const struct TXSLogTag {
	TXSLogTag() {}
} __txclogger_tag__;
const struct TXSLogInfoNull {
	TXSLogInfoNull() {}
} __txclogger_info_null__;


class TXCMessage {
public:
	TXCMessage() : m_message() { m_message.reserve(512); }

	TXCMessage(std::string &_holder) : m_message(_holder) {}

	~TXCMessage() {}

public:
	const std::string &Message() const { return m_message; }

	std::string &Message() { return m_message; }

	const std::string &String() const { return m_message; }

	std::string &String() { return m_message; }

#ifdef __GNUC__

	__attribute__((__format__ (printf, 2, 0)))
#endif
	TXCMessage &WriteNoFormat(const char *_log) {
		m_message += _log;
		return *this;
	}

#ifdef __GNUC__

	__attribute__((__format__ (printf, 3, 0)))
#endif
	TXCMessage &WriteNoFormat(const TXSTypeSafeFormat &, const char *_log) {
		m_message += _log;
		return *this;
	}

	TXCMessage &operator<<(const TXCVariant &_value);

	TXCMessage &operator>>(const TXCVariant &_value);

	TXCMessage &operator()() { return *this; }

	void operator+=(const TXCVariant &_value) { m_message += _value.ToString(); }

#ifdef __GNUC__

	__attribute__((__format__ (printf, 2, 3)))
#endif
	TXCMessage &operator()(const char *_format, ...);

#ifdef __GNUC__

	__attribute__((__format__ (printf, 2, 0)))
#endif
	TXCMessage &VPrintf(const char *_format, va_list _list);

#define TXCLOGGER_FORMAT_ARGS(n) PP_ENUM_TRAILING_PARAMS(n, const TXCVariant& a)

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(0));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(1));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(2));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(3));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(4));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(5));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(6));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(7));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(8));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(9));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(10));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(11));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(12));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(13));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(14));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(15));

	TXCMessage &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(16));

#undef TXCLOGGER_FORMAT_ARGS

private:
	void DoTXSTypeSafeFormat(const char *_format, const TXCVariant **_args);

private:
//    TXCMessage(const TXCMessage&);
//    TXCMessage& operator=(const TXCMessage&);

private:
	std::string m_message;
};

class TXCLogger {
public:
	TXCLogger(TXELoggerLevel _level, const char *_tag, const char *_file, const char *_func, int _line,
			bool (*_hook)(TXSLogInfo &_info, std::string &_log))
			: m_info(), m_message(), m_isassert(false), m_exp(NULL), m_hook(_hook), m_isinfonull(false) {
		m_info.level = _level;
		m_info.tag = _tag;
		m_info.filename = _file;
		m_info.func_name = _func;
		m_info.line = _line;
		m_info.timeval.tv_sec = 0;
		m_info.timeval.tv_usec = 0;
		m_info.pid = -1;
		m_info.tid = -1;
		m_info.maintid = -1;

		m_message.reserve(512);
	}

	~TXCLogger() {
		if (!m_isassert && m_message.empty()) return;

		gettimeofday(&m_info.timeval, NULL);
		if (m_hook && !m_hook(m_info, m_message)) return;

		if (m_isassert)
			txf_logger_assert(m_isinfonull ? NULL : &m_info, m_exp, m_message.c_str());
		else
			txf_logger_write(m_isinfonull ? NULL : &m_info, m_message.c_str());
	}

public:
	TXCLogger &Assert(const char *_exp) {
		m_isassert = true;
		m_exp = _exp;
		return *this;
	}

	bool Empty() const { return !m_isassert && m_message.empty(); }

	const std::string &Message() const { return m_message; }

#ifdef __GNUC__

	__attribute__((__format__ (printf, 2, 0)))
#endif
	TXCLogger &WriteNoFormat(const char *_log) {
		m_message += _log;
		return *this;
	}

#ifdef __GNUC__

	__attribute__((__format__ (printf, 3, 0)))
#endif
	TXCLogger &WriteNoFormat(const TXSTypeSafeFormat &, const char *_log) {
		m_message += _log;
		return *this;
	}

	TXCLogger &operator<<(const TXCVariant &_value);

	TXCLogger &operator>>(const TXCVariant &_value);

	void operator>>(TXCLogger &_txclogger) {
		if (_txclogger.m_info.level < m_info.level) {
			_txclogger.m_info.level = m_info.level;
			_txclogger.m_isassert = m_isassert;
			_txclogger.m_exp = m_exp;
		}

		m_isassert = false;
		m_exp = NULL;

		_txclogger.m_message += m_message;
		m_message.clear();
	}

	void operator<<(TXCLogger &_txclogger) {
		_txclogger.operator>>(*this);
	}

	TXCLogger &operator()() { return *this; }

	TXCLogger &operator()(const TXSLogInfoNull &) {
		m_isinfonull = true;
		return *this;
	}

	TXCLogger &operator()(const TXSLogTag &, const char *_tag) {
		m_info.tag = _tag;
		return *this;
	}

#ifdef __GNUC__

	__attribute__((__format__ (printf, 2, 3)))
#endif
	TXCLogger &operator()(const char *_format, ...);

#ifdef __GNUC__

	__attribute__((__format__ (printf, 2, 0)))
#endif
	TXCLogger &VPrintf(const char *_format, va_list _list);

#define TXCLOGGER_FORMAT_ARGS(n) PP_ENUM_TRAILING_PARAMS(n, const TXCVariant& a)

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(0));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(1));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(2));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(3));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(4));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(5));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(6));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(7));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(8));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(9));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(10));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(11));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(12));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(13));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(14));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(15));

	TXCLogger &operator()(const TXSTypeSafeFormat &, const char *_format TXCLOGGER_FORMAT_ARGS(16));

#undef TXCLOGGER_FORMAT_ARGS

private:
	void DoTXSTypeSafeFormat(const char *_format, const TXCVariant **_args);

private:
	TXCLogger(const TXCLogger &);

	TXCLogger &operator=(const TXCLogger &);

private:
	TXSLogInfo m_info;
	std::string m_message;
	bool m_isassert;
	const char *m_exp;

	bool (*m_hook)(TXSLogInfo &_info, std::string &_log);

	bool m_isinfonull;
};


class TXCScopeTracer {
public:
	TXCScopeTracer(TXELoggerLevel _level, const char *_tag, const char *_name, const char *_file, const char *_func,
				 int _line,
				 const char *_log)
			: m_enable(txf_logger_is_enabled_for(_level)), m_info(), m_tv() {
		m_info.level = _level;

		if (m_enable) {
			m_info.tag = _tag;
			m_info.filename = _file;
			m_info.func_name = _func;
			m_info.line = _line;
			gettimeofday(&m_info.timeval, NULL);
			m_info.pid = -1;
			m_info.tid = -1;
			m_info.maintid = -1;

			strncpy(m_name, _name, sizeof(m_name));
			m_name[sizeof(m_name) - 1] = '\0';

			m_tv = m_info.timeval;
			char strout[1024] = {'\0'};
			snprintf(strout, sizeof(strout), "-> %s %s", m_name, NULL != _log ? _log : "");
			txf_logger_write(&m_info, strout);
		}
	}

	~TXCScopeTracer() {
		if (m_enable) {
			timeval tv;
			gettimeofday(&tv, NULL);
			m_info.timeval = tv;
			long timeSpan = (tv.tv_sec - m_tv.tv_sec) * 1000 + (tv.tv_usec - m_tv.tv_usec) / 1000;
			char strout[1024] = {'\0'};
			snprintf(strout, sizeof(strout), "<- %s +%ld, %s", m_name, timeSpan, m_exitmsg.c_str());
			txf_logger_write(&m_info, strout);
		}
	}

	void Exit(const std::string &_exitmsg) {
		m_exitmsg += _exitmsg;
	}

private:
	TXCScopeTracer(const TXCScopeTracer &);

	TXCScopeTracer &operator=(const TXCScopeTracer &);

private:
	bool m_enable;
	TXSLogInfo m_info;
	char m_name[128];
	timeval m_tv;

	std::string m_exitmsg;
};

///////////////////////////TXCMessage////////////////////
inline TXCMessage &TXCMessage::operator<<(const TXCVariant &_value) {
	if (NULL != _value.ToString()) {
		m_message += _value.ToString();
	} else {
		assert(false);
	}
	return *this;
}

inline TXCMessage &TXCMessage::operator>>(const TXCVariant &_value) {
	if (NULL != _value.ToString()) {
		m_message.insert(0, _value.ToString());
	} else {
		assert(false);
	}
	return *this;
}

inline TXCMessage &TXCMessage::VPrintf(const char *_format, va_list _list) {
	if (_format == NULL) {
		assert(false);
		return *this;
	}

	char temp[4096] = {'\0'};
	vsnprintf(temp, 4096, _format, _list);
	m_message += temp;
	return *this;
}

inline TXCMessage &TXCMessage::operator()(const char *_format, ...) {
	if (_format == NULL) {
		assert(false);
		return *this;
	}

	va_list valist;
	va_start(valist, _format);
	VPrintf(_format, valist);
	va_end(valist);
	return *this;
}

#define TXCLOGGER_FORMAT_ARGS(n) PP_ENUM_TRAILING_PARAMS(n, const TXCVariant& a)
#define TXCLOGGER_VARIANT_ARGS(n) PP_ENUM_PARAMS(n, &a)
#define TXCLOGGER_VARIANT_ARGS_NULL(n) PP_ENUM(n, NULL)
#define TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(n, m) \
        inline TXCMessage& TXCMessage::operator()(const TXSTypeSafeFormat&, const char* _format TXCLOGGER_FORMAT_ARGS(n)) { \
        if (_format != NULL) { \
            const TXCVariant* args[16] = { TXCLOGGER_VARIANT_ARGS(n) PP_COMMA_IF(PP_AND(n, m)) TXCLOGGER_VARIANT_ARGS_NULL(m) }; \
            DoTXSTypeSafeFormat(_format, args); \
        } \
        return *this;\
    }

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(0, 16)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(1, 15)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(2, 14)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(3, 13)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(4, 12)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(5, 11)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(6, 10)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(7, 9)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(8, 8)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(9, 7)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(10, 6)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(11, 5)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(12, 4)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(13, 3)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(14, 2)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(15, 1)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(16, 0)

#undef TXCLOGGER_FORMAT_ARGS
#undef TXCLOGGER_VARIANT_ARGS
#undef TXCLOGGER_VARIANT_ARGS_NULL
#undef TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT

inline void TXCMessage::DoTXSTypeSafeFormat(const char *_format, const TXCVariant **_args) {

	const char *current = _format;
	int count = 0;
	while ('\0' != *current) {
		if ('%' != *current) {
			m_message += *current;
			++current;
			continue;
		}

		char nextch = *(current + 1);
		if (('0' <= nextch && nextch <= '9') || nextch == '_') {
			int argIndex = count;
			if (nextch != '_') argIndex = nextch - '0';

			if (_args[argIndex] != NULL) {
				if (NULL != _args[argIndex]->ToString()) {
					m_message += _args[argIndex]->ToString();
				} else {
					m_message += "(null)";
					assert(false);
				}
			} else {
				assert(false);
			}
			count++;
			current += 2;
		} else if (nextch == '%') {
			m_message += '%';
			current += 2;
		} else {
			++current;
			assert(false);
		}
	}
}

///////////////////////////TXCLogger////////////////////
inline TXCLogger &TXCLogger::operator<<(const TXCVariant &_value) {
	if (NULL != _value.ToString()) {
		m_message += _value.ToString();
	} else {
		m_info.level = kLevelFatal;
		m_message += "{!!! TXCLogger& TXCLogger::operator<<(const TXCVariant& _value): _value.ToString() == NULL !!!}";
		assert(false);
	}
	return *this;
}

inline TXCLogger &TXCLogger::operator>>(const TXCVariant &_value) {
	if (NULL != _value.ToString()) {
		m_message.insert(0, _value.ToString());
	} else {
		m_info.level = kLevelFatal;
		m_message.insert(0,
						 "{!!! TXCLogger& TXCLogger::operator>>(const TXCVariant& _value): _value.ToString() == NULL !!!}");
		assert(false);
	}
	return *this;
}

inline TXCLogger &TXCLogger::VPrintf(const char *_format, va_list _list) {
	if (_format == NULL) {
		m_info.level = kLevelFatal;
		m_message += "{!!! TXCLogger& TXCLogger::operator()(const char* _format, va_list _list): _format == NULL !!!}";
		assert(false);
		return *this;
	}

	char temp[4096] = {'\0'};
	vsnprintf(temp, 4096, _format, _list);
	m_message += temp;
	return *this;
}

inline TXCLogger &TXCLogger::operator()(const char *_format, ...) {
	if (_format == NULL) {
		m_info.level = kLevelFatal;
		m_message += "{!!! TXCLogger& TXCLogger::operator()(const char* _format, ...): _format == NULL !!!}";
		assert(false);
		return *this;
	}

	va_list valist;
	va_start(valist, _format);
	VPrintf(_format, valist);
	va_end(valist);
	return *this;
}

#define TXCLOGGER_FORMAT_ARGS(n) PP_ENUM_TRAILING_PARAMS(n, const TXCVariant& a)
#define TXCLOGGER_VARIANT_ARGS(n) PP_ENUM_PARAMS(n, &a)
#define TXCLOGGER_VARIANT_ARGS_NULL(n) PP_ENUM(n, NULL)
#define TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(n, m) \
        inline TXCLogger& TXCLogger::operator()(const TXSTypeSafeFormat&, const char* _format TXCLOGGER_FORMAT_ARGS(n)) { \
        if (_format != NULL) { \
            const TXCVariant* args[16] = { TXCLOGGER_VARIANT_ARGS(n) PP_COMMA_IF(PP_AND(n, m)) TXCLOGGER_VARIANT_ARGS_NULL(m) }; \
            DoTXSTypeSafeFormat(_format, args); \
        } \
        return *this;\
    }

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(0, 16)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(1, 15)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(2, 14)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(3, 13)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(4, 12)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(5, 11)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(6, 10)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(7, 9)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(8, 8)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(9, 7)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(10, 6)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(11, 5)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(12, 4)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(13, 3)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(14, 2)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(15, 1)

TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT(16, 0)


#undef TXCLOGGER_FORMAT_ARGS
#undef TXCLOGGER_VARIANT_ARGS
#undef TXCLOGGER_VARIANT_ARGS_NULL
#undef TXCLOGGER_TYPESAFE_FORMAT_IMPLEMENT

inline void TXCLogger::DoTXSTypeSafeFormat(const char *_format, const TXCVariant **_args) {

	const char *current = _format;
	int count = 0;
	while ('\0' != *current) {
		if ('%' != *current) {
			m_message += *current;
			++current;
			continue;
		}

		char nextch = *(current + 1);
		if (('0' <= nextch && nextch <= '9') || nextch == '_') {

			int argIndex = count;
			if (nextch != '_') argIndex = nextch - '0';

			if (_args[argIndex] != NULL) {
				if (NULL != _args[argIndex]->ToString()) {
					m_message += _args[argIndex]->ToString();
				} else {
					m_info.level = kLevelFatal;
					m_message += "{!!! void TXCLogger::DoTXSTypeSafeFormat: _args[";
					m_message += TXCVariant(argIndex).ToString();
					m_message += "]->ToString() == NULL !!!}";
					assert(false);
				}
			} else {
				m_info.level = kLevelFatal;
				m_message += "{!!! void TXCLogger::DoTXSTypeSafeFormat: _args[";
				m_message += TXCVariant(argIndex).ToString();
				m_message += "] == NULL !!!}";
				assert(false);
			}
			count++;
			current += 2;
		} else if (nextch == '%') {
			m_message += '%';
			current += 2;
		} else {
			++current;
			m_info.level = kLevelFatal;
			m_message += "{!!! void TXCLogger::DoTXSTypeSafeFormat: %";
			m_message += nextch;
			m_message += " not fit mode !!!}";
			assert(false);
		}
	}
}

#endif //cpp


#define __CONCAT_IMPL__(x, y)       x##y
#define __CONCAT__(x, y)            __CONCAT_IMPL__(x, y)
#define __ANONYMOUS_VARIABLE__(x)   __CONCAT__(x, __LINE__)

#define __XFILE__                   (__FILE__)

#ifndef _MSC_VER
//#define __XFUNCTION__       __PRETTY_FUNCTION__
#define __XFUNCTION__       __FUNCTION__
#else
// Definitely, VC6 not support this feature!
#if _MSC_VER > 1200
#define __XFUNCTION__   __FUNCSIG__
#else
#define __XFUNCTION__   "N/A"
#warning " is not supported by this compiler"
#endif
#endif

//txclogger define

#ifndef TXCLOGGER_TAG
#define TXCLOGGER_TAG ""
#endif

/* tips: this code replace or change the tag in source file
static const char* __my_txclogger_tag = "prefix_"TXCLOGGER_TAG"_suffix";
#undef TXCLOGGER_TAG
#define TXCLOGGER_TAG __my_txclogger_tag
*/

#define xdump txclogger_dump
#define TXCLOGGER_ROUTER_OUTPUT(op1, op, ...) PP_IF(PP_NUM_PARAMS(__VA_ARGS__),PP_IF(PP_DEC(PP_NUM_PARAMS(__VA_ARGS__)),op,op1), )

#if !defined(__cplusplus)

#ifdef __GNUC__
__attribute__((__format__ (printf, 2, 3)))
#endif
__inline void  __txclogger_c_write(const TXSLogInfo* _info, const char* _log, ...) { txf_logger_write(_info, _log); }

#define txclogger2(level, tag, file, func, line, ...)      if ((!txf_logger_is_enabled_for(level)));\
															  else { TXSLogInfo info= {level, tag, file, func, line,\
																	 {0, 0}, -1, -1, -1};\ gettimeofday(&info.timeval, NULL);\
																	 TXCLOGGER_ROUTER_OUTPUT(__txclogger_c_write(&info, __VA_ARGS__),txf_logger_print(&info, __VA_ARGS__), __VA_ARGS__);}

#define txclogger2_if(exp, level, tag, file, func, line, ...)    if (!(exp) || !txf_logger_is_enabled_for(level));\
																	else { TXSLogInfo info= {level, tag, file, func, line,\
																		   {0, 0}, -1, -1, -1}; gettimeofday(&info.timeval, NULL);\
																		   TXCLOGGER_ROUTER_OUTPUT(__txclogger_c_write(&info, __VA_ARGS__),txf_logger_print(&info, __VA_ARGS__), __VA_ARGS__);}

#define __txclogger_c_impl(level,  ...) 			txclogger2(level, TXCLOGGER_TAG, __XFILE__, __XFUNCTION__, __LINE__, __VA_ARGS__)
#define __txclogger_c_impl_if(level, exp, ...) 	txclogger2_if(exp, level, TXCLOGGER_TAG, __XFILE__, __XFUNCTION__, __LINE__, __VA_ARGS__)

#define xverbose2(...)             __txclogger_c_impl(kLevelVerbose, __VA_ARGS__)
#define xdebug2(...)               __txclogger_c_impl(kLevelDebug, __VA_ARGS__)
#define xinfo2(...)                __txclogger_c_impl(kLevelInfo, __VA_ARGS__)
#define xwarn2(...)                __txclogger_c_impl(kLevelWarn, __VA_ARGS__)
#define xerror2(...)               __txclogger_c_impl(kLevelError, __VA_ARGS__)
#define xfatal2(...)               __txclogger_c_impl(kLevelFatal, __VA_ARGS__)

#define xverbose2_if(exp, ...)     __txclogger_c_impl_if(kLevelVerbose, exp, __VA_ARGS__)
#define xdebug2_if(exp, ...)       __txclogger_c_impl_if(kLevelDebug, exp, __VA_ARGS__)
#define xinfo2_if(exp, ...)        __txclogger_c_impl_if(kLevelInfo, exp, __VA_ARGS__)
#define xwarn2_if(exp, ...)        __txclogger_c_impl_if(kLevelWarn, exp,  __VA_ARGS__)
#define xerror2_if(exp, ...)       __txclogger_c_impl_if(kLevelError, exp, __VA_ARGS__)
#define xfatal2_if(exp, ...)       __txclogger_c_impl_if(kLevelFatal, exp, __VA_ARGS__)

#define xassert2(exp, ...)    if (((exp) || !txf_logger_is_enabled_for(kLevelFatal)));else {\
									TXSLogInfo info= {kLevelFatal, TXCLOGGER_TAG, __XFILE__, __XFUNCTION__, __LINE__,\
									{0, 0}, -1, -1, -1};\
									gettimeofday(&info.m_tv, NULL);\
									txf_logger_assert_p(&info, #exp, __VA_ARGS__);}
//"##__VA_ARGS__" remove "," if NULL
#else

#ifndef TXCLOGGER_HOOK
#define TXCLOGGER_HOOK NULL
#endif

#define txclogger(level, tag, file, func, line, ...)     if ((!txf_logger_is_enabled_for(level)));\
                                                       else TXCLogger(level, tag, file, func, line, TXCLOGGER_HOOK)\
                                                             TXCLOGGER_ROUTER_OUTPUT(.WriteNoFormat(TSF __VA_ARGS__),(TSF __VA_ARGS__), __VA_ARGS__)

#define txclogger2(level, tag, file, func, line, ...)     if ((!txf_logger_is_enabled_for(level)));\
                                                        else TXCLogger(level, tag, file, func, line, TXCLOGGER_HOOK)\
                                                             TXCLOGGER_ROUTER_OUTPUT(.WriteNoFormat(__VA_ARGS__),(__VA_ARGS__), __VA_ARGS__)

#define txclogger2_if(exp, level, tag, file, func, line, ...)     if ((!(exp) || !txf_logger_is_enabled_for(level)));\
                                                                else TXCLogger(level, tag, file, func, line, TXCLOGGER_HOOK)\
                                                                     TXCLOGGER_ROUTER_OUTPUT(.WriteNoFormat(__VA_ARGS__),(__VA_ARGS__), __VA_ARGS__)

#define __txclogger_cpp_impl2(level, ...)             txclogger2(level, TXCLOGGER_TAG, __XFILE__, __XFUNCTION__, __LINE__, __VA_ARGS__)
#define __txclogger_cpp_impl_if(level, exp, ...)     txclogger2_if(exp, level, TXCLOGGER_TAG, __XFILE__, __XFUNCTION__, __LINE__, __VA_ARGS__)

#define xverbose2(...)             __txclogger_cpp_impl2(kLevelVerbose, __VA_ARGS__)
#define xdebug2(...)               __txclogger_cpp_impl2(kLevelDebug, __VA_ARGS__)
#define xinfo2(...)                __txclogger_cpp_impl2(kLevelInfo, __VA_ARGS__)
#define xwarn2(...)                __txclogger_cpp_impl2(kLevelWarn, __VA_ARGS__)
#define xerror2(...)               __txclogger_cpp_impl2(kLevelError, __VA_ARGS__)
#define xfatal2(...)               __txclogger_cpp_impl2(kLevelFatal, __VA_ARGS__)
#define xlog2(level, ...)          __txclogger_cpp_impl2(level, __VA_ARGS__)

#define xverbose2_if(exp, ...)     __txclogger_cpp_impl_if(kLevelVerbose, exp,  __VA_ARGS__)
#define xdebug2_if(exp, ...)       __txclogger_cpp_impl_if(kLevelDebug, exp,  __VA_ARGS__)
#define xinfo2_if(exp, ...)        __txclogger_cpp_impl_if(kLevelInfo, exp,  __VA_ARGS__)
#define xwarn2_if(exp, ...)        __txclogger_cpp_impl_if(kLevelWarn, exp,  __VA_ARGS__)
#define xerror2_if(exp, ...)       __txclogger_cpp_impl_if(kLevelError, exp,  __VA_ARGS__)
#define xfatal2_if(exp, ...)       __txclogger_cpp_impl_ifkLevelFatal, exp, __VA_ARGS__)
#define xlog2_if(level, ...)       __txclogger_cpp_impl_if(level, __VA_ARGS__)

#define xgroup2_define(group)      TXCLogger group(kLevelAll, TXCLOGGER_TAG, __XFILE__, __XFUNCTION__, __LINE__, TXCLOGGER_HOOK)
#define xgroup2(...)               TXCLogger(kLevelAll, TXCLOGGER_TAG, __XFILE__, __XFUNCTION__, __LINE__, TXCLOGGER_HOOK)(__VA_ARGS__)
#define xgroup2_if(exp, ...)       if ((!(exp))); else TXCLogger(kLevelAll, TXCLOGGER_TAG, __XFILE__, __XFUNCTION__, __LINE__, TXCLOGGER_HOOK)(__VA_ARGS__)

#define xassert2(exp, ...)    if (((exp) || !txf_logger_is_enabled_for(kLevelFatal)));\
                             else TXCLogger(kLevelFatal, TXCLOGGER_TAG, __XFILE__, __XFUNCTION__, __LINE__, TXCLOGGER_HOOK).Assert(#exp)\
                                  TXCLOGGER_ROUTER_OUTPUT(.WriteNoFormat(__VA_ARGS__),(__VA_ARGS__), __VA_ARGS__)

#define xmessage2_define(name, ...)    TXCMessage name; name TXCLOGGER_ROUTER_OUTPUT(.WriteNoFormat(__VA_ARGS__),(__VA_ARGS__), __VA_ARGS__)
#define xmessage2(...)                TXCMessage() TXCLOGGER_ROUTER_OUTPUT(.WriteNoFormat(__VA_ARGS__),(__VA_ARGS__), __VA_ARGS__)


#define TXCLOGGER_SCOPE_MESSAGE(...)      PP_IF(PP_NUM_PARAMS(__VA_ARGS__), xmessage2(__VA_ARGS__).String().c_str(), NULL)
#define __xscope_impl(level, name, ...)   TXCScopeTracer __ANONYMOUS_VARIABLE__(_tracer_)(level, TXCLOGGER_TAG, name, __XFILE__, __XFUNCTION__, __LINE__, TXCLOGGER_SCOPE_MESSAGE(__VA_ARGS__))

#define xverbose_scope(name, ...)       __xscope_impl(kLevelVerbose, name, __VA_ARGS__)
#define xdebug_scope(name, ...)         __xscope_impl(kLevelDebug, name, __VA_ARGS__)
#define xinfo_scope(name, ...)          __xscope_impl(kLevelInfo, name, __VA_ARGS__)

#define __xfunction_scope_impl(level, name, ...)    TXCScopeTracer ____xloger_anonymous_function_scope_20151022____(level, TXCLOGGER_TAG, name, __XFILE__, __XFUNCTION__, __LINE__, TXCLOGGER_SCOPE_MESSAGE(__VA_ARGS__))

#define xverbose_function(...)          __xfunction_scope_impl(kLevelVerbose, __FUNCTION__, __VA_ARGS__)
#define xdebug_function(...)            __xfunction_scope_impl(kLevelDebug, __FUNCTION__, __VA_ARGS__)
#define xinfo_function(...)             __xfunction_scope_impl(kLevelInfo, __FUNCTION__, __VA_ARGS__)
#define xexitmsg_function(...)             ____xloger_anonymous_function_scope_20151022____.Exit(xmessage2(__VA_ARGS__).String())
#define xexitmsg_function_if(exp, ...)     if((!exp)); else ____xloger_anonymous_function_scope_20151022____.Exit(xmessage2(__VA_ARGS__).String())


#define TSF __tsf__,
#define XTAG __txclogger_tag__,
#define XNULL __txclogger_info_null__
#define XENDL "\n"
#define XTHIS "@%p, ", this

#endif
#endif /* TXCLOGGER_H */
