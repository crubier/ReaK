/**
 * \file joint_space_limits.hpp
 * 
 * This library provides classes to help create and manipulate joint-space topologies in over 
 * a joint-space with limits (speed, acceleration, and jerk limits).
 * 
 * \author Sven Mikael Persson <mikael.s.persson@gmail.com>
 * \date March 2012
 */

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

#ifndef REAK_JOINT_SPACE_LIMITS_HPP
#define REAK_JOINT_SPACE_LIMITS_HPP


#include "base/defs.hpp"

#include <boost/config.hpp> // For BOOST_STATIC_CONSTANT

#include "joint_space_topologies.hpp"
#include <boost/mpl/equal.hpp>
#include "se2_topologies.hpp"
#include "se3_topologies.hpp"

namespace ReaK {

namespace pp {

  
  
namespace detail {
  
  
  
  
/*******************************************************************************************************************
                                 FUNCTIONS TO CREATE RATE-LIMITED JOINT-SPACES
*******************************************************************************************************************/ 
  
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_joint_space< OutSpace >,
      boost::mpl::equal<
        max_derivation_order< InSpace, time_topology >,
	boost::mpl::size_t<0>
      >
    >,
  void >::type create_rl_joint_space_impl(OutSpace& space_out,
					  const InSpace& space_in,
					  const RateLimitMap& j_limits,
					  std::size_t& gen_i, std::size_t&, std::size_t&) {
    
    space_out = OutSpace(arithmetic_tuple<
                           line_segment_topology< typename RateLimitMap::value_type >
                         >(line_segment_topology< typename RateLimitMap::value_type >(
			     get<0>(space_in).getName() + "_rl",
			     ( get<0>(space_in).origin() - get<0>(space_in).get_radius() ) / j_limits.gen_speed_limits[gen_i],
		             ( get<0>(space_in).origin() + get<0>(space_in).get_radius() ) / j_limits.gen_speed_limits[gen_i]
			   )
			 )
		);
    ++gen_i;
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_joint_space< OutSpace >,
      boost::mpl::equal<
        max_derivation_order< InSpace, time_topology >,
	boost::mpl::size_t<1>
      >
    >,
  void >::type create_rl_joint_space_impl(OutSpace& space_out,
					  const InSpace& space_in,
					  const RateLimitMap& j_limits,
					  std::size_t& gen_i, std::size_t&, std::size_t&) {
    
    space_out = OutSpace(arithmetic_tuple<
                           line_segment_topology< typename RateLimitMap::value_type >,
                           line_segment_topology< typename RateLimitMap::value_type >
                         >(line_segment_topology< typename RateLimitMap::value_type >(
			     get<0>(space_in).getName() + "_rl",
			     ( get<0>(space_in).origin() - get<0>(space_in).get_radius() ) / j_limits.gen_speed_limits[gen_i],
		             ( get<0>(space_in).origin() + get<0>(space_in).get_radius() ) / j_limits.gen_speed_limits[gen_i]
			   ),
			   line_segment_topology< typename RateLimitMap::value_type >(
			     get<1>(space_in).getName() + "_rl",
			     ( get<1>(space_in).origin() - get<1>(space_in).get_radius() ) / j_limits.gen_accel_limits[gen_i],
		             ( get<1>(space_in).origin() + get<1>(space_in).get_radius() ) / j_limits.gen_accel_limits[gen_i]
			   )
			 ),
	                 euclidean_tuple_distance(),
	                 reach_time_differentiation_tuple< 1 >::type(
	                   reach_time_differentiation( j_limits.gen_speed_limits[gen_i] / j_limits.gen_accel_limits[gen_i] )
	                 )
		);
    ++gen_i;
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_joint_space< OutSpace >,
      boost::mpl::equal<
        max_derivation_order< InSpace, time_topology >,
	boost::mpl::size_t<2>
      >
    >,
  void >::type create_rl_joint_space_impl(OutSpace& space_out,
					  const InSpace& space_in,
					  const RateLimitMap& j_limits,
					  std::size_t& gen_i, std::size_t&, std::size_t&) {
    
    space_out = OutSpace(arithmetic_tuple<
                           line_segment_topology< typename RateLimitMap::value_type >,
                           line_segment_topology< typename RateLimitMap::value_type >,
                           line_segment_topology< typename RateLimitMap::value_type >
                         >(line_segment_topology< typename RateLimitMap::value_type >(
			     get<0>(space_in).getName() + "_rl",
			     ( get<0>(space_in).origin() - get<0>(space_in).get_radius() ) / j_limits.gen_speed_limits[gen_i],
		             ( get<0>(space_in).origin() + get<0>(space_in).get_radius() ) / j_limits.gen_speed_limits[gen_i]
			   ),
			   line_segment_topology< typename RateLimitMap::value_type >(
			     get<1>(space_in).getName() + "_rl",
			     ( get<1>(space_in).origin() - get<1>(space_in).get_radius() ) / j_limits.gen_accel_limits[gen_i],
		             ( get<1>(space_in).origin() + get<1>(space_in).get_radius() ) / j_limits.gen_accel_limits[gen_i]
			   ),
			   line_segment_topology< typename RateLimitMap::value_type >(
			     get<2>(space_in).getName() + "_rl",
			     ( get<2>(space_in).origin() - get<2>(space_in).get_radius() ) / j_limits.gen_jerk_limits[gen_i],
		             ( get<2>(space_in).origin() + get<2>(space_in).get_radius() ) / j_limits.gen_jerk_limits[gen_i]
			   )
			 ),
	                 euclidean_tuple_distance(),
	                 reach_time_differentiation_tuple< 2 >::type(
	                   reach_time_differentiation( j_limits.gen_speed_limits[gen_i] / j_limits.gen_accel_limits[gen_i] ),
	                   reach_time_differentiation( j_limits.gen_accel_limits[gen_i] / j_limits.gen_jerk_limits[gen_i] )
	                 )
		);
    ++gen_i;
  };
  
  
  
  
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_se2_space< OutSpace >,
      boost::mpl::equal<
        max_derivation_order< typename arithmetic_tuple_element< 0, InSpace >::type, time_topology >,
	boost::mpl::size_t<0>
      >
    >,
  void >::type create_rl_joint_space_impl(OutSpace& space_out,
					  const InSpace& space_in,
					  const RateLimitMap& j_limits,
					  std::size_t&, std::size_t& f2d_i, std::size_t&) {
    
    typedef typename RateLimitMap::value_type ValueType;
    typedef line_segment_topology< ValueType > LineSegTopo;
    typedef hyperbox_topology< vect<ValueType, 2> > BoxTopo;
    typedef arithmetic_tuple< LineSegTopo > RotTopoTuple;
    typedef arithmetic_tuple< BoxTopo > PosTopoTuple;
    typedef typename arithmetic_tuple_element<0, OutSpace>::type PosTopoOutType;
    typedef typename arithmetic_tuple_element<1, OutSpace>::type RotTopoOutType;
    
    space_out = OutSpace(arithmetic_tuple< PosTopoTuple, RotTopoTuple >(
                           PosTopoOutType(
			     PosTopoTuple(
			       BoxTopo(
			         get<0>(get<0>(space_in)).getName() + "_rl",
			         get<0>(get<0>(space_in)).get_lower_corner() * ( ValueType(1.0) / j_limits.frame2D_speed_limits[2 * f2d_i]),
			         get<0>(get<0>(space_in)).get_upper_corner() * ( ValueType(1.0) / j_limits.frame2D_speed_limits[2 * f2d_i])
			       )
			     )
			   ),
			   RotTopoOutType(
			     RotTopoTuple(
			       LineSegTopo(
			         get<0>(get<1>(space_in)).getName() + "_rl",
			         ( get<0>(get<1>(space_in)).origin() - get<0>(get<1>(space_in)).get_radius() ) / j_limits.frame2D_speed_limits[2 * f2d_i + 1],
		                 ( get<0>(get<1>(space_in)).origin() + get<0>(get<1>(space_in)).get_radius() ) / j_limits.frame2D_speed_limits[2 * f2d_i + 1]
			       )
			     )
			   )
			 )
		);
    f2d_i += 2;
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_se2_space< OutSpace >,
      boost::mpl::equal<
        max_derivation_order< typename arithmetic_tuple_element< 0, InSpace >::type, time_topology >,
	boost::mpl::size_t<1>
      >
    >,
  void >::type create_rl_joint_space_impl(OutSpace& space_out,
					  const InSpace& space_in,
					  const RateLimitMap& j_limits,
					  std::size_t&, std::size_t& f2d_i, std::size_t&) {
    
    typedef typename RateLimitMap::value_type ValueType;
    typedef line_segment_topology< ValueType > LineSegTopo;
    typedef hyperbox_topology< vect<ValueType, 2> > BoxTopo;
    typedef hyperball_topology< vect<ValueType, 2> > BallTopo;
    typedef arithmetic_tuple< LineSegTopo, LineSegTopo > RotTopoTuple;
    typedef arithmetic_tuple< BoxTopo, BallTopo > PosTopoTuple;
    typedef typename arithmetic_tuple_element<0, OutSpace>::type PosTopoOutType;
    typedef typename arithmetic_tuple_element<1, OutSpace>::type RotTopoOutType;
    
    space_out = OutSpace(arithmetic_tuple< PosTopoTuple, RotTopoTuple >(
                           PosTopoOutType(
			     PosTopoTuple(
			       BoxTopo(
			         get<0>(get<0>(space_in)).getName() + "_rl",
			         get<0>(get<0>(space_in)).get_lower_corner() * ( ValueType(1.0) / j_limits.frame2D_speed_limits[2 * f2d_i]),
			         get<0>(get<0>(space_in)).get_upper_corner() * ( ValueType(1.0) / j_limits.frame2D_speed_limits[2 * f2d_i])
			       ),
			       BallTopo(
			         get<1>(get<0>(space_in)).getName() + "_rl",
			         get<1>(get<0>(space_in)).origin() * ( ValueType(1.0) / j_limits.frame2D_accel_limits[2 * f2d_i]),
			         get<1>(get<0>(space_in)).get_radius() / j_limits.frame2D_accel_limits[2 * f2d_i]
			       )
			     ),
		             euclidean_tuple_distance(),
			     reach_time_differentiation_tuple< 1 >::type(
	                       reach_time_differentiation( j_limits.frame2D_speed_limits[2 * f2d_i] / j_limits.frame2D_accel_limits[2 * f2d_i] )
	                     )
			   ),
			   RotTopoOutType(
			     RotTopoTuple(
			       LineSegTopo(
			         get<0>(get<1>(space_in)).getName() + "_rl",
			         ( get<0>(get<1>(space_in)).origin() - get<0>(get<1>(space_in)).get_radius() ) / j_limits.frame2D_speed_limits[2 * f2d_i + 1],
		                 ( get<0>(get<1>(space_in)).origin() + get<0>(get<1>(space_in)).get_radius() ) / j_limits.frame2D_speed_limits[2 * f2d_i + 1]
			       ),
			       LineSegTopo(
			         get<1>(get<1>(space_in)).getName() + "_rl",
			         ( get<1>(get<1>(space_in)).origin() - get<1>(get<1>(space_in)).get_radius() ) / j_limits.frame2D_accel_limits[2 * f2d_i + 1],
		                 ( get<1>(get<1>(space_in)).origin() + get<1>(get<1>(space_in)).get_radius() ) / j_limits.frame2D_accel_limits[2 * f2d_i + 1]
			       )
			     ),
		             euclidean_tuple_distance(),
			     reach_time_differentiation_tuple< 1 >::type(
	                       reach_time_differentiation( j_limits.frame2D_speed_limits[2 * f2d_i + 1] / j_limits.frame2D_accel_limits[2 * f2d_i + 1] )
	                     )
			   )
			 )
		);
    f2d_i += 2;
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_se2_space< OutSpace >,
      boost::mpl::equal<
        max_derivation_order< typename arithmetic_tuple_element< 0, InSpace >::type, time_topology >,
	boost::mpl::size_t<2>
      >
    >,
  void >::type create_rl_joint_space_impl(OutSpace& space_out,
					  const InSpace& space_in,
					  const RateLimitMap& j_limits,
					  std::size_t&, std::size_t& f2d_i, std::size_t&) {
    
    typedef typename RateLimitMap::value_type ValueType;
    typedef line_segment_topology< ValueType > LineSegTopo;
    typedef hyperbox_topology< vect<ValueType, 2> > BoxTopo;
    typedef hyperball_topology< vect<ValueType, 2> > BallTopo;
    typedef arithmetic_tuple< LineSegTopo, LineSegTopo, LineSegTopo > RotTopoTuple;
    typedef arithmetic_tuple< BoxTopo, BallTopo, BallTopo > PosTopoTuple;
    typedef typename arithmetic_tuple_element<0, OutSpace>::type PosTopoOutType;
    typedef typename arithmetic_tuple_element<1, OutSpace>::type RotTopoOutType;
    
    space_out = OutSpace(arithmetic_tuple< PosTopoTuple, RotTopoTuple >(
                           PosTopoOutType(
			     PosTopoTuple(
			       BoxTopo(
			         get<0>(get<0>(space_in)).getName() + "_rl",
			         get<0>(get<0>(space_in)).get_lower_corner() * ( ValueType(1.0) / j_limits.frame2D_speed_limits[2 * f2d_i]),
			         get<0>(get<0>(space_in)).get_upper_corner() * ( ValueType(1.0) / j_limits.frame2D_speed_limits[2 * f2d_i])
			       ),
			       BallTopo(
			         get<1>(get<0>(space_in)).getName() + "_rl",
			         get<1>(get<0>(space_in)).origin() * ( ValueType(1.0) / j_limits.frame2D_accel_limits[2 * f2d_i]),
			         get<1>(get<0>(space_in)).get_radius() / j_limits.frame2D_accel_limits[2 * f2d_i]
			       ),
			       BallTopo(
			         get<2>(get<0>(space_in)).getName() + "_rl",
			         get<2>(get<0>(space_in)).origin() * ( ValueType(1.0) / j_limits.frame2D_jerk_limits[2 * f2d_i]),
			         get<2>(get<0>(space_in)).get_radius() / j_limits.frame2D_jerk_limits[2 * f2d_i]
			       )
			     ),
		             euclidean_tuple_distance(),
			     reach_time_differentiation_tuple< 2 >::type(
	                       reach_time_differentiation( j_limits.frame2D_speed_limits[2 * f2d_i] / j_limits.frame2D_accel_limits[2 * f2d_i] ),
	                       reach_time_differentiation( j_limits.frame2D_accel_limits[2 * f2d_i] / j_limits.frame2D_jerk_limits[2 * f2d_i] )
	                     )
			   ),
			   RotTopoOutType(
			     RotTopoTuple(
			       LineSegTopo(
			         get<0>(get<1>(space_in)).getName() + "_rl",
			         ( get<0>(get<1>(space_in)).origin() - get<0>(get<1>(space_in)).get_radius() ) / j_limits.frame2D_speed_limits[2 * f2d_i + 1],
		                 ( get<0>(get<1>(space_in)).origin() + get<0>(get<1>(space_in)).get_radius() ) / j_limits.frame2D_speed_limits[2 * f2d_i + 1]
			       ),
			       LineSegTopo(
			         get<1>(get<1>(space_in)).getName() + "_rl",
			         ( get<1>(get<1>(space_in)).origin() - get<1>(get<1>(space_in)).get_radius() ) / j_limits.frame2D_accel_limits[2 * f2d_i + 1],
		                 ( get<1>(get<1>(space_in)).origin() + get<1>(get<1>(space_in)).get_radius() ) / j_limits.frame2D_accel_limits[2 * f2d_i + 1]
			       ),
			       LineSegTopo(
			         get<2>(get<1>(space_in)).getName() + "_rl",
			         ( get<2>(get<1>(space_in)).origin() - get<2>(get<1>(space_in)).get_radius() ) / j_limits.frame2D_jerk_limits[2 * f2d_i + 1],
		                 ( get<2>(get<1>(space_in)).origin() + get<2>(get<1>(space_in)).get_radius() ) / j_limits.frame2D_jerk_limits[2 * f2d_i + 1]
			       )
			     ),
		             euclidean_tuple_distance(),
			     reach_time_differentiation_tuple< 2 >::type(
	                       reach_time_differentiation( j_limits.frame2D_speed_limits[2 * f2d_i + 1] / j_limits.frame2D_accel_limits[2 * f2d_i + 1] ),
	                       reach_time_differentiation( j_limits.frame2D_accel_limits[2 * f2d_i + 1] / j_limits.frame2D_jerk_limits[2 * f2d_i + 1] )
	                     )
			   )
			 )
		);
    f2d_i += 2;
  };
  
  
  
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_se3_space< OutSpace >,
      boost::mpl::equal<
        max_derivation_order< typename arithmetic_tuple_element< 0, InSpace >::type, time_topology >,
	boost::mpl::size_t<0>
      >
    >,
  void >::type create_rl_joint_space_impl(OutSpace& space_out,
					  const InSpace& space_in,
					  const RateLimitMap& j_limits,
					  std::size_t&, std::size_t&, std::size_t& f3d_i) {
    
    typedef typename RateLimitMap::value_type ValueType;
    typedef rate_limited_quat_space< ValueType > QuatTopo;
    typedef hyperbox_topology< vect<ValueType, 3> > BoxTopo;
    typedef arithmetic_tuple< QuatTopo > RotTopoTuple;
    typedef arithmetic_tuple< BoxTopo > PosTopoTuple;
    typedef typename arithmetic_tuple_element<0, OutSpace>::type PosTopoOutType;
    typedef typename arithmetic_tuple_element<1, OutSpace>::type RotTopoOutType;
    
    space_out = OutSpace(arithmetic_tuple< PosTopoTuple, RotTopoTuple >(
                           PosTopoOutType(
			     PosTopoTuple(
			       BoxTopo(
			         get<0>(get<0>(space_in)).getName() + "_rl",
			         get<0>(get<0>(space_in)).get_lower_corner() * ( ValueType(1.0) / j_limits.frame3D_speed_limits[2 * f3d_i]),
			         get<0>(get<0>(space_in)).get_upper_corner() * ( ValueType(1.0) / j_limits.frame3D_speed_limits[2 * f3d_i])
			       )
			     )
			   ),
			   RotTopoOutType(
			     RotTopoTuple(
			       QuatTopo(
			         get<0>(get<1>(space_in)).getName() + "_rl",
			         j_limits.frame3D_speed_limits[2 * f3d_i + 1]
			       )
			     )
			   )
			 )
		);
    f3d_i += 2;
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_se3_space< OutSpace >,
      boost::mpl::equal<
        max_derivation_order< typename arithmetic_tuple_element< 0, InSpace >::type, time_topology >,
	boost::mpl::size_t<1>
      >
    >,
  void >::type create_rl_joint_space_impl(OutSpace& space_out,
					  const InSpace& space_in,
					  const RateLimitMap& j_limits,
					  std::size_t&, std::size_t&, std::size_t& f3d_i) {
    
    typedef typename RateLimitMap::value_type ValueType;
    typedef rate_limited_quat_space< ValueType > QuatTopo;
    typedef ang_velocity_3D_topology< ValueType > AngVelTopo;
    typedef hyperbox_topology< vect<ValueType, 3> > BoxTopo;
    typedef hyperball_topology< vect<ValueType, 3> > BallTopo;
    typedef arithmetic_tuple< QuatTopo, AngVelTopo > RotTopoTuple;
    typedef arithmetic_tuple< BoxTopo, BallTopo > PosTopoTuple;
    typedef typename arithmetic_tuple_element<0, OutSpace>::type PosTopoOutType;
    typedef typename arithmetic_tuple_element<1, OutSpace>::type RotTopoOutType;
    
    space_out = OutSpace(arithmetic_tuple< PosTopoTuple, RotTopoTuple >(
                           PosTopoOutType(
			     PosTopoTuple(
			       BoxTopo(
			         get<0>(get<0>(space_in)).getName() + "_rl",
			         get<0>(get<0>(space_in)).get_lower_corner() * ( ValueType(1.0) / j_limits.frame3D_speed_limits[2 * f3d_i]),
			         get<0>(get<0>(space_in)).get_upper_corner() * ( ValueType(1.0) / j_limits.frame3D_speed_limits[2 * f3d_i])
			       ),
			       BallTopo(
			         get<1>(get<0>(space_in)).getName() + "_rl",
			         get<1>(get<0>(space_in)).origin() * ( ValueType(1.0) / j_limits.frame3D_accel_limits[2 * f3d_i]),
			         get<1>(get<0>(space_in)).get_radius() / j_limits.frame3D_accel_limits[2 * f3d_i]
			       )
			     ),
		             euclidean_tuple_distance(),
			     reach_time_differentiation_tuple< 1 >::type(
	                       reach_time_differentiation( j_limits.frame3D_speed_limits[2 * f3d_i] / j_limits.frame3D_accel_limits[2 * f3d_i] )
	                     )
			   ),
			   RotTopoOutType(
			     RotTopoTuple(
			       QuatTopo(
			         get<0>(get<1>(space_in)).getName() + "_rl",
			         j_limits.frame3D_speed_limits[2 * f3d_i + 1]
			       ),
			       AngVelTopo(
			         get<1>(get<1>(space_in)).getName() + "_rl",
			         get<1>(get<1>(space_in)).get_radius() / j_limits.frame3D_accel_limits[2 * f3d_i + 1]
		               )
			     ),
		             euclidean_tuple_distance(),
			     reach_time_differentiation_tuple< 1 >::type(
	                       reach_time_differentiation( j_limits.frame3D_speed_limits[2 * f3d_i + 1] / j_limits.frame3D_accel_limits[2 * f3d_i + 1] )
	                     )
			   )
			 )
		);
    f3d_i += 2;
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_se3_space< OutSpace >,
      boost::mpl::equal<
        max_derivation_order< typename arithmetic_tuple_element< 0, InSpace >::type, time_topology >,
	boost::mpl::size_t<2>
      >
    >,
  void >::type create_rl_joint_space_impl(OutSpace& space_out,
					  const InSpace& space_in,
					  const RateLimitMap& j_limits,
					  std::size_t&, std::size_t&, std::size_t& f3d_i) {
    
    typedef typename RateLimitMap::value_type ValueType;
    typedef rate_limited_quat_space< ValueType > QuatTopo;
    typedef ang_velocity_3D_topology< ValueType > AngVelTopo;
    typedef ang_accel_3D_topology< ValueType > AngAccTopo;
    typedef hyperbox_topology< vect<ValueType, 3> > BoxTopo;
    typedef hyperball_topology< vect<ValueType, 3> > BallTopo;
    typedef arithmetic_tuple< QuatTopo, AngVelTopo, AngAccTopo > RotTopoTuple;
    typedef arithmetic_tuple< BoxTopo, BallTopo, BallTopo > PosTopoTuple;
    typedef typename arithmetic_tuple_element<0, OutSpace>::type PosTopoOutType;
    typedef typename arithmetic_tuple_element<1, OutSpace>::type RotTopoOutType;
    
    space_out = OutSpace(arithmetic_tuple< PosTopoTuple, RotTopoTuple >(
                           PosTopoOutType(
			     PosTopoTuple(
			       BoxTopo(
			         get<0>(get<0>(space_in)).getName() + "_rl",
			         get<0>(get<0>(space_in)).get_lower_corner() * ( ValueType(1.0) / j_limits.frame3D_speed_limits[2 * f3d_i]),
			         get<0>(get<0>(space_in)).get_upper_corner() * ( ValueType(1.0) / j_limits.frame3D_speed_limits[2 * f3d_i])
			       ),
			       BallTopo(
			         get<1>(get<0>(space_in)).getName() + "_rl",
			         get<1>(get<0>(space_in)).origin() * ( ValueType(1.0) / j_limits.frame3D_accel_limits[2 * f3d_i]),
			         get<1>(get<0>(space_in)).get_radius() / j_limits.frame3D_accel_limits[2 * f3d_i]
			       ),
			       BallTopo(
			         get<2>(get<0>(space_in)).getName() + "_rl",
			         get<2>(get<0>(space_in)).origin() * ( ValueType(1.0) / j_limits.frame3D_jerk_limits[2 * f3d_i]),
			         get<2>(get<0>(space_in)).get_radius() / j_limits.frame3D_jerk_limits[2 * f3d_i]
			       )
			     ),
		             euclidean_tuple_distance(),
			     reach_time_differentiation_tuple< 2 >::type(
	                       reach_time_differentiation( j_limits.frame3D_speed_limits[2 * f3d_i] / j_limits.frame3D_accel_limits[2 * f3d_i] ),
	                       reach_time_differentiation( j_limits.frame3D_accel_limits[2 * f3d_i] / j_limits.frame3D_jerk_limits[2 * f3d_i] )
	                     )
			   ),
			   RotTopoOutType(
			     RotTopoTuple(
			       QuatTopo(
			         get<0>(get<1>(space_in)).getName() + "_rl",
			         j_limits.frame3D_speed_limits[2 * f3d_i + 1]
			       ),
			       AngVelTopo(
			         get<1>(get<1>(space_in)).getName() + "_rl",
			         get<1>(get<1>(space_in)).get_radius() / j_limits.frame3D_accel_limits[2 * f3d_i + 1]
		               ),
			       AngAccTopo(
			         get<2>(get<1>(space_in)).getName() + "_rl",
			         get<2>(get<1>(space_in)).get_radius() / j_limits.frame3D_jerk_limits[2 * f3d_i + 1]
		               )
			     ),
		             euclidean_tuple_distance(),
			     reach_time_differentiation_tuple< 2 >::type(
	                       reach_time_differentiation( j_limits.frame3D_speed_limits[2 * f3d_i + 1] / j_limits.frame3D_accel_limits[2 * f3d_i + 1] ),
	                       reach_time_differentiation( j_limits.frame3D_accel_limits[2 * f3d_i + 1] / j_limits.frame3D_jerk_limits[2 * f3d_i + 1] )
	                     )
			   )
			 )
		);
    f3d_i += 2;
  };
  
  
  template <typename Idx, typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::disable_if< 
    boost::mpl::or_<
      is_rate_limited_joint_space< OutSpace >,
      is_rate_limited_se2_space< OutSpace >,
      is_rate_limited_se3_space< OutSpace >
    >,
  void >::type create_rl_joint_space_impl(OutSpace& space_out,
					  const InSpace& space_in,
					  const RateLimitMap& j_limits,
					  std::size_t& gen_i, std::size_t& f2d_i, std::size_t& f3d_i) {
    create_rl_joint_spaces_impl< boost::mpl::prior< arithmetic_tuple_size< OutSpace > > >(space_out, space_in, j_limits, gen_i, f2d_i, f3d_i);
  };
  
  
  
  template <typename Idx, typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::disable_if< 
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_rl_joint_spaces_impl(OutSpace& space_out,
					   const InSpace& space_in,
					   const RateLimitMap& j_limits,
					   std::size_t& gen_i, std::size_t& f2d_i, std::size_t& f3d_i) {
    
    create_rl_joint_spaces_impl< boost::mpl::prior<Idx> >(space_out,space_in,j_limits,gen_i,f2d_i,f3d_i);
    
    create_rl_joint_space_impl(get< Idx::type::value >(space_out), 
                               get< Idx::type::value >(space_in),
			       j_limits, gen_i, f2d_i, f3d_i);
    
  };
  
  template <typename Idx, typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if< 
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_rl_joint_spaces_impl(OutSpace& space_out,
					   const InSpace& space_in,
					   const RateLimitMap& j_limits,
					   std::size_t& gen_i, std::size_t& f2d_i, std::size_t& f3d_i) {
    
    create_rl_joint_space_impl(get< 0 >(space_out), 
                               get< 0 >(space_in),
			       j_limits, gen_i, f2d_i, f3d_i);
    
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  void create_rl_joint_spaces_impl(OutSpace& space_out,
				   const InSpace& space_in,
				   const RateLimitMap& j_limits) {
    std::size_t gen_i = 0;
    std::size_t f2d_i = 0;
    std::size_t f3d_i = 0;
    create_rl_joint_spaces_impl< boost::mpl::prior< arithmetic_tuple_size< OutSpace > > >(get< 0 >(space_out), 
											  get< 0 >(space_in),
											  j_limits, gen_i, f2d_i, f3d_i);
    
  };
  
  
  
  
  
  
  
/*******************************************************************************************************************
                                 FUNCTIONS TO CREATE NORMAL JOINT-SPACES
*******************************************************************************************************************/ 
  
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_joint_space< InSpace >,
      boost::mpl::equal<
        max_derivation_order< InSpace, time_topology >,
	boost::mpl::size_t<0>
      >
    >,
  void >::type create_normal_joint_space_impl(OutSpace& space_out,
					      const InSpace& space_in,
					      const RateLimitMap& j_limits,
					      std::size_t& gen_i, std::size_t&, std::size_t&) {
    
    space_out = OutSpace(arithmetic_tuple<
                           line_segment_topology< typename RateLimitMap::value_type >
                         >(line_segment_topology< typename RateLimitMap::value_type >(
			     get<0>(space_in).getName() + "_non_rl",
			     ( get<0>(space_in).origin() - get<0>(space_in).get_radius() ) * j_limits.gen_speed_limits[gen_i],
		             ( get<0>(space_in).origin() + get<0>(space_in).get_radius() ) * j_limits.gen_speed_limits[gen_i]
			   )
			 )
		);
    ++gen_i;
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_joint_space< InSpace >,
      boost::mpl::equal<
        max_derivation_order< InSpace, time_topology >,
	boost::mpl::size_t<1>
      >
    >,
  void >::type create_normal_joint_space_impl(OutSpace& space_out,
					      const InSpace& space_in,
					      const RateLimitMap& j_limits,
					      std::size_t& gen_i, std::size_t&, std::size_t&) {
    
    space_out = OutSpace(arithmetic_tuple<
                           line_segment_topology< typename RateLimitMap::value_type >,
                           line_segment_topology< typename RateLimitMap::value_type >
                         >(line_segment_topology< typename RateLimitMap::value_type >(
			     get<0>(space_in).getName() + "_non_rl",
			     ( get<0>(space_in).origin() - get<0>(space_in).get_radius() ) * j_limits.gen_speed_limits[gen_i],
		             ( get<0>(space_in).origin() + get<0>(space_in).get_radius() ) * j_limits.gen_speed_limits[gen_i]
			   ),
			   line_segment_topology< typename RateLimitMap::value_type >(
			     get<1>(space_in).getName() + "_non_rl",
			     ( get<1>(space_in).origin() - get<1>(space_in).get_radius() ) * j_limits.gen_accel_limits[gen_i],
		             ( get<1>(space_in).origin() + get<1>(space_in).get_radius() ) * j_limits.gen_accel_limits[gen_i]
			   )
			 )
		);
    ++gen_i;
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_joint_space< InSpace >,
      boost::mpl::equal<
        max_derivation_order< InSpace, time_topology >,
	boost::mpl::size_t<2>
      >
    >,
  void >::type create_normal_joint_space_impl(OutSpace& space_out,
					      const InSpace& space_in,
					      const RateLimitMap& j_limits,
					      std::size_t& gen_i, std::size_t&, std::size_t&) {
    
    space_out = OutSpace(arithmetic_tuple<
                           line_segment_topology< typename RateLimitMap::value_type >,
                           line_segment_topology< typename RateLimitMap::value_type >,
                           line_segment_topology< typename RateLimitMap::value_type >
                         >(line_segment_topology< typename RateLimitMap::value_type >(
			     get<0>(space_in).getName() + "_non_rl",
			     ( get<0>(space_in).origin() - get<0>(space_in).get_radius() ) * j_limits.gen_speed_limits[gen_i],
		             ( get<0>(space_in).origin() + get<0>(space_in).get_radius() ) * j_limits.gen_speed_limits[gen_i]
			   ),
			   line_segment_topology< typename RateLimitMap::value_type >(
			     get<1>(space_in).getName() + "_non_rl",
			     ( get<1>(space_in).origin() - get<1>(space_in).get_radius() ) * j_limits.gen_accel_limits[gen_i],
		             ( get<1>(space_in).origin() + get<1>(space_in).get_radius() ) * j_limits.gen_accel_limits[gen_i]
			   ),
			   line_segment_topology< typename RateLimitMap::value_type >(
			     get<2>(space_in).getName() + "_non_rl",
			     ( get<2>(space_in).origin() - get<2>(space_in).get_radius() ) * j_limits.gen_jerk_limits[gen_i],
		             ( get<2>(space_in).origin() + get<2>(space_in).get_radius() ) * j_limits.gen_jerk_limits[gen_i]
			   )
			 )
		);
    ++gen_i;
  };
  
  
  
  
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_se2_space< InSpace >,
      boost::mpl::equal<
        max_derivation_order< typename arithmetic_tuple_element< 0, InSpace >::type, time_topology >,
	boost::mpl::size_t<0>
      >
    >,
  void >::type create_normal_joint_space_impl(OutSpace& space_out,
					      const InSpace& space_in,
					      const RateLimitMap& j_limits,
					      std::size_t&, std::size_t& f2d_i, std::size_t&) {
    
    typedef typename RateLimitMap::value_type ValueType;
    typedef line_segment_topology< ValueType > LineSegTopo;
    typedef hyperbox_topology< vect<ValueType, 2> > BoxTopo;
    typedef arithmetic_tuple< LineSegTopo > RotTopoTuple;
    typedef arithmetic_tuple< BoxTopo > PosTopoTuple;
    typedef typename arithmetic_tuple_element<0, OutSpace>::type PosTopoOutType;
    typedef typename arithmetic_tuple_element<1, OutSpace>::type RotTopoOutType;
    
    space_out = OutSpace(arithmetic_tuple< PosTopoTuple, RotTopoTuple >(
                           PosTopoOutType(
			     PosTopoTuple(
			       BoxTopo(
			         get<0>(get<0>(space_in)).getName() + "_non_rl",
			         get<0>(get<0>(space_in)).get_lower_corner() * j_limits.frame2D_speed_limits[2 * f2d_i],
			         get<0>(get<0>(space_in)).get_upper_corner() * j_limits.frame2D_speed_limits[2 * f2d_i]
			       )
			     )
			   ),
			   RotTopoOutType(
			     RotTopoTuple(
			       LineSegTopo(
			         get<0>(get<1>(space_in)).getName() + "_non_rl",
			         ( get<0>(get<1>(space_in)).origin() - get<0>(get<1>(space_in)).get_radius() ) * j_limits.frame2D_speed_limits[2 * f2d_i + 1],
		                 ( get<0>(get<1>(space_in)).origin() + get<0>(get<1>(space_in)).get_radius() ) * j_limits.frame2D_speed_limits[2 * f2d_i + 1]
			       )
			     )
			   )
			 )
		);
    f2d_i += 2;
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_se2_space< InSpace >,
      boost::mpl::equal<
        max_derivation_order< typename arithmetic_tuple_element< 0, InSpace >::type, time_topology >,
	boost::mpl::size_t<1>
      >
    >,
  void >::type create_normal_joint_space_impl(OutSpace& space_out,
					      const InSpace& space_in,
					      const RateLimitMap& j_limits,
					      std::size_t&, std::size_t& f2d_i, std::size_t&) {
    
    typedef typename RateLimitMap::value_type ValueType;
    typedef line_segment_topology< ValueType > LineSegTopo;
    typedef hyperbox_topology< vect<ValueType, 2> > BoxTopo;
    typedef hyperball_topology< vect<ValueType, 2> > BallTopo;
    typedef arithmetic_tuple< LineSegTopo, LineSegTopo > RotTopoTuple;
    typedef arithmetic_tuple< BoxTopo, BallTopo > PosTopoTuple;
    typedef typename arithmetic_tuple_element<0, OutSpace>::type PosTopoOutType;
    typedef typename arithmetic_tuple_element<1, OutSpace>::type RotTopoOutType;
    
    space_out = OutSpace(arithmetic_tuple< PosTopoTuple, RotTopoTuple >(
                           PosTopoOutType(
			     PosTopoTuple(
			       BoxTopo(
			         get<0>(get<0>(space_in)).getName() + "_non_rl",
			         get<0>(get<0>(space_in)).get_lower_corner() * j_limits.frame2D_speed_limits[2 * f2d_i],
			         get<0>(get<0>(space_in)).get_upper_corner() * j_limits.frame2D_speed_limits[2 * f2d_i]
			       ),
			       BallTopo(
			         get<1>(get<0>(space_in)).getName() + "_non_rl",
			         get<1>(get<0>(space_in)).origin() * j_limits.frame2D_accel_limits[2 * f2d_i],
			         get<1>(get<0>(space_in)).get_radius() * j_limits.frame2D_accel_limits[2 * f2d_i]
			       )
			     )
			   ),
			   RotTopoOutType(
			     RotTopoTuple(
			       LineSegTopo(
			         get<0>(get<1>(space_in)).getName() + "_non_rl",
			         ( get<0>(get<1>(space_in)).origin() - get<0>(get<1>(space_in)).get_radius() ) * j_limits.frame2D_speed_limits[2 * f2d_i + 1],
		                 ( get<0>(get<1>(space_in)).origin() + get<0>(get<1>(space_in)).get_radius() ) * j_limits.frame2D_speed_limits[2 * f2d_i + 1]
			       ),
			       LineSegTopo(
			         get<1>(get<1>(space_in)).getName() + "_non_rl",
			         ( get<1>(get<1>(space_in)).origin() - get<1>(get<1>(space_in)).get_radius() ) * j_limits.frame2D_accel_limits[2 * f2d_i + 1],
		                 ( get<1>(get<1>(space_in)).origin() + get<1>(get<1>(space_in)).get_radius() ) * j_limits.frame2D_accel_limits[2 * f2d_i + 1]
			       )
			     )
			   )
			 )
		);
    f2d_i += 2;
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_se2_space< InSpace >,
      boost::mpl::equal<
        max_derivation_order< typename arithmetic_tuple_element< 0, InSpace >::type, time_topology >,
	boost::mpl::size_t<2>
      >
    >,
  void >::type create_normal_joint_space_impl(OutSpace& space_out,
					      const InSpace& space_in,
					      const RateLimitMap& j_limits,
					      std::size_t&, std::size_t& f2d_i, std::size_t&) {
    
    typedef typename RateLimitMap::value_type ValueType;
    typedef line_segment_topology< ValueType > LineSegTopo;
    typedef hyperbox_topology< vect<ValueType, 2> > BoxTopo;
    typedef hyperball_topology< vect<ValueType, 2> > BallTopo;
    typedef arithmetic_tuple< LineSegTopo, LineSegTopo, LineSegTopo > RotTopoTuple;
    typedef arithmetic_tuple< BoxTopo, BallTopo, BallTopo > PosTopoTuple;
    typedef typename arithmetic_tuple_element<0, OutSpace>::type PosTopoOutType;
    typedef typename arithmetic_tuple_element<1, OutSpace>::type RotTopoOutType;
    
    space_out = OutSpace(arithmetic_tuple< PosTopoTuple, RotTopoTuple >(
                           PosTopoOutType(
			     PosTopoTuple(
			       BoxTopo(
			         get<0>(get<0>(space_in)).getName() + "_non_rl",
			         get<0>(get<0>(space_in)).get_lower_corner() * j_limits.frame2D_speed_limits[2 * f2d_i],
			         get<0>(get<0>(space_in)).get_upper_corner() * j_limits.frame2D_speed_limits[2 * f2d_i]
			       ),
			       BallTopo(
			         get<1>(get<0>(space_in)).getName() + "_non_rl",
			         get<1>(get<0>(space_in)).origin() * j_limits.frame2D_accel_limits[2 * f2d_i],
			         get<1>(get<0>(space_in)).get_radius() * j_limits.frame2D_accel_limits[2 * f2d_i]
			       ),
			       BallTopo(
			         get<2>(get<0>(space_in)).getName() + "_non_rl",
			         get<2>(get<0>(space_in)).origin() * j_limits.frame2D_jerk_limits[2 * f2d_i],
			         get<2>(get<0>(space_in)).get_radius() * j_limits.frame2D_jerk_limits[2 * f2d_i]
			       )
			     )
			   ),
			   RotTopoOutType(
			     RotTopoTuple(
			       LineSegTopo(
			         get<0>(get<1>(space_in)).getName() + "_non_rl",
			         ( get<0>(get<1>(space_in)).origin() - get<0>(get<1>(space_in)).get_radius() ) * j_limits.frame2D_speed_limits[2 * f2d_i + 1],
		                 ( get<0>(get<1>(space_in)).origin() + get<0>(get<1>(space_in)).get_radius() ) * j_limits.frame2D_speed_limits[2 * f2d_i + 1]
			       ),
			       LineSegTopo(
			         get<1>(get<1>(space_in)).getName() + "_non_rl",
			         ( get<1>(get<1>(space_in)).origin() - get<1>(get<1>(space_in)).get_radius() ) * j_limits.frame2D_accel_limits[2 * f2d_i + 1],
		                 ( get<1>(get<1>(space_in)).origin() + get<1>(get<1>(space_in)).get_radius() ) * j_limits.frame2D_accel_limits[2 * f2d_i + 1]
			       ),
			       LineSegTopo(
			         get<2>(get<1>(space_in)).getName() + "_non_rl",
			         ( get<2>(get<1>(space_in)).origin() - get<2>(get<1>(space_in)).get_radius() ) * j_limits.frame2D_jerk_limits[2 * f2d_i + 1],
		                 ( get<2>(get<1>(space_in)).origin() + get<2>(get<1>(space_in)).get_radius() ) * j_limits.frame2D_jerk_limits[2 * f2d_i + 1]
			       )
			     )
			   )
			 )
		);
    f2d_i += 2;
  };
  
  
  
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_se3_space< InSpace >,
      boost::mpl::equal<
        max_derivation_order< typename arithmetic_tuple_element< 0, InSpace >::type, time_topology >,
	boost::mpl::size_t<0>
      >
    >,
  void >::type create_normal_joint_space_impl(OutSpace& space_out,
					      const InSpace& space_in,
					      const RateLimitMap& j_limits,
					      std::size_t&, std::size_t&, std::size_t& f3d_i) {
    
    typedef typename RateLimitMap::value_type ValueType;
    typedef quaternion_topology< ValueType > QuatTopo;
    typedef hyperbox_topology< vect<ValueType, 3> > BoxTopo;
    typedef arithmetic_tuple< QuatTopo > RotTopoTuple;
    typedef arithmetic_tuple< BoxTopo > PosTopoTuple;
    typedef typename arithmetic_tuple_element<0, OutSpace>::type PosTopoOutType;
    typedef typename arithmetic_tuple_element<1, OutSpace>::type RotTopoOutType;
    
    space_out = OutSpace(arithmetic_tuple< PosTopoTuple, RotTopoTuple >(
                           PosTopoOutType(
			     PosTopoTuple(
			       BoxTopo(
			         get<0>(get<0>(space_in)).getName() + "_non_rl",
			         get<0>(get<0>(space_in)).get_lower_corner() * j_limits.frame3D_speed_limits[2 * f3d_i],
			         get<0>(get<0>(space_in)).get_upper_corner() * j_limits.frame3D_speed_limits[2 * f3d_i]
			       )
			     )
			   ),
			   RotTopoOutType(
			     RotTopoTuple(
			       QuatTopo(
			         get<0>(get<1>(space_in)).getName() + "_non_rl"
			       )
			     )
			   )
			 )
		);
    f3d_i += 2;
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_se3_space< InSpace >,
      boost::mpl::equal<
        max_derivation_order< typename arithmetic_tuple_element< 0, InSpace >::type, time_topology >,
	boost::mpl::size_t<1>
      >
    >,
  void >::type create_normal_joint_space_impl(OutSpace& space_out,
					      const InSpace& space_in,
					      const RateLimitMap& j_limits,
					      std::size_t&, std::size_t&, std::size_t& f3d_i) {
    
    typedef typename RateLimitMap::value_type ValueType;
    typedef quaternion_topology< ValueType > QuatTopo;
    typedef ang_velocity_3D_topology< ValueType > AngVelTopo;
    typedef hyperbox_topology< vect<ValueType, 3> > BoxTopo;
    typedef hyperball_topology< vect<ValueType, 3> > BallTopo;
    typedef arithmetic_tuple< QuatTopo, AngVelTopo > RotTopoTuple;
    typedef arithmetic_tuple< BoxTopo, BallTopo > PosTopoTuple;
    typedef typename arithmetic_tuple_element<0, OutSpace>::type PosTopoOutType;
    typedef typename arithmetic_tuple_element<1, OutSpace>::type RotTopoOutType;
    
    space_out = OutSpace(arithmetic_tuple< PosTopoTuple, RotTopoTuple >(
                           PosTopoOutType(
			     PosTopoTuple(
			       BoxTopo(
			         get<0>(get<0>(space_in)).getName() + "_non_rl",
			         get<0>(get<0>(space_in)).get_lower_corner() * j_limits.frame3D_speed_limits[2 * f3d_i],
			         get<0>(get<0>(space_in)).get_upper_corner() * j_limits.frame3D_speed_limits[2 * f3d_i]
			       ),
			       BallTopo(
			         get<1>(get<0>(space_in)).getName() + "_non_rl",
			         get<1>(get<0>(space_in)).origin() * j_limits.frame3D_accel_limits[2 * f3d_i],
			         get<1>(get<0>(space_in)).get_radius() * j_limits.frame3D_accel_limits[2 * f3d_i]
			       )
			     )
			   ),
			   RotTopoOutType(
			     RotTopoTuple(
			       QuatTopo(
			         get<0>(get<1>(space_in)).getName() + "_non_rl"
			       ),
			       AngVelTopo(
			         get<1>(get<1>(space_in)).getName() + "_non_rl",
			         get<1>(get<1>(space_in)).get_radius() * j_limits.frame3D_accel_limits[2 * f3d_i + 1]
		               )
			     )
			   )
			 )
		);
    f3d_i += 2;
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if<
    boost::mpl::and_<
      is_rate_limited_se3_space< InSpace >,
      boost::mpl::equal<
        max_derivation_order< typename arithmetic_tuple_element< 0, InSpace >::type, time_topology >,
	boost::mpl::size_t<2>
      >
    >,
  void >::type create_normal_joint_space_impl(OutSpace& space_out,
					      const InSpace& space_in,
					      const RateLimitMap& j_limits,
					      std::size_t&, std::size_t&, std::size_t& f3d_i) {
    
    typedef typename RateLimitMap::value_type ValueType;
    typedef quaternion_topology< ValueType > QuatTopo;
    typedef ang_velocity_3D_topology< ValueType > AngVelTopo;
    typedef ang_accel_3D_topology< ValueType > AngAccTopo;
    typedef hyperbox_topology< vect<ValueType, 3> > BoxTopo;
    typedef hyperball_topology< vect<ValueType, 3> > BallTopo;
    typedef arithmetic_tuple< QuatTopo, AngVelTopo, AngAccTopo > RotTopoTuple;
    typedef arithmetic_tuple< BoxTopo, BallTopo, BallTopo > PosTopoTuple;
    typedef typename arithmetic_tuple_element<0, OutSpace>::type PosTopoOutType;
    typedef typename arithmetic_tuple_element<1, OutSpace>::type RotTopoOutType;
    
    space_out = OutSpace(arithmetic_tuple< PosTopoTuple, RotTopoTuple >(
                           PosTopoOutType(
			     PosTopoTuple(
			       BoxTopo(
			         get<0>(get<0>(space_in)).getName() + "_non_rl",
			         get<0>(get<0>(space_in)).get_lower_corner() * j_limits.frame3D_speed_limits[2 * f3d_i],
			         get<0>(get<0>(space_in)).get_upper_corner() * j_limits.frame3D_speed_limits[2 * f3d_i]
			       ),
			       BallTopo(
			         get<1>(get<0>(space_in)).getName() + "_non_rl",
			         get<1>(get<0>(space_in)).origin() * j_limits.frame3D_accel_limits[2 * f3d_i],
			         get<1>(get<0>(space_in)).get_radius() * j_limits.frame3D_accel_limits[2 * f3d_i]
			       ),
			       BallTopo(
			         get<2>(get<0>(space_in)).getName() + "_non_rl",
			         get<2>(get<0>(space_in)).origin() * j_limits.frame3D_jerk_limits[2 * f3d_i],
			         get<2>(get<0>(space_in)).get_radius() * j_limits.frame3D_jerk_limits[2 * f3d_i]
			       )
			     )
			   ),
			   RotTopoOutType(
			     RotTopoTuple(
			       QuatTopo(
			         get<0>(get<1>(space_in)).getName() + "_non_rl"
			       ),
			       AngVelTopo(
			         get<1>(get<1>(space_in)).getName() + "_non_rl",
			         get<1>(get<1>(space_in)).get_radius() * j_limits.frame3D_accel_limits[2 * f3d_i + 1]
		               ),
			       AngAccTopo(
			         get<2>(get<1>(space_in)).getName() + "_non_rl",
			         get<2>(get<1>(space_in)).get_radius() * j_limits.frame3D_jerk_limits[2 * f3d_i + 1]
		               )
			     )
			   )
			 )
		);
    f3d_i += 2;
  };
  
  
  template <typename Idx, typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::disable_if< 
    boost::mpl::or_<
      is_rate_limited_joint_space< InSpace >,
      is_rate_limited_se2_space< InSpace >,
      is_rate_limited_se3_space< InSpace >
    >,
  void >::type create_normal_joint_space_impl(OutSpace& space_out,
					      const InSpace& space_in,
					      const RateLimitMap& j_limits,
					      std::size_t& gen_i, std::size_t& f2d_i, std::size_t& f3d_i) {
    create_normal_joint_spaces_impl< boost::mpl::prior< arithmetic_tuple_size< OutSpace > > >(space_out, space_in, j_limits, gen_i, f2d_i, f3d_i);
  };
  
  
  
  template <typename Idx, typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::disable_if< 
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_normal_joint_spaces_impl(OutSpace& space_out,
					       const InSpace& space_in,
					       const RateLimitMap& j_limits,
					       std::size_t& gen_i, std::size_t& f2d_i, std::size_t& f3d_i) {
    
    create_normal_joint_spaces_impl< boost::mpl::prior<Idx> >(space_out,space_in,j_limits,gen_i,f2d_i,f3d_i);
    
    create_normal_joint_space_impl(get< Idx::type::value >(space_out), 
                                   get< Idx::type::value >(space_in),
			           j_limits, gen_i, f2d_i, f3d_i);
    
  };
  
  template <typename Idx, typename OutSpace, typename InSpace, typename RateLimitMap>
  typename boost::enable_if< 
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_normal_joint_spaces_impl(OutSpace& space_out,
					       const InSpace& space_in,
					       const RateLimitMap& j_limits,
					       std::size_t& gen_i, std::size_t& f2d_i, std::size_t& f3d_i) {
    
    create_normal_joint_space_impl(get< 0 >(space_out), 
                                   get< 0 >(space_in),
			           j_limits, gen_i, f2d_i, f3d_i);
    
  };
  
  template <typename OutSpace, typename InSpace, typename RateLimitMap>
  void create_normal_joint_spaces_impl(OutSpace& space_out,
				       const InSpace& space_in,
				       const RateLimitMap& j_limits) {
    std::size_t gen_i = 0;
    std::size_t f2d_i = 0;
    std::size_t f3d_i = 0;
    create_normal_joint_spaces_impl< boost::mpl::prior< arithmetic_tuple_size< OutSpace > > >(get< 0 >(space_out), 
											      get< 0 >(space_in),
											      j_limits, gen_i, f2d_i, f3d_i);
    
  };
  
  
  
  
  
  
/*******************************************************************************************************************
                                 FUNCTIONS TO CREATE RATE-LIMITED JOINT-SPACE VECTORS
*******************************************************************************************************************/ 
  
  

  
  template <typename OutPoint, typename InPoint, typename RateLimitMap>
  void create_rl_joint_vectors_impl(OutPoint& result,
				    const InPoint& pt,
				    const RateLimitMap& j_limits) {
    std::size_t gen_i = 0;
    std::size_t f2d_i = 0;
    std::size_t f3d_i = 0;
    // TODO
  };
  
  
  
/*******************************************************************************************************************
                                 FUNCTIONS TO CREATE NORMAL JOINT-SPACE VECTORS
*******************************************************************************************************************/ 
  
  
  
  template <typename OutPoint, typename InPoint, typename RateLimitMap>
  void create_normal_joint_vectors_impl(OutPoint& result,
				        const InPoint& pt,
				        const RateLimitMap& j_limits) {
    std::size_t gen_i = 0;
    std::size_t f2d_i = 0;
    std::size_t f3d_i = 0;
    // TODO
  };
  
  
  
  
  
  
  template <typename Idx, typename PointType1, typename PointType2, typename Vector>
  typename boost::disable_if<
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_0th_rl_joint_vector_impl(PointType1& result, const PointType2& pt, const Vector& speed_limits) {
    create_0th_rl_joint_vector_impl< boost::mpl::prior<Idx> >(result, pt, speed_limits);
    
    get< 0 >(get< Idx::type::value >(result)) = get< 0 >(get< Idx::type::value >(pt)) / speed_limits[Idx::type::value];
  };
  
  template <typename Idx, typename PointType1, typename PointType2, typename Vector>
  typename boost::enable_if<
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_0th_rl_joint_vector_impl(PointType1& result, const PointType2& pt, const Vector& speed_limits) {
    get< 0 >(get< 0 >(result)) = get< 0 >(get< 0 >(pt)) / speed_limits[0];
  };
  
  
  
  template <typename Idx, typename PointType1, typename PointType2, typename Vector>
  typename boost::disable_if<
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_1st_rl_joint_vector_impl(PointType1& result, const PointType2& pt, 
					       const Vector& speed_limits, const Vector& accel_limits) {
    create_1st_rl_joint_vector_impl< boost::mpl::prior<Idx> >(result, pt, speed_limits, accel_limits);
    
    get< 0 >(get< Idx::type::value >(result)) = get< 0 >(get< Idx::type::value >(pt)) / speed_limits[Idx::type::value];
    get< 1 >(get< Idx::type::value >(result)) = get< 1 >(get< Idx::type::value >(pt)) / accel_limits[Idx::type::value];
  };
  
  template <typename Idx, typename PointType1, typename PointType2, typename Vector>
  typename boost::enable_if<
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_1st_rl_joint_vector_impl(PointType1& result, const PointType2& pt, 
					       const Vector& speed_limits, const Vector& accel_limits) {
    get< 0 >(get< 0 >(result)) = get< 0 >(get< 0 >(pt)) / speed_limits[0];
    get< 1 >(get< 0 >(result)) = get< 1 >(get< 0 >(pt)) / accel_limits[0];
  };
  
  
  
  
  template <typename Idx, typename PointType1, typename PointType2, typename Vector>
  typename boost::disable_if<
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_2nd_rl_joint_vector_impl(PointType1& result, const PointType2& pt, 
					       const Vector& speed_limits, 
					       const Vector& accel_limits, 
					       const Vector& jerk_limits) {
    create_2nd_rl_joint_vector_impl< boost::mpl::prior<Idx> >(result, pt, speed_limits, accel_limits, jerk_limits);
    
    get< 0 >(get< Idx::type::value >(result)) = get< 0 >(get< Idx::type::value >(pt)) / speed_limits[Idx::type::value];
    get< 1 >(get< Idx::type::value >(result)) = get< 1 >(get< Idx::type::value >(pt)) / accel_limits[Idx::type::value];
    get< 2 >(get< Idx::type::value >(result)) = get< 2 >(get< Idx::type::value >(pt)) / jerk_limits[Idx::type::value];
  };
  
  template <typename Idx, typename PointType1, typename PointType2, typename Vector>
  typename boost::enable_if<
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_2nd_rl_joint_vector_impl(PointType1& result, const PointType2& pt, 
					       const Vector& speed_limits, 
					       const Vector& accel_limits, 
					       const Vector& jerk_limits) {
    get< 0 >(get< 0 >(result)) = get< 0 >(get< 0 >(pt)) / speed_limits[0];
    get< 1 >(get< 0 >(result)) = get< 1 >(get< 0 >(pt)) / accel_limits[0];
    get< 2 >(get< 0 >(result)) = get< 2 >(get< 0 >(pt)) / jerk_limits[0];
  };
  
  
  
  
  template <typename Idx, typename PointType1, typename PointType2, typename Vector>
  typename boost::disable_if<
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_0th_joint_vector_impl(PointType1& result, const PointType2& pt, const Vector& speed_limits) {
    create_0th_joint_vector_impl< boost::mpl::prior<Idx> >(result, pt, speed_limits);
    
    get< 0 >(get< Idx::type::value >(result)) = get< 0 >(get< Idx::type::value >(pt)) * speed_limits[Idx::type::value];
  };
  
  template <typename Idx, typename PointType1, typename PointType2, typename Vector>
  typename boost::enable_if<
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_0th_joint_vector_impl(PointType1& result, const PointType2& pt, const Vector& speed_limits) {
    get< 0 >(get< 0 >(result)) = get< 0 >(get< 0 >(pt)) * speed_limits[0];
  };
  
  
  
  template <typename Idx, typename PointType1, typename PointType2, typename Vector>
  typename boost::disable_if<
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_1st_joint_vector_impl(PointType1& result, const PointType2& pt, 
					       const Vector& speed_limits, const Vector& accel_limits) {
    create_1st_joint_vector_impl< boost::mpl::prior<Idx> >(result, pt, speed_limits, accel_limits);
    
    get< 0 >(get< Idx::type::value >(result)) = get< 0 >(get< Idx::type::value >(pt)) * speed_limits[Idx::type::value];
    get< 1 >(get< Idx::type::value >(result)) = get< 1 >(get< Idx::type::value >(pt)) * accel_limits[Idx::type::value];
  };
  
  template <typename Idx, typename PointType1, typename PointType2, typename Vector>
  typename boost::enable_if<
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_1st_joint_vector_impl(PointType1& result, const PointType2& pt, 
					       const Vector& speed_limits, const Vector& accel_limits) {
    get< 0 >(get< 0 >(result)) = get< 0 >(get< 0 >(pt)) * speed_limits[0];
    get< 1 >(get< 0 >(result)) = get< 1 >(get< 0 >(pt)) * accel_limits[0];
  };
  
  
  
  
  template <typename Idx, typename PointType1, typename PointType2, typename Vector>
  typename boost::disable_if<
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_2nd_joint_vector_impl(PointType1& result, const PointType2& pt, 
					       const Vector& speed_limits, 
					       const Vector& accel_limits, 
					       const Vector& jerk_limits) {
    create_2nd_joint_vector_impl< boost::mpl::prior<Idx> >(result, pt, speed_limits, accel_limits, jerk_limits);
    
    get< 0 >(get< Idx::type::value >(result)) = get< 0 >(get< Idx::type::value >(pt)) * speed_limits[Idx::type::value];
    get< 1 >(get< Idx::type::value >(result)) = get< 1 >(get< Idx::type::value >(pt)) * accel_limits[Idx::type::value];
    get< 2 >(get< Idx::type::value >(result)) = get< 2 >(get< Idx::type::value >(pt)) * jerk_limits[Idx::type::value];
  };
  
  template <typename Idx, typename PointType1, typename PointType2, typename Vector>
  typename boost::enable_if<
    boost::mpl::less<
      Idx,
      boost::mpl::size_t<1>
    >,
  void >::type create_2nd_joint_vector_impl(PointType1& result, const PointType2& pt, 
					       const Vector& speed_limits, 
					       const Vector& accel_limits, 
					       const Vector& jerk_limits) {
    get< 0 >(get< 0 >(result)) = get< 0 >(get< 0 >(pt)) * speed_limits[0];
    get< 1 >(get< 0 >(result)) = get< 1 >(get< 0 >(pt)) * accel_limits[0];
    get< 2 >(get< 0 >(result)) = get< 2 >(get< 0 >(pt)) * jerk_limits[0];
  };
  
  
  
};






template <typename T>
struct joint_limits_collection {
  vect_n<T> gen_speed_limits;
  vect_n<T> gen_accel_limits;
  vect_n<T> gen_jerk_limits;
  vect_n<T> frame2D_speed_limits;
  vect_n<T> frame2D_accel_limits;
  vect_n<T> frame2D_jerk_limits;
  vect_n<T> frame3D_speed_limits;
  vect_n<T> frame3D_accel_limits;
  vect_n<T> frame3D_jerk_limits;
  
  typedef T value_type;
  
  template <typename NormalSpaceType>
  typename get_rate_limited_space< NormalSpaceType >::type make_rl_joint_space(const NormalSpaceType& j_space) {
    typename get_rate_limited_space< NormalSpaceType >::type result;
    detail::create_rl_joint_spaces_impl(result, j_space, *this);
    return result;
  };
  
  template <typename RateLimitedSpaceType>
  typename get_rate_illimited_space< RateLimitedSpaceType >::type make_normal_joint_space(const RateLimitedSpaceType& j_space) {
    typename get_rate_illimited_space< RateLimitedSpaceType >::type result;
    detail::create_normal_joint_spaces_impl(result, j_space, *this);
    return result;
  };
  
  template <typename NormalSpaceType>
  typename topology_traits< typename get_rate_limited_space< NormalSpaceType >::type >::point_type map_to_space(
      const typename topology_traits< NormalSpaceType >::point_type& pt,
      const NormalSpaceType& , const typename get_rate_limited_space< NormalSpaceType >::type& ) {
    typename topology_traits< typename get_rate_limited_space< NormalSpaceType >::type >::point_type result;
    detail::create_rl_joint_vectors_impl(result, pt, *this);
    return result;
  };
  
  
  template <typename RateLimitedSpaceType>
  typename topology_traits< typename get_rate_illimited_space< RateLimitedSpaceType >::type >::point_type map_to_space(
      const typename topology_traits< RateLimitedSpaceType >::point_type& pt,
      const RateLimitedSpaceType& , const typename get_rate_illimited_space< RateLimitedSpaceType >::type& ) {
    typename topology_traits< typename get_rate_illimited_space< RateLimitedSpaceType >::type >::point_type result;
    detail::create_normal_joint_vectors_impl(result, pt, *this);
    return result;
  };
  
};



template <typename T, std::size_t N, typename DistanceMetric = inf_norm_tuple_distance>
struct joint_limits_1st_order {
  vect<T, N> speed_limits;
  
  typedef typename metric_space_array< rl_joint_space_0th_order<T>::type, N, DistanceMetric >::type rate_limited_space_type;
  typedef typename metric_space_array< joint_space_0th_order<T>::type, N, DistanceMetric >::type normal_space_type;
  typedef T value_type;
  
  joint_limits_1st_order(const vect<T,N>& aSpeedLimits) : speed_limits(aSpeedLimits) { };
  
  rate_limited_space_type make_rl_joint_space(const normal_space_type& j_space) {
    rate_limited_space_type result;
    detail::create_0th_rl_joint_space_impl<boost::mpl::size_t<N-1>, T, N, DistanceMetric >(result, j_space, speed_limits);
    return result;
  };
  
  
  typename topology_traits< rate_limited_space_type >::point_type map_to_space(
      const typename topology_traits< normal_space_type >::point_type& pt,
      const normal_space_type& , const rate_limited_space_type& ) {
    typename topology_traits< rate_limited_space_type >::point_type result;
    detail::create_0th_rl_joint_vector_impl< boost::mpl::size_t<N-1> >(result, pt, speed_limits);
    return result;
  };
  
  
  typename topology_traits< normal_space_type >::point_type map_to_space(
      const typename topology_traits< rate_limited_space_type >::point_type& pt,
      const rate_limited_space_type& , const normal_space_type& ) {
    typename topology_traits< normal_space_type >::point_type result;
    detail::create_0th_joint_vector_impl< boost::mpl::size_t<N-1> >(result, pt, speed_limits);
    return result;
  };
  
};




template <typename T, std::size_t N, typename DistanceMetric = inf_norm_tuple_distance>
struct joint_limits_2nd_order {
  vect<T, N> speed_limits;
  vect<T, N> accel_limits;
  
  typedef typename metric_space_array< rl_joint_space_1st_order<T>::type, N, DistanceMetric >::type rate_limited_space_type;
  typedef typename metric_space_array< joint_space_1st_order<T>::type, N, DistanceMetric >::type normal_space_type;
  
  
  joint_limits_2nd_order(const vect<T,N>& aSpeedLimits, 
                         const vect<T,N>& aAccelLimits) : 
                         speed_limits(aSpeedLimits), 
                         accel_limits(aAccelLimits) { };
  
  rate_limited_space_type make_rl_joint_space(const normal_space_type& j_space) {
    rate_limited_space_type result;
    detail::create_1st_rl_joint_space_impl<boost::mpl::size_t<N-1>, T, N, DistanceMetric >(result, j_space, speed_limits);
    return result;
  };
  
  
  
  typename topology_traits< rate_limited_space_type >::point_type map_to_space(
      const typename topology_traits< normal_space_type >::point_type& pt,
      const normal_space_type& , const rate_limited_space_type& ) {
    typename topology_traits< rate_limited_space_type >::point_type result;
    detail::create_1st_rl_joint_vector_impl< boost::mpl::size_t<N-1> >(result, pt, speed_limits, accel_limits);
    return result;
  };
  
  
  typename topology_traits< normal_space_type >::point_type map_to_space(
      const typename topology_traits< rate_limited_space_type >::point_type& pt,
      const rate_limited_space_type& , const normal_space_type& ) {
    typename topology_traits< normal_space_type >::point_type result;
    detail::create_1st_joint_vector_impl< boost::mpl::size_t<N-1> >(result, pt, speed_limits, accel_limits);
    return result;
  };
  
};







template <typename T, std::size_t N, typename DistanceMetric = inf_norm_tuple_distance>
struct joint_limits_3rd_order {
  vect<T, N> speed_limits;
  vect<T, N> accel_limits;
  vect<T, N> jerk_limits;
  
  typedef typename metric_space_array< rl_joint_space_2nd_order<T>::type, N, DistanceMetric >::type rate_limited_space_type;
  typedef typename metric_space_array< joint_space_2nd_order<T>::type, N, DistanceMetric >::type normal_space_type;
  
  
  joint_limits_3rd_order(const vect<T,N>& aSpeedLimits, 
                         const vect<T,N>& aAccelLimits, 
                         const vect<T,N>& aJerkLimits) : 
                         speed_limits(aSpeedLimits), 
                         accel_limits(aAccelLimits), 
                         jerk_limits(aJerkLimits) { };
  
  rate_limited_space_type make_rl_joint_space(const normal_space_type& j_space) {
    rate_limited_space_type result;
    detail::create_2nd_rl_joint_space_impl<boost::mpl::size_t<N-1>, T, N, DistanceMetric >(result, j_space, speed_limits);
    return result;
  };
  
  
  typename topology_traits< rate_limited_space_type >::point_type map_to_space(
      const typename topology_traits< normal_space_type >::point_type& pt,
      const normal_space_type& , const rate_limited_space_type& ) {
    typename topology_traits< rate_limited_space_type >::point_type result;
    detail::create_2nd_rl_joint_vector_impl< boost::mpl::size_t<N-1> >(result, pt, speed_limits, accel_limits, jerk_limits);
    return result;
  };
  
  
  typename topology_traits< normal_space_type >::point_type map_to_space(
      const typename topology_traits< rate_limited_space_type >::point_type& pt,
      const rate_limited_space_type& , const normal_space_type& ) {
    typename topology_traits< normal_space_type >::point_type result;
    detail::create_2nd_joint_vector_impl< boost::mpl::size_t<N-1> >(result, pt, speed_limits, accel_limits, jerk_limits);
    return result;
  };
  
};



};



};

#endif








