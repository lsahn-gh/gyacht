gyacht_sources = [
  'main.c',
  'gyacht-application.c',
  'gyacht-container.c',
  'gyacht-container-json.c',
  'gyacht-container-list-view.c',
  'gyacht-container-service.c',
  'gyacht-file-utils.c',
  'gyacht-image.c',
  'gyacht-image-json.c',
  'gyacht-image-list-view.c',
  'gyacht-image-service.c',
  'gyacht-path-manager.c',
  'gyacht-service.c',
  'gyacht-window.c',
]

gyacht_deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('gtk+-3.0', version: '>= 3.22'),
  dependency('json-glib-1.0', version: '>= 1.4.4'),
  dependency('libhandy-0.0', version: '>= 0.0.8')
]

gnome = import('gnome')

gyacht_sources += gnome.compile_resources('gyacht-resources',
  'gyacht.gresource.xml',
  c_name: 'gyacht'
)

executable(
  'gyacht',
  gyacht_sources,
  dependencies: gyacht_deps,
  c_args: gyacht_c_args,
  install: true,
)
