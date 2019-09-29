cc_library(
    name = "mysqlclient",
    srcs = [
        "lib/libmysqlclient.a",
    ],
    hdrs = [
    ],
    includes = ['./include'],
    deps = [
    ],
    copts = [
        "-imysqlclient"
    ],
    linkstatic = 1,
    linkopts = [
        "-lm",
        "-lz",

    ],

    visibility = ["//visibility:public"],
)