/*
@file: timer.hpp
@desc: timer
@author: jade@bryllite.com
@date: 2018-05-14
@note: timer 
*/

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_TIMER_HPP_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_TIMER_HPP_

#include <chrono>
#include <ctime>
#include <time.h>
#include <map>

#include "utils.hpp"
#include "lockable.hpp"

// namespace bryllite
namespace bryllite {

// timer class
class timer
{
public:
	// ctor & dtor
	timer();
	virtual ~timer();

	// get timestamp in milliseconds
	static time_t timestamp(void);

	// get elapsed time in msec
	time_t laptime(void);

	// get timestamp for id marked
	time_t timestamp_for(int id);

	// get laptime for id marked
	time_t laptime_for(int id);

	// mark timestamp for id
	time_t mark_for(int id);

	void reset(void);

protected:
	time_t m_beginTime ;
	std::map< int, time_t > m_marked ;
};

// timestamp
time_t timestamp(void);
typedef int timer_id;

// timer callback interface
class timer_callback
{
public:
	virtual void onTimeOut( timer_id id, void* context ) = 0;
};

// callback timer
class callback_timer : public lockable, public timer
{
protected:

	// timer item
	class timer_item
	{
	public:
		timer_id m_id;
		time_t m_timeout;
		timer_callback* m_callback;
		void* m_context;
		time_t m_beginTime;
		int m_repeat;		// repeat cnt: -1 means infinite

	public:
		timer_item(timer_id id, time_t timeout, timer_callback* callback, int repeat, void* context);
	};

public:
	callback_timer();

	// register callback
	bool callback( timer_id id, time_t timeout, timer_callback* pCallback, int repeat = -1, void* pContext = nullptr );

	void work( void );

protected:

	std::map< timer_id, timer_item > m_timers;
};

// get formatted date & time code with time shift in sec
// e.g. format="%y-%m-%d", output="2018-05-14"
// e.g. format="%h:%i:%s", output="04:54:30"
// e.g. format="%y-%m-%d %h:%i:%s", output="2018-05-14 04:54:30"
std::string dateTimeCode(const char* fmt = "%y-%m-%d %h:%i:%s", time_t time_shift_sec = 0);
std::string dateCode(const char* format = "%y-%m-%d", time_t time_shift_sec = 0);
std::string timeCode(const char* format = "%h:%i:%s", time_t time_shift_sec = 0);


}; //namespace bryllite

#endif //_BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_TIMER_HPP_
