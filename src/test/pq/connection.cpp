// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq/connection.hpp>

void run()
{
   // overwrite the default with an environment variable if needed
   const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );

   // suppress false positive from clang-analyzer/clang-tidy
   (void)connection_string;

   // connection_string must be valid
   TEST_THROWS( tao::pq::connection::create( "=" ) );

   // connection_string must reference an existing and accessible database
   TEST_THROWS( tao::pq::connection::create( "dbname=DOES_NOT_EXIST" ) );

   // open a connection
   const auto connection = tao::pq::connection::create( connection_string );

   // open a second, independent connection (and discard it immediately)
   (void)tao::pq::connection::create( connection_string );

   // execute an SQL statement
   connection->execute( "DROP TABLE IF EXISTS tao_connection_test" );

   // execution of empty statements fails
   TEST_THROWS( connection->execute( "" ) );

   // execution of invalid statements fails
   TEST_THROWS( connection->execute( "FOO BAR BAZ" ) );

   // a prepared statement's name must be a valid C++ identifier
   TEST_THROWS( connection->prepare( "", "DROP TABLE IF EXISTS tao_connection_test" ) );
   TEST_THROWS( connection->prepare( "0drop_table", "DROP TABLE IF EXISTS tao_connection_test" ) );
   TEST_THROWS( connection->prepare( "drop table", "DROP TABLE IF EXISTS tao_connection_test" ) );

   // prepare a statement
   connection->prepare( "drop_table", "DROP TABLE IF EXISTS tao_connection_test" );

   // execute a prepared statement
   //
   // note: the name of a prepared statement must be a valid identifier, all
   // actual SQL statements can be writen in a form which does not match a valid
   // identifier, so you can always make sure that they can not be confused.
   connection->execute( "drop_table" );

   // a statement which is not a query does not return "affected rows"
   TEST_THROWS( connection->execute( "drop_table" ).rows_affected() );

   // deallocate a prepared statement
   connection->deallocate( "drop_table" );

   // no longer possible
   TEST_THROWS( connection->execute( "drop_table" ) );

   // deallocate must refer to a prepared statement
   TEST_THROWS( connection->deallocate( "drop_table" ) );

   // deallocate must get a valid name
   TEST_THROWS( connection->deallocate( "FOO BAR" ) );

   // test that prepared statement names are case sensitive
   connection->prepare( "a", "SELECT 1" );
   connection->prepare( "A", "SELECT 2" );

   TEST_THROWS( connection->prepare( "a", "SELECT 2" ) );

   TEST_ASSERT_MESSAGE( "checking prepared statement 'a'", connection->execute( "a" ).as< int >() == 1 );

   connection->deallocate( "a" );

   TEST_ASSERT_MESSAGE( "checking prepared statement 'A'", connection->execute( "A" ).as< int >() == 2 );

   connection->prepare( "a", "SELECT 3" );

   TEST_ASSERT_MESSAGE( "checking prepared statement 'a'", connection->execute( "a" ).as< int >() == 3 );
   TEST_ASSERT_MESSAGE( "checking prepared statement 'A'", connection->execute( "A" ).as< int >() == 2 );

   connection->deallocate( "A" );

   TEST_ASSERT_MESSAGE( "checking prepared statement 'a'", connection->execute( "a" ).as< int >() == 3 );

   connection->prepare( "A", "SELECT 4" );

   TEST_ASSERT_MESSAGE( "checking prepared statement 'a'", connection->execute( "a" ).as< int >() == 3 );
   TEST_ASSERT_MESSAGE( "checking prepared statement 'A'", connection->execute( "A" ).as< int >() == 4 );

   // create a test table
   connection->execute( "CREATE TABLE tao_connection_test ( a INTEGER PRIMARY KEY, b INTEGER )" );

   // a DELETE statement does not yield a result set
   TEST_THROWS( connection->execute( "DELETE FROM tao_connection_test" ).empty() );

   // out of range access throws
   TEST_THROWS( connection->execute( "SELECT * FROM tao_connection_test" ).at( 0 ) );

   // insert some data
   connection->execute( "INSERT INTO tao_connection_test VALUES ( 1, 42 )" );

   // read data
   TEST_ASSERT( connection->execute( "SELECT b FROM tao_connection_test WHERE a = 1" )[ 0 ][ 0 ].get() == std::string( "42" ) );

   TEST_THROWS( connection->execute( "SELECT $1", "" ).as< std::basic_string< unsigned char > >() );
   TEST_THROWS( connection->execute( "SELECT $1", "\\" ).as< std::basic_string< unsigned char > >() );
   TEST_THROWS( connection->execute( "SELECT $1", "\\xa" ).as< std::basic_string< unsigned char > >() );
   TEST_THROWS( connection->execute( "SELECT $1", "\\xa." ).as< std::basic_string< unsigned char > >() );

   TEST_THROWS( connection->execute( "SELECT $1", "" ).as< tao::pq::binary >() );
   TEST_THROWS( connection->execute( "SELECT $1", "\\" ).as< tao::pq::binary >() );
   TEST_THROWS( connection->execute( "SELECT $1", "\\xa" ).as< tao::pq::binary >() );
   TEST_THROWS( connection->execute( "SELECT $1", "\\xa." ).as< tao::pq::binary >() );
}

auto main() -> int  // NOLINT(bugprone-exception-escape)
{
   try {
      run();
   }
   // LCOV_EXCL_START
   catch( const std::exception& e ) {
      std::cerr << "exception: " << e.what() << std::endl;
      throw;
   }
   catch( ... ) {
      std::cerr << "unknown exception" << std::endl;
      throw;
   }
   // LCOV_EXCL_STOP
}
