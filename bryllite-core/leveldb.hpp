#pragma once

#include <leveldb/db.h>

// level db class
class CLevelDB
{
private:
	// level db instance
	std::unique_ptr<leveldb::DB> _db;

	// read options
	leveldb::ReadOptions _read_options;

	// write options
	leveldb::WriteOptions _write_options;

	// db options
	leveldb::Options _db_options;

	// db file path
	boost::filesystem::path _db_path;

public:
	static leveldb::ReadOptions defaultReadOptions(void);
	static leveldb::WriteOptions defaultWriteOptions(void);
	static leveldb::Options defaultDBOptions(void);

	CLevelDB();
	explicit CLevelDB(const boost::filesystem::path& path,
		leveldb::Options dbOptions = defaultDBOptions(),
		leveldb::ReadOptions readOptions = defaultReadOptions(),
		leveldb::WriteOptions writeOptions = defaultWriteOptions()
	);

	// open db
	bool Open(const boost::filesystem::path& path,
		leveldb::Options dbOptions = defaultDBOptions(),
		leveldb::ReadOptions readOptions = defaultReadOptions(),
		leveldb::WriteOptions writeOptions = defaultWriteOptions()
	);

	// close db
	bool Close(void);

	// is opened?
	bool isOpen(void) const;

	// set db ( insert, update )
//	bool Set(std::string key, std::string value);
	bool Set(leveldb::Slice key, leveldb::Slice value);

	// get db ( select )
//	bool Get(std::string key, std::string& value) const;
	bool Get(leveldb::Slice key, std::string& value) const;

	// delete db ( delete )
//	bool Delete(std::string key);
	bool Delete(leveldb::Slice key);

	// exists?
//	bool exists(std::string key) const;
	bool exists(leveldb::Slice key) const;

	leveldb::Iterator* newIterator(void);
};
