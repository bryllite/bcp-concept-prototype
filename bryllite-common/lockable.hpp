/*
@file: lockable.hpp
@desc: smart lock for block scope & object lock
@author: jade@bryllite.com
@date: 2018-05-14
@note: smart lock for block scope & object lock
*/

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_LOCKABLE_HPP_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_LOCKABLE_HPP_

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <mutex>
#endif // _WIN32


#if defined(_WIN32) && (0)
#define _USE_CRITICAL_SECTION
#endif

// namespace bryllite
namespace bryllite {

// lockable : target object inherit lockable
class lockable
{
	friend class smart_lock ;

public:
	// ctor & dtor
	lockable();
	virtual ~lockable();

	// is locked?
	bool locked( void );

	// try_lock: 
	bool try_lock( void );

	void unlock( void );

protected:
#ifdef _USE_CRITICAL_SECTION
	CRITICAL_SECTION m_cs ;			// critical section is little bit fater than std::mutex 
#else // _USE_CRITICAL_SECTION
	std::recursive_mutex m_mutex ;	// recursive mutex lock
#endif // _USE_CRITICAL_SECTION
};

// lock exception : for deadlock detection
class lock_exception : public bryllite::exception
{
public:
	enum lock_error_code {
		E_SUCCESS = bryllite::exception::E_SUCCESS,
		E_NULLPTR,
		E_TIMEOUT,
		E_UNKNOWN,
		E_END
	};

public:
	lock_exception( int errorCode, std::string errorMessage );
};


// lock controller class
// timeout 0 means infinitely waiting
class smart_lock
{
public:
	// lock with ctor
//	smart_lock( lockable* pLockable, time_t timeout, const char* filename, int line );
	// lock with ctor
	smart_lock(lockable& lock, time_t timeout, const char* filename, int line);

	// unlock with dtor
	virtual ~smart_lock();

private:
//	lockable* m_pLockable ;
	lockable& m_lock;
};

}; // namespace bryllite

// lock macros
#define		smartlock( lock )				bryllite::smart_lock __smart_lock_object__( lock, 0, __FILENAME__, __LINE__ )
#define		try_smartlock( lock, timeout )	try { bryllite::smart_lock __try_smart_lock_object__( lock, timeout, __FILENAME__, __LINE__ ) ;
#define		catch_smartlock( e )			} catch( bryllite::lock_exception& e ) {
#define		end_smartlock()					}


#endif // _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_LOCKABLE_HPP_
