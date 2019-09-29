cc_library(
	name = "mysqlclient",
	srcs = ["lib/libmysqlclient.a"],
	hdrs = glob([
	    "include/mysql/*.h",
	    "include/mysql/mysql/*.h",
	    "include/mysql/mysql/*.h.pp",
	    "include/mysql/mysql/psi/*.h",
	 ]),
	copts = ["-Imysqlclient/include/mysql"],
	deps = [],
	visibility = ["//visibility:public"],
	includes = [".", "include"],
)
