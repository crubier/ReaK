/**
 * \file hyperball_topology.hpp
 * 
 * This library provides classes that define a hyper-ball vector-topology. A hyper-ball vector-topology is 
 * a vector-topology where the points are vector values and the boundary is a hyper-ellipsoid.
 * 
 * \author Sven Mikael Persson <mikael.s.persson@gmail.com>
 * \date October 2011
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

#ifndef REAK_HYPERBALL_TOPOLOGY_HPP
#define REAK_HYPERBALL_TOPOLOGY_HPP


#include "base/defs.hpp"

#include <boost/config.hpp> // For BOOST_STATIC_CONSTANT

#include "lin_alg/vect_concepts.hpp"

#include "vector_topology.hpp"

#include <cmath>
#include "base/named_object.hpp"
#include "lin_alg/mat_alg_identity.hpp"

#include "path_planning/global_rng.hpp"
#include "lin_alg/mat_cholesky.hpp"

namespace ReaK {

namespace pp {

/**
 * This library provides classes that define a hyper-ball vector-topology. A hyper-ball vector-topology is 
 * a vector-topology where the points are vector values and the boundary is a hyper-ellipsoid.
 * \tparam Vector The vector-type for the topology, should model an Arithmetic concept and WritableVectorConcept.
 * \tparam PDMatrix The matrix-type for the hyper-ellipsoid, should model the ReadableMatrixConcept.
 */
template <typename Vector, typename PDMatrix = mat< vect_traits<Vector>::value_type, mat_structure::identity > >
class hyperball_topology : public vector_topology<Vector>
{
  public:
    typedef hyperball_topology<Vector,PDMatrix> self;
    
    typedef Vector point_type;
    typedef Vector point_difference_type;
    
    typedef PDMatrix pd_matrix_type;
    
    BOOST_STATIC_CONSTANT(std::size_t, dimensions = vect_traits<Vector>::dimensions);
    
  protected:
    point_type center_point;
    double radius_value;
    pd_matrix_type scaling_mat;
    
  public:
    
    hyperball_topology(const std::string& aName = "hyperball_topology",
                       const point_type& aOrigin = point_type(),
		       double aRadius = 1.0,
		       const pd_matrix_type& aScaling = pd_matrix_type() ) : 
		       vector_topology<Vector>(aName),
		       center_point(aOrigin),
		       radius_value(aRadius),
		       scaling_mat(aScaling) { };
		       
		       
    /**
     * Returns the distance between two points.
     */
    double distance(const point_type& a, const point_type& b) const 
    {
      return this->norm(this->difference(b,a));
    }
    
    /**
     * Returns the norm of the difference between two points.
     */
    double norm(const point_difference_type& delta) const {
      using std::sqrt;
      return sqrt(delta * (scaling_mat * delta));
    }
    
    /**
     * Generates a random point in the space, uniformly distributed.
     */
    point_type random_point() const {
      mat< typename mat_traits<pd_matrix_type>::value_type, mat_structure::square> L;
      decompose_Cholesky(scaling_mat,L);
      
      boost::variate_generator< pp::global_rng_type&, boost::normal_distribution<typename vect_traits<point_difference_type>::value_type> > var_rnd(pp::get_global_rng(), boost::normal_distribution<typename vect_traits<point_difference_type>::value_type>());
      
      point_difference_type dp = this->difference(center_point,center_point);
      for(typename vect_traits<point_difference_type>::size_type i = 0; i < dp.size(); ++i)
        dp[i] = var_rnd();
      
      using std::sqrt;
      double factor = boost::uniform_01<pp::global_rng_type&,double>(pp::get_global_rng())() * radius_value / sqrt(dp * dp);
    
      return this->adjust(center_point,(L * (factor * dp)));
    };

    /**
     * Takes a point and clips it to within this line-segment space.
     */
    point_type bound(const point_type& a) const {
      return this->adjust(a,this->get_diff_to_boundary(a));
    };

    /**
     * Returns the distance to the boundary of the space.
     */
    double distance_from_boundary(const point_type& a) const {
      point_difference_type c2a = this->difference(a,center_point);
      using std::fabs;
      return fabs(radius_value - this->norm(c2a));
    };
    
    /**
     * Returns the difference to the closest boundary.
     */
    point_difference_type get_diff_to_boundary(const point_type& a) const {
      point_difference_type c2a = this->difference(a,center_point);
      return (radius_value - this->norm(c2a)) * c2a;
    };

    /**
     * Returns the origin of the space (the lower-limit).
     */
    point_type origin() const {
      return center_point;
    };
    
    /**
     * Returns the radius of the space.
     */
    double get_radius() const {
      return radius_value;
    };

    
/*******************************************************************************
                   ReaK's RTTI and Serialization interfaces
*******************************************************************************/
    
    virtual void RK_CALL save(serialization::oarchive& A, unsigned int) const {
      ReaK::named_object::save(A,named_object::getStaticObjectType()->TypeVersion());
      A & RK_SERIAL_SAVE_WITH_NAME(center_point)
        & RK_SERIAL_SAVE_WITH_NAME(radius_value)
        & RK_SERIAL_SAVE_WITH_NAME(scaling_mat);
    };

    virtual void RK_CALL load(serialization::iarchive& A, unsigned int) {
      ReaK::named_object::load(A,named_object::getStaticObjectType()->TypeVersion());
      A & RK_SERIAL_LOAD_WITH_NAME(center_point)
        & RK_SERIAL_LOAD_WITH_NAME(radius_value)
        & RK_SERIAL_LOAD_WITH_NAME(scaling_mat);
    };

    RK_RTTI_MAKE_CONCRETE_1BASE(self,0xC2400008,1,"hyperball_topology",vector_topology<Vector>)
    
};


};

};

#endif







