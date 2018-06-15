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
	colors.cpp colors.hpp
	log.cpp log.hpp
	threadable.cpp threadable.hpp
	fsm.cpp fsm.hpp
#	enum.hpp
#	nlohmann/json.hpp
	bryllite-common.cpp bryllite-common.hpp
)

add_library( bryllite-common STATIC ${bryllite-common-sources} )
