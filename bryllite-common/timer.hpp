/*
@file: timer.hpp
@desc: timer
@author: jade@bryllite.com
@date: 2018-05-14
@note: timer 
*/

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_TIMER_HPP_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_TIMER_HPP_

typedef int timer_id;

// timer callback interface
class ICallbackTimer
{
public:
	virtual void onTimeOut( timer_id id, void* context ) = 0;
};



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

// callback timer
class callback_timer : public timer
{
protected:

	// timer item
	class timer_item
	{
	public:
		timer_id m_id;
		time_t m_timeout;
		ICallbackTimer* m_callback;
		void* m_context;
		time_t m_beginTime;
		int m_repeat;		// repeat cnt: -1 means infinite

	public:
		timer_item(timer_id id, time_t timeout, ICallbackTimer* callback, int repeat, void* context);
	};

public:
	callback_timer();

	// set timer callback
	bool set_timer( timer_id id, time_t timeout, ICallbackTimer* pCallback, int repeat = -1, void* pContext = nullptr );

	// stop timer callback
	void stop_timer( timer_id id );

	void work( void );

protected:
	lockable m_lock;
	std::map< timer_id, timer_item > m_timers;
};


// network time protocol timer
class ntp_timer
{
	enum { ntp_default_port = 123 };
protected:
	time_t _ntp_time;				// ntp get time (ms)
	time_t _local_timestamp;		// local time (ms)

	std::string _host;		// ntp host
	unsigned short _port;	// ntp port

	bool _sync;				// is time synchronized?

public:
	ntp_timer();
	ntp_timer(std::string host, unsigned short port);

	bool sync(time_t timeout);
	bool sync(std::string host, unsigned short port, time_t timeout);

	// time (sec)
	time_t time(void);

	// time (msec)
	time_t timestamp(void);

	// get time difference
	time_t time_diff( void );

	// get server time
	static time_t get_ntp_time(std::string host, unsigned short port);

protected:
	bool synchronize( std::string host, unsigned short port, time_t timeout );
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
