// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef SRC_TEST_MACROS_HPP
#define SRC_TEST_MACROS_HPP

// This is an internal header used for unit-tests.

#include <cstdlib>
#include <iostream>

#include <tao/pq/internal/demangle.hpp>

#define STRINGIFY_INTERNAL( ... ) #__VA_ARGS__
#define STRINGIFY( ... ) STRINGIFY_INTERNAL( __VA_ARGS__ )
#define FILE_AND_LINE __FILE__ ":" STRINGIFY( __LINE__ )

#define TEST_EXECUTE_MESSAGE( MeSSaGe, ... )                                        \
   do {                                                                             \
      std::cout << "TEST [ " MeSSaGe << " ] in [ " FILE_AND_LINE " ]" << std::endl; \
      __VA_ARGS__;                                                                  \
   } while( false )

#define TEST_FAILED                                                     \
   do {                                                                 \
      std::cerr << "TEST FAILED in [ " FILE_AND_LINE " ]" << std::endl; \
      std::exit( 1 );                                                   \
   } while( false )

#define TEST_ASSERT_MESSAGE( MeSSaGe, ... )       \
   TEST_EXECUTE_MESSAGE(                          \
      MeSSaGe,                                    \
      if( !static_cast< bool >( __VA_ARGS__ ) ) { \
         TEST_FAILED;                             \
      } )

#define TEST_THROWS_MESSAGE( MeSSaGe, ... )                                                   \
   TEST_EXECUTE_MESSAGE(                                                                      \
      MeSSaGe,                                                                                \
      try {                                                                                   \
         __VA_ARGS__;                                                                         \
         TEST_FAILED;                                                                         \
      } catch( const std::exception& e ) {                                                    \
         std::cout << "TEST caught [ " << tao::pq::internal::demangle( typeid( e ) ) << " ] " \
                   << "with [ " << e.what() << " ] in [ " FILE_AND_LINE " ]" << std::endl;    \
      } catch( ... ) {                                                                        \
         std::cout << "TEST caught unknown exception in [ " FILE_AND_LINE " ]" << std::endl;  \
      } )

#define TEST_EXECUTE( ... ) TEST_EXECUTE_MESSAGE( "EXECUTE " #__VA_ARGS__, __VA_ARGS__ )
#define TEST_ASSERT( ... ) TEST_ASSERT_MESSAGE( "ASSERT " #__VA_ARGS__, __VA_ARGS__ )
#define TEST_THROWS( ... ) TEST_THROWS_MESSAGE( "THROWS " #__VA_ARGS__, (void)__VA_ARGS__ )

#endif
