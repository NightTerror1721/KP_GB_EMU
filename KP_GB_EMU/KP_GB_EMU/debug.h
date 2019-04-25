#pragma once

#ifdef _DEBUG
#include <iostream>
#include <string>

inline const std::string __db_msg_tag(const std::string& tag, const std::string& msg) { return tag + std::string(msg) + "\n"; }

#define db_info(msg, ...) std::printf(__db_msg_tag("[INFO] ", msg).c_str(), ##__VA_ARGS__);
#define db_warning(msg, ...) std::printf(__db_msg_tag("[WARNING] ", msg).c_str(), ##__VA_ARGS__);
#define db_error(msg, ...) std::fprintf(stderr, __db_msg_tag("[ERROR] ", msg).c_str(), ##__VA_ARGS__);
#define db_statement(expr) expr;
#define db_assert(expr) assert(expr);


#else

#define db_info(msg, ...)
#define db_warning(msg, ...)
#define db_error(msg, ...)
#define db_assert(expr)
#define db_statement(expr)

#endif
