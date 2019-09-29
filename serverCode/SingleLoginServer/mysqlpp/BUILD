cc_library(
	name = "mysqlpp",
	srcs = ["lib/libmysqlpp.a"],
	hdrs = glob([
	    "include/mysql++/*.h",
	 ]),
	copts = ["-Imysqlpp/include"],
	visibility = ["//visibility:public"],
	includes = [".", "include"],
	deps = [
	    "//mysqlclient:mysqlclient",
	]
)
