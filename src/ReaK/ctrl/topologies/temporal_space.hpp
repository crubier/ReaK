/**
 * \file temporal_space.hpp
 *
 * This library provides an implementation of a temporal-space which augments a
 * topology with a temporal dimension (time-stamp).
 *
 * \author Sven Mikael Persson <mikael.s.persson@gmail.com>
 * \date March 2011
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

#ifndef REAK_TEMPORAL_SPACE_HPP
#define REAK_TEMPORAL_SPACE_HPP

#include "base/defs.hpp"
#include <boost/config.hpp>

#include "path_planning/metric_space_concept.hpp"
#include "temporal_distance_metrics.hpp"
#include "default_random_sampler.hpp"

#include "base/named_object.hpp"

namespace ReaK {

namespace pp {

/**
 * This type represents the points of a temporal-space.
 */
template <typename SpacePoint, typename TimePoint = double>
struct temporal_point : public serialization::serializable {
  typedef temporal_point<SpacePoint,TimePoint> self;

  TimePoint time; ///< Holds the time associated to the space-time point.
  SpacePoint pt; ///< Holds the spatial-point associated to the space-time point.

  /**
   * Default constructor.
   */
  temporal_point() : time(), pt() { };
  /**
   * Constructor from a time and spatial point.
   * \param aTime The time associated to the space-time point.
   * \param aPt The spatial-point associated to the space-time point.
   */
  temporal_point(const TimePoint& aTime,
	         const SpacePoint& aPt) :
	         time(aTime), pt(aPt) { };

  temporal_point(const self& rhs) : time(rhs.time), pt(rhs.pt) { };

  self& operator=(const self& rhs) {
    time = rhs.time;
    pt = rhs.pt;
    return *this;
  };

#ifdef RK_ENABLE_CXX0X_FEATURES
  temporal_point(self&& rhs) : time(std::move(rhs.time)), pt(std::move(rhs.pt)) { };

  self& operator=(self&& rhs) {
    time = std::move(rhs.time);
    pt = std::move(rhs.pt);
    return *this;
  };
#endif


/*******************************************************************************
                   ReaK's RTTI and Serialization interfaces
*******************************************************************************/

    virtual void RK_CALL save(serialization::oarchive& A, unsigned int) const {
      A & RK_SERIAL_SAVE_WITH_NAME(time)
        & RK_SERIAL_SAVE_WITH_NAME(pt);
    };

    virtual void RK_CALL load(serialization::iarchive& A, unsigned int) {
      A & RK_SERIAL_LOAD_WITH_NAME(time)
        & RK_SERIAL_LOAD_WITH_NAME(pt);
    };

    RK_RTTI_MAKE_ABSTRACT_1BASE(self,0x0000002E,1,"temporal_point",serialization::serializable)
};

/**
 * This nested type represents the difference between two points of the temporal-space.
 */
template <typename SpaceDiff, typename TimeDiff = double>
struct temporal_point_difference : public serialization::serializable {
  typedef temporal_point_difference<SpaceDiff,TimeDiff> self;

  TimeDiff time; ///< Holds the time-difference.
  SpaceDiff pt; ///< Holds the spatial-difference.

  /**
   * Default constructor.
   */
  temporal_point_difference() : time(), pt() { };
  /**
   * Constructor from a time and space difference.
   * \param aTime The time difference.
   * \param aPt The spatial-difference.
   */
  temporal_point_difference(const TimeDiff& aTime,
	                    const SpaceDiff& aPt) :
	                    time(aTime), pt(aPt) { };

  temporal_point_difference(const self& rhs) : time(rhs.time), pt(rhs.pt) { };

  self& operator=(const self& rhs) {
    time = rhs.time;
    pt = rhs.pt;
    return *this;
  };

#ifdef RK_ENABLE_CXX0X_FEATURES
  temporal_point_difference(self&& rhs) : time(std::move(rhs.time)), pt(std::move(rhs.pt)) { };

  self& operator=(self&& rhs) {
    time = std::move(rhs.time);
    pt = std::move(rhs.pt);
    return *this;
  };
#endif

  self operator-() const {
    return self(-time,-pt);
  };

  friend self operator*(const self& a, double b) {
    return self(a.time * b, a.pt * b);
  };

  friend self operator*(double a, const self& b) {
    return self(a * b.time, a * b.pt);
  };

  self& operator+=(const self& b) {
    time += b.time;
    pt += b.pt;
    return *this;
  };

  self& operator-=(const self& b) {
    time -= b.time;
    pt -= b.pt;
    return *this;
  };

  friend self operator+(const self& a, const self& b) {
    self result;
    result.time = a.time + b.time;
    result.pt = a.pt + b.pt;
    return result;
  };

  friend self operator-(const self& a, const self& b) {
    self result;
    result.time = a.time - b.time;
    result.pt = a.pt - b.pt;
    return result;
  };


/*******************************************************************************
                   ReaK's RTTI and Serialization interfaces
*******************************************************************************/

    virtual void RK_CALL save(serialization::oarchive& A, unsigned int) const {
      A & RK_SERIAL_SAVE_WITH_NAME(time)
        & RK_SERIAL_SAVE_WITH_NAME(pt);
    };

    virtual void RK_CALL load(serialization::iarchive& A, unsigned int) {
      A & RK_SERIAL_LOAD_WITH_NAME(time)
        & RK_SERIAL_LOAD_WITH_NAME(pt);
    };

    RK_RTTI_MAKE_ABSTRACT_1BASE(self,0x0000002F,1,"temporal_point_difference",serialization::serializable)

};



/**
 * This class implementats a temporal-space which augments a
 * topology with a temporal dimension (time-stamp). The time-dimension resides on a line-segment
 * topology (see line_segment_topology), while the spatial topology and distance-metric
 * is provided by the user. Models the TemporalSpaceConcept.
 * \tparam Topology The topology type which represents the spatial dimensions, should model MetricSpaceConcept.
 * \tparam TimeTopology The topology type which represents the time dimension, should model MetricSpaceConcept.
 * \tparam TemporalDistanceMetric The distance metric type for the temporal-space, should model the TemporalDistMetricConcept.
 */
template <typename Topology, typename TimeTopology, typename TemporalDistanceMetric = spatial_distance_only>
class temporal_space : public named_object {
  public:
    typedef TimeTopology time_topology;
    typedef Topology space_topology;

    typedef TemporalDistanceMetric distance_metric_type;
    typedef default_random_sampler random_sampler_type;

    typedef temporal_space<Topology,TimeTopology,TemporalDistanceMetric> self;

  protected:
    space_topology space;
    time_topology time;
    distance_metric_type dist;

  public:
    /**
     * Parametrized constructor.
     * \param aSpace The space topology to be used.
     * \param aTime The time topology to be used.
     * \param aDist The temporal distance metric functor to use.
     */
    explicit temporal_space(const std::string& aName = "",
			    const space_topology& aSpace = space_topology(),
			    const time_topology& aTime = time_topology(),
			    const distance_metric_type& aDist = distance_metric_type()) :
                            named_object(), space(aSpace), time(aTime), dist(aDist) { this->setName(aName); };

    typedef temporal_point<typename topology_traits<space_topology>::point_type,
                           typename topology_traits<time_topology>::point_type> point_type;
    typedef temporal_point_difference<typename topology_traits<space_topology>::point_difference_type,
                                      typename topology_traits<time_topology>::point_difference_type> point_difference_type;
	
    BOOST_STATIC_CONSTANT(std::size_t, dimensions = topology_traits<space_topology>::dimensions + topology_traits<time_topology>::dimensions);

    /** Returns the underlying space topology. */
    const space_topology& get_space_topology() const { return space; };
    /** Returns the underlying time topology. */
    const time_topology& get_time_topology() const { return time; };
    /** Returns the temporal distance metric functor used. */
    const distance_metric_type& get_distance_metric() const { return dist; };

    friend
    const TemporalDistanceMetric& get(distance_metric_t,const self& space) {
      return space.dist;
    };


    /** Returns the underlying space topology. */
    space_topology& get_space_topology() { return space; };
    /** Returns the underlying time topology. */
    time_topology& get_time_topology() { return time; };
    /** Returns the temporal distance metric functor used. */
    distance_metric_type& get_distance_metric() { return dist; };

    friend
    TemporalDistanceMetric& get(distance_metric_t,self& space) {
      return space.dist;
    };

   /*************************************************************************
    *                             TopologyConcept
    * **********************************************************************/

    /**
     * Returns the difference between two points (a - b).
     * \param a The first point.
     * \param b The second point.
     * \return The difference between the two points.
     */
    point_difference_type difference(const point_type& a, const point_type& b) const {
      return point_difference_type(time.difference(a.time, b.time), space.difference(a.pt, b.pt));
    };

    /**
     * Returns the addition of a point-difference to a point.
     * \param a The starting point.
     * \param delta The point-difference.
     * \return The addition of a point-difference to a point.
     */
    point_type adjust(const point_type& a, const point_difference_type& delta) const {
      return point_type(time.adjust(a.time, delta.time), space.adjust(a.pt, delta.pt));
    };

    /**
     * Returns the origin of the temporal-space.
     * \return The origin of the temporal-space.
     */
    point_type origin() const {
      return point_type(time.origin(), space.origin());
    };

    /*************************************************************************
    *                             MetricSpaceConcept
    * **********************************************************************/

    /**
     * Computes the distance between two points in the temporal-space.
     * \param a The first point.
     * \param b The second point.
     * \return The distance between a and b.
     */
    double distance(const point_type& a, const point_type& b) const {
      return dist(a, b, *this);
    };

    /**
     * Computes the norm of a difference between two points.
     * \param a The difference between two points.
     * \return The norm of a difference between two points.
     */
    double norm(const point_difference_type& a) const {
      return dist(a, *this);
    };

    /*************************************************************************
    *                             LieGroupConcept
    * **********************************************************************/

    /**
     * Returns a point which is at a fraction between two points a to b.
     * \param a The first point.
     * \param fraction The fraction between the two points (0 to 1).
     * \param b The second point.
     * \return The point which is at a fraction between two points.
     */
    point_type move_position_toward(const point_type& a, double fraction, const point_type& b) const {
      return point_type(time.move_position_toward(a.time, fraction, b.time),
	                space.move_position_toward(a.pt, fraction, b.pt));
    };

    /*************************************************************************
    *                             PointDistributionConcept
    * **********************************************************************/

    /**
     * Returns a random point within the temporal-space.
     * \return A random point within the temporal-space.
     */
    point_type random_point() const
    {
      return point_type(get(random_sampler,time)(time), get(random_sampler,space)(space));
    };


/*******************************************************************************
                   ReaK's RTTI and Serialization interfaces
*******************************************************************************/

    virtual void RK_CALL save(serialization::oarchive& A, unsigned int) const {
      ReaK::named_object::save(A,named_object::getStaticObjectType()->TypeVersion());
      A & RK_SERIAL_SAVE_WITH_NAME(space)
        & RK_SERIAL_SAVE_WITH_NAME(time)
	& RK_SERIAL_SAVE_WITH_NAME(dist);
    };

    virtual void RK_CALL load(serialization::iarchive& A, unsigned int) {
      ReaK::named_object::load(A,named_object::getStaticObjectType()->TypeVersion());
      A & RK_SERIAL_LOAD_WITH_NAME(space)
        & RK_SERIAL_LOAD_WITH_NAME(time)
	& RK_SERIAL_LOAD_WITH_NAME(dist);
    };

    RK_RTTI_MAKE_CONCRETE_1BASE(self,0xC2400004,1,"temporal_space",named_object)

};

template <typename Topology, typename TimeTopology, typename TemporalDistanceMetric>
struct is_metric_space< temporal_space<Topology, TimeTopology, TemporalDistanceMetric> > : boost::mpl::and_< is_metric_space<Topology>, is_metric_space<TimeTopology> > { };

template <typename Topology, typename TimeTopology, typename TemporalDistanceMetric>
struct is_point_distribution< temporal_space<Topology, TimeTopology, TemporalDistanceMetric> > : boost::mpl::and_< is_point_distribution<Topology>, is_point_distribution<TimeTopology> > { };




};

};

#endif
















