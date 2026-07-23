load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

cc_library(
    name = "blackbox",
    hdrs = ["blackbox.h"],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":blackbox",
        ":validator",
    ],
)

cc_library(
    name = "validator",
    srcs = ["validator.cc"],
    hdrs = ["validator.h"],
    deps = [":blackbox"]
)
