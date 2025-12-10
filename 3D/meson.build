project(
  'OpenGL',
  ['cpp', 'c'],
  version : '0.1',
  meson_version : '>= 1.3.0',
  default_options : ['warning_level=3', 'cpp_std=c++14'],
)

cpp = meson.get_compiler('cpp')


glfw_dep = dependency('glfw3', required : true)
glm_dep = dependency('glm', required : true)

dependencies = [
  glfw_dep, glm_dep
]

inc = include_directories('include')

exe = executable(
  'OpenGL',
  ['opengl.cpp', 'glad.c', 'tiny_gltf.cc'],
  link_args : ['-framework', 'Cocoa', '-framework', 'IOKit', '-framework', 'Foundation', '-framework', 'CoreVideo', '-framework', 'CoreFoundation', '-framework', 'CoreServices', '-framework', 'OpenGL', '-DGL_SILENCE_DEPRECATION'],
  install : true,
  dependencies : dependencies,
  include_directories : [inc],
)

test('basic', exe)
