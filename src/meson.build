request_sources = [
  'main.c',
  'request-window.c',
  'request-url-bar.c',
  'request-response-bar.c',
  'request-header-list.c',
  'request-double-entry.c',
  'request-response-panel.c',
  'request-source-view.c',
]

request_deps = [
  dependency('glib-2.0', version: '>= 2.66'),
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('gtk4', version: '>= 4.0'),
  dependency('libsoup-2.4'),
  dependency('liburiparser'),
  dependency('gtksourceview-5'),
  dependency('jansson'),
]

# Only needed for development
find_program('uncrustify', required : false)

gnome = import('gnome')

request_sources += gnome.compile_resources('request-resources',
  'resources/request.gresource.xml',
  dependencies: [ theme ],
  c_name: 'request'
)

executable('request', request_sources,
  dependencies: request_deps,
  install: true,
)

# highlight
# libgtksourceview3
