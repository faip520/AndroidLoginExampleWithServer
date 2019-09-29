cc_library(
    name = "piggy-utils",
    srcs = ["src/openssl_rsa.cpp", "src/piggy_base64.cpp"],
    hdrs = ["include/piggy-utils/openssl_rsa.h", "include/piggy-utils/piggy_base64.h"],
	copts = ["-Iinclude/piggy-utils"],
    visibility = ["//visibility:public"],
    includes = ["include/piggy-utils"],
    deps = [
            "//:grpc++",
        ],
)