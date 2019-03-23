load('//:buckaroo_macros.bzl', 'buckaroo_deps')

cxx_binary(
  name = 'hb-appstore',
  srcs = glob(['./main.cpp', 'gui/*.cpp', 'console/*.cpp']),
  deps = buckaroo_deps(),
)
