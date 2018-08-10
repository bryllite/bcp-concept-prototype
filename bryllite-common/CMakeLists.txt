# source files
set( bryllite-common-sources
	types.hpp
	assert.cpp assert.hpp
	singleton.hpp
	utils.cpp utils.hpp
	version.hpp version.hpp.in
	timer.cpp timer.hpp
	exception.cpp exception.hpp
	lockable.cpp lockable.hpp
	clog.cpp clog.hpp
	threadable.cpp threadable.hpp
	bytestream.cpp bytestream.hpp
	bryllite-common.cpp bryllite-common.hpp
	stdafx.cpp stdafx.hpp
)

add_library( bryllite-common STATIC ${bryllite-common-sources} )
