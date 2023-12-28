#ifndef LOG_H
#define LOG_H
#include "main.h"
#include "chrono.h"
#include "SEGGER_RTT.h"

void do_log(const char *fmt, ...);

#define LOG_E(x, ...) do_log(RTT_CTRL_TEXT_BRIGHT_RED x "\n" RTT_CTRL_TEXT_BRIGHT_WHITE, ##__VA_ARGS__)
#define LOG_W(x, ...) do_log(RTT_CTRL_TEXT_BRIGHT_YELLOW x "\n" RTT_CTRL_TEXT_BRIGHT_WHITE, ##__VA_ARGS__)
#define LOG_I(x, ...) do_log(RTT_CTRL_TEXT_BRIGHT_GREEN x "\n" RTT_CTRL_TEXT_BRIGHT_WHITE, ##__VA_ARGS__)
#define LOG_D(x, ...) do_log(RTT_CTRL_TEXT_BRIGHT_WHITE x "\n" RTT_CTRL_TEXT_BRIGHT_WHITE, ##__VA_ARGS__)

#define LOG_E_STAMPED(x, ...) do_log(RTT_CTRL_TEXT_BRIGHT_RED "[%9.4f] " x "\n" RTT_CTRL_TEXT_BRIGHT_WHITE, GET_STAMP(), ##__VA_ARGS__)
#define LOG_W_STAMPED(x, ...) do_log(RTT_CTRL_TEXT_BRIGHT_YELLOW "[%9.4f] " x "\n" RTT_CTRL_TEXT_BRIGHT_WHITE, GET_STAMP(), ##__VA_ARGS__)
#define LOG_I_STAMPED(x, ...) do_log(RTT_CTRL_TEXT_BRIGHT_GREEN "[%9.4f] " x "\n" RTT_CTRL_TEXT_BRIGHT_WHITE, GET_STAMP(), ##__VA_ARGS__)
#define LOG_D_STAMPED(x, ...) do_log(RTT_CTRL_TEXT_BRIGHT_WHITE "[%9.4f] " x "\n" RTT_CTRL_TEXT_BRIGHT_WHITE, GET_STAMP(), ##__VA_ARGS__)

#endif