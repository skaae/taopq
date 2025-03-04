// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

void run()
{
   TEST_ASSERT( !tao::pq::internal::getenv( "PATH" ).empty() );
   TEST_THROWS( tao::pq::internal::getenv( "TAOPQ_DOESNOTEXIST" ) );

   TEST_ASSERT( !tao::pq::internal::getenv( "PATH", "" ).empty() );
   TEST_ASSERT( tao::pq::internal::getenv( "TAOPQ_DOESNOTEXIST", "" ).empty() );
   TEST_ASSERT( tao::pq::internal::getenv( "TAOPQ_DOESNOTEXIST", "DEFAULT VALUE" ) == "DEFAULT VALUE" );
}

auto main() -> int
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
