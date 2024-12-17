project('lab2', 'cpp')

executable(
	'task2',
	sources: [
		'task2/main.cpp',
	],
	cpp_args: [
		'-Wall',
		'-Wextra',
		'-Wpedantic',
	],
	override_options: {
		'cpp_std': 'c++20',
	}
)

executable(
	'task1',
	sources: [
		'task1/main.cpp',
		'task1/Student.cpp',
	],
	include_directories: [
		'task1',
	],
	cpp_args: [
		'-Wall',
		'-Wextra',
		'-Wpedantic',
	],
	override_options: {
		'cpp_std': 'c++20',
	}
)