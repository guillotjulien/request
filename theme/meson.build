sassc = find_program('sassc', required: true)

theme = custom_target('Theme generation',
	input: 'main.scss',
	output: 'style.css',
	command: [
		sassc,
		[ '-a', '-M', '-t', 'compact' ],
		'@INPUT@', '@OUTPUT@',
	],
	depend_files: files([
        '_colors.scss',
        'widgets/_request-response-bar.scss',
        'widgets/_request-url-bar.scss',
        'widgets/_request-double-entry.scss',
	]),
	build_by_default: true,
)
