
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

#ifndef REAK_STATE_ESTIMATOR_CONCEPT_HPP
#define REAK_STATE_ESTIMATOR_CONCEPT_HPP

#include "belief_state_concept.hpp"
#include "discrete_sss_concept.hpp"
#include "state_vector_concept.hpp"

namespace ReaK {

namespace ctrl {

template <typename StateEstimator>
struct state_estimator_traits {
  typedef typename StateEstimator::belief_type belief_type;
  typedef typename StateEstimator::system_type system_type;
  typedef typename belief_state_traits<belief_type>::state_type state_type;
  
  BOOST_STATIC_CONSTANT(std::size_t, dimensions = ss_system_traits<system_type>::dimensions);
  
};


template <typename StateEstimator>
struct StateEstimatorConcept {
  StateEstimator e;
  
  typename state_estimator_traits<StateEstimator>::belief_type b;
  typedef typename state_estimator_traits<StateEstimator>::system_type system_type;
  system_type sys;
  typename discrete_sss_traits<system_type>::input_type u;
  typename discrete_sss_traits<system_type>::output_type y;
  typename discrete_sss_traits<system_type>::time_type t;
  
  BOOST_CONCEPT_ASSERT((BeliefStateConcept< typename state_estimator_traits<StateEstimator>::belief_type >));
  BOOST_CONCEPT_ASSERT((DiscreteSSSConcept< typename state_estimator_traits<StateEstimator>::system_type >));
  
  void constraints() {
    b = e.predict_next_belief(b,u,t);
    b = e.update_belief(b,u,y,t);
  };
  
};




};

};


#endif


























