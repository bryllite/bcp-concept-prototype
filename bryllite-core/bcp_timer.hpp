#pragma once

// get local time stamp
time_t getLocalTimeStamp(void);

// remote time
class CRemoteTime
{
protected:
	time_t m_remoteTime;	// remote time (ms)
	time_t m_localTime;		// local time (ms)
	time_t m_moveTime;		// for time adjustment (ms))

public:
	// ctor
	CRemoteTime();
	CRemoteTime(time_t remoteTime);
	CRemoteTime(time_t remoteTime, time_t localTime);

	// get remote time
	time_t remoteTime(void) const;

	// move remote time
	time_t moveTime(time_t msec);

protected:
	// get local time
	time_t localTime(void) const;
};


// bcp timer
class CBcpTimer
{
protected:
	enum { DEFAULT_NTP_PORT = 123 };
	const time_t NTP_TIMESTAMP_DELTA = 2208988800ull;

	// ntp packet
	class ntp_packet
	{
	public:
		// ctor
		ntp_packet();
		ntp_packet(time_t localTimeMs, uint8_t stratumLevel=0);

		uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
								 // li.   Two bits.   Leap indicator.
								 // vn.   Three bits. Version number of the protocol.
								 // mode. Three bits. Client will pick mode 3 for client.

		uint8_t stratum;         // Eight bits. Stratum level of the local clock.
		uint8_t poll;            // Eight bits. Maximum interval between successive messages.
		uint8_t precision;       // Eight bits. Precision of the local clock.

		uint32_t rootDelay;      // 32 bits. Total round trip delay time.
		uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
		uint32_t refId;          // 32 bits. Reference clock identifier.

		uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
		uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

		uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
		uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

		uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
		uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

		uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
		uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

	};	// Total: 384 bits or 48 bytes.

public:
	// time server lists
	class TimeServerEntry
	{
	protected:
		std::string _host;
		unsigned short _port;
		time_t _timeout;
		uint8_t _stratum;

	public:
		TimeServerEntry(std::string host, uint8_t stratum = 0, unsigned short port = DEFAULT_NTP_PORT, time_t timeout = 1000);

		std::string host(void) const;
		unsigned short port(void) const;
		time_t timeout(void) const;
		uint8_t stratum(void) const;
	};

protected:
	CRemoteTime _remote_time;		// remote time
	CRemoteTime _temp_time;			// temp time for async recv

	bool _synchronized;				// synchronized?
	time_t _latency;				// one-way latency (ms)

	ntp_packet _receive_packet;		// receive packet

	std::vector< TimeServerEntry > _time_servers;	// time server entries

public:
	// ctor
	CBcpTimer();

	// local timestamp
	time_t localTime(void) const;

	// bryllite timestamp
	time_t getTime(void) const;

	// time shift in milliseconds
	void moveTime(time_t ms);

	// get time diff with localtime
	time_t diff(void) const;

	// add time server
	void addTimeServer(std::string host, uint8_t stratum = 0, unsigned short port = DEFAULT_NTP_PORT, time_t timeout = 200);

	// start time sync
	bool synchronize(void);
	bool synchronize(TimeServerEntry timeServer);
	bool sync_ok(void) const;

	// latency (ms) : travel_time/2
	time_t latency(void);

	// get date time string
	std::string dateTimeString(const char* format = "%y-%m-%d %h:%i:%s.%u", time_t time = 0);
	std::string dateString(const char* format = "%y-%m-%d", time_t time = 0);
	std::string timeString(const char* format = "%h:%i:%s.%u", time_t time = 0);

	time_t year(time_t time = 0);
	time_t month(time_t time = 0);
	time_t day(time_t time = 0);
	time_t hour(time_t time = 0);
	time_t minute(time_t time = 0);
	time_t second(time_t time = 0);
	time_t msec(time_t time = 0);

protected:
	// receive time from time server
	bool read_ntp_time(std::string host, short int port, uint8_t stratum, time_t timeout_ms);

	// receive handler
	void handle_receive(const boost::system::error_code& error, size_t bytes_transferred);
};
