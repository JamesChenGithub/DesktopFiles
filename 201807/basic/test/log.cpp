#include "log/txg_appender.h"
#include "log/TXCLogger.h"

int main() {
	txf_logger_set_level(kLevelDebug);
	txf_appender_set_console_log(true);
	txf_appender_open(kAppednerAsync, "/Users/alderzhang/Downloads/log", "Test");
	xinfo2("%s", "test1");
	xinfo2("%s", "test2");
	xinfo2("%s", "test3");
	xinfo2("%s", "test4");
	txf_appender_close();
	return 0;
}