#include "gtest/gtest.h"

using namespace testing;

#include "SQLiteDB.h"

class SQLiteTest : public Test
{
protected:
	virtual void SetUp() 
    {
    }

    virtual void TearDown() 
    {
    }
};

TEST_F(SQLiteTest, DB)
{
    SQLite::DB db;
    db.Open(":memory:",false);
    db.ExecDML("CREATE TABLE a(a);");
    EXPECT_TRUE(db.TableExists("a"));
    int count = db.ExecDML("INSERT INTO a VALUES(%1%);", 42);
    EXPECT_EQ(1, count);
}

TEST_F(SQLiteTest, Query)
{
    SQLite::DB db;
    db.Open(":memory:",false);
    db.ExecDML("CREATE TABLE a(a);");
    int count = db.ExecDML("INSERT INTO a VALUES(%1%),(%2%);", 42, 3);
    EXPECT_EQ(2, count);
    SQLite::Query q = db.ExecQuery("SELECT * FROM %1%","a");
    int sum = 0;
    for (; !q.IsEOF(); q.NextRow())
    {
        sum += q.GetIntField("a");
    }
    EXPECT_EQ(45, sum);
}

TEST_F(SQLiteTest, SingleInt64)
{
	SQLite::DB db;
	db.Open(":memory:", false);
	db.ExecDML("CREATE TABLE a(a);");
	EXPECT_TRUE(db.TableExists("a"));
	int count = db.ExecDML("INSERT INTO a VALUES(%1%);", 42);
	EXPECT_EQ(1, count);
	EXPECT_EQ(42, db.ExecSingleInt64("SELECT a FROM a;"));
}


