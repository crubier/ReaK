
/*
 *    Copyright 2011 Sven Mikael Persson
 *
 *    THIS SOFTWARE IS DISTRIBUTED UNDER THE TERMS OF THE GNU GENERAL PUBLIC LICENSE v3 (GPLv3).
 *
 *    This file is part of ReaK.
 *
 *    ReaK is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    ReaK is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with ReaK (as LICENSE in the root folder).  
 *    If not, see <http://www.gnu.org/licenses/>.
 */

#include "ssv_recorder.hpp"
#include "tsv_recorder.hpp"
#include "bin_recorder.hpp"
#include "tcp_recorder.hpp"

#include <sstream>

#include "base/chrono_incl.hpp"

#define BOOST_TEST_DYN_LINK

#define BOOST_TEST_MODULE recorders
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>


BOOST_AUTO_TEST_CASE( ssv_record_extract_test )
{
  using namespace ReaK;
  using namespace recorder;
  
  {
    std::stringstream ss;
    {
      ssv_recorder output_rec;
      output_rec.setStream(ss);
      
      BOOST_CHECK_NO_THROW( output_rec << "x" << "2*x" << "x^2" );
      BOOST_CHECK_NO_THROW( output_rec << data_recorder::end_name_row );
      for(double x = 0; x < 10.1; x += 0.5) {
        BOOST_CHECK_NO_THROW( output_rec << x << 2*x << x*x );
        BOOST_CHECK_NO_THROW( output_rec << data_recorder::end_value_row );
      };
      BOOST_CHECK_NO_THROW( output_rec << data_recorder::flush );
    };
    
    {
      ssv_extractor input_rec;
      input_rec.setStream(ss);
      
      BOOST_CHECK_EQUAL( input_rec.getColCount(), 3 );
      
      std::string s1, s2, s3;
      BOOST_CHECK_NO_THROW( input_rec >> s1 >> s2 >> s3 );
      BOOST_CHECK( s1 == "x" );
      BOOST_CHECK( s2 == "2*x" );
      BOOST_CHECK( s3 == "x^2" );
      for(double x = 0; x < 10.1; x += 0.5) {
        double v1, v2, v3;
        BOOST_CHECK_NO_THROW( input_rec >> v1 >> v2 >> v3 );
        BOOST_CHECK_CLOSE( v1, x, 1e-6 );
        BOOST_CHECK_CLOSE( v2, (2.0*x), 1e-6 );
        BOOST_CHECK_CLOSE( v3, (x*x), 1e-6 );
        BOOST_CHECK_NO_THROW( input_rec >> data_extractor::end_value_row );
      };
      BOOST_CHECK_NO_THROW( input_rec >> data_extractor::close );
    };
    
  };
  
};


BOOST_AUTO_TEST_CASE( tsv_record_extract_test )
{
  using namespace ReaK;
  using namespace recorder;
  
  {
    std::stringstream ss;
    {
      tsv_recorder output_rec;
      output_rec.setStream(ss);
      
      BOOST_CHECK_NO_THROW( output_rec << "x" << "2*x" << "x^2" );
      BOOST_CHECK_NO_THROW( output_rec << data_recorder::end_name_row );
      for(double x = 0; x < 10.1; x += 0.5) {
        BOOST_CHECK_NO_THROW( output_rec << x << 2*x << x*x );
        BOOST_CHECK_NO_THROW( output_rec << data_recorder::end_value_row );
      };
      BOOST_CHECK_NO_THROW( output_rec << data_recorder::flush );
    };
    
    {
      tsv_extractor input_rec;
      input_rec.setStream(ss);
      
      BOOST_CHECK_EQUAL( input_rec.getColCount(), 3 );
      
      std::string s1, s2, s3;
      BOOST_CHECK_NO_THROW( input_rec >> s1 >> s2 >> s3 );
      BOOST_CHECK( s1 == "x" );
      BOOST_CHECK( s2 == "2*x" );
      BOOST_CHECK( s3 == "x^2" );
      for(double x = 0; x < 10.1; x += 0.5) {
        double v1, v2, v3;
        BOOST_CHECK_NO_THROW( input_rec >> v1 >> v2 >> v3 );
        BOOST_CHECK_CLOSE( v1, x, 1e-6 );
        BOOST_CHECK_CLOSE( v2, (2.0*x), 1e-6 );
        BOOST_CHECK_CLOSE( v3, (x*x), 1e-6 );
        BOOST_CHECK_NO_THROW( input_rec >> data_extractor::end_value_row );
      };
      BOOST_CHECK_NO_THROW( input_rec >> data_extractor::close );
    };
    
  };
  
};


BOOST_AUTO_TEST_CASE( bin_record_extract_test )
{
  using namespace ReaK;
  using namespace recorder;
  
  {
    std::stringstream ss;
    {
      bin_recorder output_rec;
      output_rec.setStream(ss);
      
      BOOST_CHECK_NO_THROW( output_rec << "x" << "2*x" << "x^2" );
      BOOST_CHECK_NO_THROW( output_rec << data_recorder::end_name_row );
      for(double x = 0; x < 10.1; x += 0.5) {
        BOOST_CHECK_NO_THROW( output_rec << x << 2*x << x*x );
        BOOST_CHECK_NO_THROW( output_rec << data_recorder::end_value_row );
      };
      BOOST_CHECK_NO_THROW( output_rec << data_recorder::flush );
    };
    
    {
      bin_extractor input_rec;
      input_rec.setStream(ss);
      
      BOOST_CHECK_EQUAL( input_rec.getColCount(), 3 );
      
      std::string s1, s2, s3;
      BOOST_CHECK_NO_THROW( input_rec >> s1 >> s2 >> s3 );
      BOOST_CHECK( s1 == "x" );
      BOOST_CHECK( s2 == "2*x" );
      BOOST_CHECK( s3 == "x^2" );
      for(double x = 0; x < 10.1; x += 0.5) {
        double v1, v2, v3;
        BOOST_CHECK_NO_THROW( input_rec >> v1 >> v2 >> v3 );
        BOOST_CHECK_CLOSE( v1, x, 1e-6 );
        BOOST_CHECK_CLOSE( v2, (2.0*x), 1e-6 );
        BOOST_CHECK_CLOSE( v3, (x*x), 1e-6 );
        BOOST_CHECK_NO_THROW( input_rec >> data_extractor::end_value_row );
      };
      BOOST_CHECK_NO_THROW( input_rec >> data_extractor::close );
    };
    
  };
  
};


struct server_runner {
  bool* succeeded;
  unsigned int* num_points;
  server_runner(bool* aSucceeded, unsigned int* aNumPoints) : succeeded(aSucceeded), num_points(aNumPoints) { };
  
  void operator()() {
    
    using namespace ReaK;
    using namespace recorder;
    
    try {
      tcp_recorder output_rec("17017");
      output_rec << "x" << "2*x" << "x^2" << data_recorder::end_name_row;
      
      for(double x = 0.0; x < 10.1; x += 0.5) {
        output_rec << x << 2*x << x*x << data_recorder::end_value_row;
        *num_points += 1;
      };
      output_rec << data_recorder::flush;
    } catch(...) {
      *succeeded = false;
      return;
    };
    
    *succeeded = true;
  };  
  
};


BOOST_AUTO_TEST_CASE( tcp_record_extract_test )
{
  
  using namespace ReaK;
  using namespace recorder;
  
  bool server_worked = false;
  unsigned int server_sent = 0;
  
  server_runner srv(&server_worked, &server_sent);
  ReaKaux::thread server_thd( srv );
  
  // it is necessary to give some time for the server to get up and waiting before a client can be created:
  ReaKaux::this_thread::sleep_for(ReaKaux::chrono::milliseconds(10));
  
  tcp_extractor input_rec("127.0.0.1:17017");
  
  BOOST_CHECK_EQUAL( input_rec.getColCount(), 3 );
  std::string s1, s2, s3;
  BOOST_CHECK_NO_THROW( input_rec >> s1 >> s2 >> s3 );
  BOOST_CHECK( s1 == "x" );
  BOOST_CHECK( s2 == "2*x" );
  BOOST_CHECK( s3 == "x^2" );
  
  for(double x = 0; x < 10.1; x += 0.5) {
    double v1, v2, v3;
    BOOST_CHECK_NO_THROW( input_rec >> v1 >> v2 >> v3 );
    BOOST_CHECK_CLOSE( v1, x, 1e-6 );
    BOOST_CHECK_CLOSE( v2, (2.0*x), 1e-6 );
    BOOST_CHECK_CLOSE( v3, (x*x), 1e-6 );
    BOOST_CHECK_NO_THROW( input_rec >> data_extractor::end_value_row );
  };
  
  BOOST_CHECK_NO_THROW( server_thd.join() );
  BOOST_CHECK_EQUAL( server_sent, 21 );
  BOOST_CHECK( server_worked );
  
};
















