/*
@file: threadable.hpp
@desc: make class threadable
@author: jade@bryllite.com
@date: 2018-05-15
@note: 
*/

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_THREADABLE_HPP_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_THREADABLE_HPP_


// namespace bryllite
namespace bryllite {


// thread context for Thread Parameter
// user thread context must override this
class ThreadContext
{
	friend class threadable ;

public:
	ThreadContext();

	virtual ~ThreadContext();

	time_t touch(void);

	bool is_hang_for(time_t timeout);

	bool signaled(void);
	void signal(bool sig);
	std::thread::id get_thread_id(void);
	std::string get_thread_id_string(void);

protected:
	std::thread::id m_threadId ;	// thread id
	std::atomic<bool> m_signaled ;	// ready for start thread?
	time_t m_heartbeat ;
};


// make child class threadable
class threadable
{
	using ThreadContextPtr = std::unique_ptr< ThreadContext > ;		// ThreadContext* shared pointer
	using ThreadPtr = std::unique_ptr< std::thread > ;				// std::thread* shared pointer

public:
	threadable();

	virtual ~threadable();

	// join all threads : use this very carefully.
	void join(void);

	// public: run_thread_for()
	std::thread::id run_thread(ThreadContext* pContext = nullptr);

protected:
	// called when thread begin. before onThreadMain(). initialize here
	// return false means onThreadMain() & onThreadEnd() will not called
	virtual bool onThreadBegin(ThreadContext* pContext);

	// pure virtual. must be inplemented by child class
	// main procedure with thread
	virtual int onThreadMain( ThreadContext* pContext ) = 0;

	// called when thread end. after onThreadMain() terminated.
	// cleanup resources here.
	virtual void onThreadEnd(ThreadContext* pContext);

	// begin a thread with context
	// if pContext == nullptr, new ThreadContext will be allocated inside the code & delete on exit
	// returns std::thread::id for thread identification
	std::thread::id run_thread_for(ThreadContext* pContext);

	// get thread count
	size_t get_thread_count(void);

	// waiting for thread signaled
	bool wait_for_signaled(ThreadContext* pContext, time_t timeout = 0);

	lockable& getLockable(void);

	// keep going or not?
	bool keepGoing(void);
	void keepGoing(bool keepGoing);
	
	// touch thread for heatbeat check
	time_t touch(ThreadContext* pContext);

	// is hang for timeout in msec
	bool is_hang_for(ThreadContext* pContext, time_t timeout);

	// find std::thread* by std::thread::id
	std::thread* find(std::thread::id tid);

private:
	// thread procedure
	int _main(ThreadContext* pContext);

protected:
	lockable m_lock ;								// thread lock
	std::vector< ThreadPtr > m_threads ;			// thread list
	std::vector< ThreadContextPtr > m_contexts ;	// ThreadContext list ( just for automatically removing )
	std::atomic< bool > m_keepGoing ;				// keep going or not? ( for all child thread )
};


}; // namespace bryllite

#endif //_BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_THREADABLE_HPP_