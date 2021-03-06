/**
 * \file anytime_sbastar.hpp
 *
 * This library provides function templates and concepts that implement an Anytime Sampling-based A* search
 * algorithm. A ASBA* uses the Anytime A* search algorithm to drive the expansion of a roadmap into the free-space 
 * in order to connect a start and goal location. This algorithm has many customization points because there 
 * are many choices to be made in the method, such as how to find nearest neighbors for attempting to 
 * connect them through free-space, how to expand vertices, when to stop the algorithm, etc. 
 * All these customization points are left to the user to implement, some are defined by the 
 * ASBAStarVisitorConcept (random-walk, edge-added, etc.).
 *
 * The ASBA* algorithm is a generalization of the Anytime A* algorithm where the neighborhood of a given node of 
 * the motion graph is not defined as a fixed set of neighbors (as in a classic A* over a fixed graph),
 * but rather as a region from which samples can be drawn (biased or not). In an ordinary A* algorithm,
 * vertices are closed when their entire neighborhood has been explored. In an ASBA* algorithm, the same 
 * criteria cannot apply since samples could be drawn ad infinitum, so, instead, this concept of the 
 * neighborhood being fully explored is derived from the expected information gained (or conversely, the 
 * "surprisal") from drawing a new sample in the neighborhood.
 * 
 * \author Sven Mikael Persson <mikael.s.persson@gmail.com>
 * \date March 2013
 */

/*
 *    Copyright 2013 Sven Mikael Persson
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

#ifndef REAK_ANYTIME_SBASTAR_HPP
#define REAK_ANYTIME_SBASTAR_HPP

#include <functional>
#include <utility>
#include <boost/tuple/tuple.hpp>
#include <boost/utility/enable_if.hpp>

#include "path_planning/metric_space_concept.hpp"
#include "path_planning/random_sampler_concept.hpp"

#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/detail/d_ary_heap.hpp>

#include "bgl_more_property_maps.hpp"
#include "bgl_more_property_tags.hpp"
#include "bgl_raw_property_graph.hpp"

#include "sbastar_search.hpp"
#include "sbastar_rrtstar.hpp"
#include "lazy_connector.hpp"
#include "branch_and_bound_connector.hpp"

/** Main namespace for ReaK */
namespace ReaK {

/** Main namespace for ReaK.Graph */
namespace graph {

/**
  * This concept class defines the valid expressions required of a class to be used as a visitor 
  * class for the ASBA* algorithm. A visitor class is essentially a class that regroups a number of 
  * callback functions that can be used to inject customization into the ASBA* algorithm. In other 
  * words, the visitor pattern in generic programming is an implementation of IoC 
  * (Inversion of Control), since the ASBA* algorithm is in control of execution, but custom behavior can
  * be injected in several places, even blocking the algorithm if needed.
  * 
  * Required concepts:
  * 
  * The visitor class should model SBAStarVisitorConcept, and AnytimeHeuristicVisitorConcept.
  * 
  * \tparam Visitor The visitor class to be tested for modeling an ASBA* visitor concept.
  * \tparam Graph The graph type on which the visitor should be able to act.
  * \tparam Topology The topology type on which the visitor class is required to work with.
  */
template <typename Visitor, typename Graph, typename Topology>
struct ASBAStarVisitorConcept : 
  SBAStarVisitorConcept<Visitor, Graph, Topology>, 
  AnytimeHeuristicVisitorConcept<Visitor,Graph> { };

/**
  * This class is simply an archetype visitor for the ASBA* algorithm. 
  */
template <typename Topology>
struct asbastar_visitor_archetype : 
  sbastar_visitor_archetype<Topology>,
  anytime_heuristic_visitor_archetype { };


/**
  * This concept class defines the valid expressions required of a class to be used as a visitor 
  * class for the ASBA*-RRT* algorithm. A visitor class is essentially a class that regroups a number of 
  * callback functions that can be used to inject customization into the ASBA*-RRT* algorithm. In other 
  * words, the visitor pattern in generic programming is an implementation of IoC 
  * (Inversion of Control), since the ASBA*-RRT* algorithm is in control of execution, but custom behavior can
  * be injected in several places, even blocking the algorithm if needed.
  * 
  * Required concepts:
  * 
  * The visitor class should model SBARRTStarVisitorConcept, and AnytimeHeuristicVisitorConcept.
  * 
  * \tparam Visitor The visitor class to be tested for modeling an ASBA*-RRT* visitor concept.
  * \tparam Graph The graph type on which the visitor should be able to act.
  * \tparam Topology The topology type on which the visitor class is required to work with.
  */
template <typename Visitor, typename Graph, typename Topology>
struct ASBARRTStarVisitorConcept : 
  SBARRTStarVisitorConcept<Visitor, Graph, Topology>, 
  AnytimeHeuristicVisitorConcept<Visitor,Graph> { };

/**
  * This class is simply an archetype visitor for the ASBA*-RRT* algorithm. 
  */
template <typename Topology>
struct asbarrtstar_visitor_archetype : 
  sbarrtstar_visitor_archetype<Topology>,
  anytime_heuristic_visitor_archetype { };




namespace detail {
  
  
  
  template <typename UniformCostVisitor,
            typename UpdatableQueue, 
            typename IndexInHeapMap,
            typename AStarHeuristicMap, 
            typename PositionMap, 
            typename WeightMap,
            typename DensityMap,
            typename ConstrictionMap, 
            typename DistanceMap,  
            typename PredecessorMap,
            typename KeyMap>
  struct anytime_sbarrtstar_bfs_visitor :
    sbastar_bfs_visitor<UniformCostVisitor, UpdatableQueue, IndexInHeapMap, 
                        AStarHeuristicMap, PositionMap, WeightMap, DensityMap, ConstrictionMap, 
                        DistanceMap, PredecessorMap, KeyMap>
  {
    typedef typename boost::property_traits<PositionMap>::value_type PositionValue;
    typedef sbastar_bfs_visitor<UniformCostVisitor, UpdatableQueue, IndexInHeapMap, 
                                AStarHeuristicMap, PositionMap, WeightMap, DensityMap, ConstrictionMap, 
                                DistanceMap, PredecessorMap, KeyMap> base_type;

    anytime_sbarrtstar_bfs_visitor(UniformCostVisitor vis, UpdatableQueue& Q, IndexInHeapMap index_in_heap,  
                                    AStarHeuristicMap heuristic, PositionMap pos, WeightMap weight, 
                                    DensityMap density, ConstrictionMap constriction, 
                                    DistanceMap dist, PredecessorMap pred, KeyMap key, double current_relaxation) :
                                    base_type(vis, Q, index_in_heap, heuristic, pos, weight, density, constriction, dist, pred, key),
                                    m_current_relaxation(current_relaxation) { };
    
    template <class Vertex, typename Graph>
    void update_key(Vertex u, Graph& g) const {
      this->m_vis.affected_vertex(u,g);
      double g_u = get(this->m_distance, g[u]);
      double h_u = get(this->m_heuristic, g[u]);
      // Key-value for the min-heap (priority-queue):
      put(this->m_key, u, ((g_u + h_u) / (1.0 - get(this->m_constriction, g[u]))) / (1.0 - get(this->m_density, g[u])) + m_current_relaxation * h_u);
    };
    
    template <typename Vertex, typename Graph>
    void requeue_vertex(Vertex u, Graph& g) const {
      update_key(u,g); 
      if( ! this->m_vis.should_close(u, g) ) {
        this->m_Q.push_or_update(u);
        this->m_vis.discover_vertex(u, g);
      };
    };
    template <typename Vertex, typename Graph>
    void affected_vertex(Vertex u, Graph& g) const { requeue_vertex(u,g); }; // same function, different name.
    
    template <typename Graph>
    void update_relaxation(const Graph& g) { 
      m_current_relaxation = this->m_vis.adjust_relaxation(m_current_relaxation, g);
      
      typedef typename boost::graph_traits<Graph>::vertex_iterator VIter;
      VIter vi, vi_end;
      for(boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi) {
        if(is_vertex_valid(*vi,g))
          requeue_vertex(*vi, g);
      };
    };
    
    template <typename Graph>
    void publish_path(const Graph& g) { 
      this->m_vis.publish_path(g);
      update_relaxation(g);
    };
    
    double m_current_relaxation;
  };
  
  
  template <typename Graph,
            typename Vertex,
            typename Topology,
            typename ASBAStarVisitor,
            typename NodeConnector,
            typename AStarHeuristicMap,
            typename PositionMap,
            typename WeightMap,
            typename DensityMap,
            typename ConstrictionMap,
            typename DistanceMap,
            typename PredecessorMap,
            typename KeyMap,
            typename NcSelector>
  inline void
  generate_anytime_sbastar_no_init_impl
    (Graph &g, Vertex start_vertex, const Topology& super_space, ASBAStarVisitor vis,
      NodeConnector connect_vertex, AStarHeuristicMap hval, PositionMap position, WeightMap weight,
      DensityMap density, ConstrictionMap constriction, DistanceMap distance,
      PredecessorMap predecessor, KeyMap key, NcSelector select_neighborhood, double init_relaxation)
  {
    typedef typename boost::property_traits<KeyMap>::value_type KeyValue;
    typedef std::less<double> KeyCompareType;  // <---- this is a min-heap.
    typedef boost::vector_property_map<std::size_t> IndexInHeapMap;
    IndexInHeapMap index_in_heap;
    {
      typename boost::graph_traits<Graph>::vertex_iterator ui, ui_end;
      for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
        put(index_in_heap,*ui, static_cast<std::size_t>(-1)); 
      };
    };
    
    typedef boost::d_ary_heap_indirect<Vertex, 4, IndexInHeapMap, KeyMap, KeyCompareType> MutableQueue;
    MutableQueue Q(key, index_in_heap, KeyCompareType()); //priority queue holding the OPEN set.
    
    anytime_sbarrtstar_bfs_visitor<
      ASBAStarVisitor,
      MutableQueue, 
      IndexInHeapMap,
      AStarHeuristicMap, 
      PositionMap, 
      WeightMap,
      DensityMap,
      ConstrictionMap, 
      DistanceMap,  
      PredecessorMap,
      KeyMap> sba_bfs_vis(vis, Q, index_in_heap, hval, position, weight, 
                          density, constriction, distance, predecessor, key, init_relaxation);
    
    put(distance, g[start_vertex], 0.0);
    
    sbastar_search_loop(g, start_vertex, super_space, sba_bfs_vis, 
                        connect_vertex, sba_node_generator(), 
                        Q, select_neighborhood);
    
  };
  
  
  template <typename Graph,
            typename Vertex,
            typename Topology,
            typename SBARRTStarVisitor,
            typename NodeConnector,
            typename AStarHeuristicMap,
            typename PositionMap,
            typename WeightMap,
            typename DensityMap,
            typename ConstrictionMap,
            typename DistanceMap,
            typename PredecessorMap,
            typename KeyMap,
            typename RandomSampler,
            typename NcSelector>
  inline void
  generate_anytime_sbarrtstar_no_init_impl(
    Graph &g, Vertex start_vertex, const Topology& super_space, SBARRTStarVisitor vis,
    NodeConnector connect_vertex, AStarHeuristicMap hval, PositionMap position, WeightMap weight,
    DensityMap density, ConstrictionMap constriction, DistanceMap distance,
    PredecessorMap predecessor, KeyMap key,
    RandomSampler get_sample,
    NcSelector select_neighborhood,
    double init_relaxation, double SA_init_temperature = 1.0)
  {
    typedef typename boost::property_traits<KeyMap>::value_type KeyValue;
    typedef std::less<double> KeyCompareType;  // <---- this is a min-heap.
    typedef boost::vector_property_map<std::size_t> IndexInHeapMap;
    IndexInHeapMap index_in_heap;
    {
      typename boost::graph_traits<Graph>::vertex_iterator ui, ui_end;
      for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
        put(index_in_heap,*ui, static_cast<std::size_t>(-1)); 
      };
    };
    
    typedef boost::d_ary_heap_indirect<Vertex, 4, IndexInHeapMap, KeyMap, KeyCompareType> MutableQueue;
    MutableQueue Q(key, index_in_heap, KeyCompareType()); //priority queue holding the OPEN set.
    
    anytime_sbarrtstar_bfs_visitor<
      SBARRTStarVisitor, MutableQueue, IndexInHeapMap,
      AStarHeuristicMap, PositionMap, WeightMap, 
      DensityMap, ConstrictionMap, 
      DistanceMap, PredecessorMap, KeyMap> sba_bfs_vis(vis, Q, index_in_heap, hval, position, weight, 
                                                        density, constriction, distance, predecessor, key, init_relaxation);
    
    put(distance, g[start_vertex], 0.0);
    
    sbarrtstar_search_loop(g, start_vertex, super_space, sba_bfs_vis, connect_vertex, sba_node_generator(), 
                            rrg_node_generator<Topology, RandomSampler, NcSelector>(&super_space, get_sample, select_neighborhood), 
                            Q, select_neighborhood, SA_init_temperature);
    
  };
  
  
  
}; //end of detail namespace.



/**
  * This function template generates a roadmap to connect a goal location to a start location
  * using the Anytime-SBA* algorithm, without initialization of the existing graph.
  * \tparam SBAStarBundle A SBA* bundle type (see make_sbastar_bundle()).
  * \param bdl A const-reference to a SBA* bundle of parameters, see make_sbastar_bundle().
  * \param init_relaxation The initial relaxation factor to use when computing the ASBA* key values.
  *        Should be greater than 0, the recommeded value is 10.
  */
template <typename SBAStarBundle>
inline void generate_anytime_sbastar_no_init(const SBAStarBundle& bdl, double init_relaxation) {
  BOOST_CONCEPT_ASSERT((ASBAStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  
  detail::generate_anytime_sbastar_no_init_impl(
    *(bdl.m_g), bdl.m_start_vertex, *(bdl.m_super_space), bdl.m_vis, motion_graph_connector(), 
    bdl.m_hval, bdl.m_position, bdl.m_weight, bdl.m_density, bdl.m_constriction, 
    bdl.m_distance, bdl.m_predecessor, bdl.m_key, 
    bdl.m_select_neighborhood, init_relaxation);
};


/**
  * This function template generates a roadmap to connect a goal location to a start location
  * using the Anytime-SBA* algorithm, with initialization of the existing graph to (re)start the search.
  * \tparam SBAStarBundle A SBA* bundle type (see make_sbastar_bundle()).
  * \param bdl A const-reference to a SBA* bundle of parameters, see make_sbastar_bundle().
  * \param init_relaxation The initial temperature of the Simulated Annealing when used 
  *        as the deciding factor between using RRT* or SBA* samples.
  */
template <typename SBAStarBundle>
inline void generate_anytime_sbastar(const SBAStarBundle& bdl, double init_relaxation) {
  BOOST_CONCEPT_ASSERT((ASBAStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  
  detail::initialize_sbastar_nodes(*(bdl.m_g), bdl.m_vis, bdl.m_distance, bdl.m_predecessor, bdl.m_key);
  
  generate_anytime_sbastar_no_init(bdl, init_relaxation);
  
};



/**
  * This function template generates a roadmap to connect a goal location to a start location
  * using the Anytime-Lazy-SBA* algorithm, without initialization of the existing graph.
  * \tparam SBAStarBundle A SBA* bundle type (see make_sbastar_bundle()).
  * \param bdl A const-reference to a SBA* bundle of parameters, see make_sbastar_bundle().
  * \param init_relaxation The initial relaxation factor to use when computing the ASBA* key values.
  *        Should be greater than 0, the recommeded value is 10.
  */
template <typename SBAStarBundle>
inline void generate_anytime_lazy_sbastar_no_init(const SBAStarBundle& bdl, double init_relaxation) {
  BOOST_CONCEPT_ASSERT((ASBAStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  
  detail::generate_anytime_sbastar_no_init_impl(
    *(bdl.m_g), bdl.m_start_vertex, *(bdl.m_super_space), bdl.m_vis, lazy_node_connector(), 
    bdl.m_hval, bdl.m_position, bdl.m_weight, bdl.m_density, bdl.m_constriction, 
    bdl.m_distance, bdl.m_predecessor, bdl.m_key, 
    bdl.m_select_neighborhood, init_relaxation);
};

/**
  * This function template generates a roadmap to connect a goal location to a start location
  * using the Anytime-Lazy-SBA* algorithm, with initialization of the existing graph to (re)start the search.
  * \tparam SBAStarBundle A SBA* bundle type (see make_sbastar_bundle()).
  * \param bdl A const-reference to a SBA* bundle of parameters, see make_sbastar_bundle().
  * \param init_relaxation The initial temperature of the Simulated Annealing when used 
  *        as the deciding factor between using RRT* or SBA* samples.
  */
template <typename SBAStarBundle>
inline void generate_anytime_lazy_sbastar(const SBAStarBundle& bdl, double init_relaxation) {
  BOOST_CONCEPT_ASSERT((ASBAStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  
  detail::initialize_sbastar_nodes(*(bdl.m_g), bdl.m_vis, bdl.m_distance, bdl.m_predecessor, bdl.m_key);
  
  generate_anytime_lazy_sbastar_no_init(bdl, init_relaxation);
  
};


/**
  * This function template generates a roadmap to connect a goal location to a start location
  * using the Anytime-Lazy-SBA* algorithm, without initialization of the existing graph.
  * \tparam SBAStarBundle A SBA* bundle type (see make_sbastar_bundle()).
  * \param bdl A const-reference to a SBA* bundle of parameters, see make_sbastar_bundle().
  * \param init_relaxation The initial relaxation factor to use when computing the ASBA* key values.
  *        Should be greater than 0, the recommeded value is 10.
  */
template <typename SBAStarBundle>
inline void generate_anytime_lazy_bnb_sbastar_no_init(const SBAStarBundle& bdl, typename SBAStarBundle::vertex_type goal_vertex, double init_relaxation) {
  BOOST_CONCEPT_ASSERT((ASBAStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  
  detail::generate_anytime_sbastar_no_init_impl(
    *(bdl.m_g), bdl.m_start_vertex, *(bdl.m_super_space), bdl.m_vis, 
    branch_and_bound_connector<typename SBAStarBundle::graph_type>(
      *(bdl.m_g),
      bdl.m_start_vertex, 
      goal_vertex
    ), 
    bdl.m_hval, bdl.m_position, bdl.m_weight, bdl.m_density, bdl.m_constriction, 
    bdl.m_distance, bdl.m_predecessor, bdl.m_key, 
    bdl.m_select_neighborhood, init_relaxation);
};

/**
  * This function template generates a roadmap to connect a goal location to a start location
  * using the Anytime-Lazy-SBA* algorithm, with initialization of the existing graph to (re)start the search.
  * \tparam SBAStarBundle A SBA* bundle type (see make_sbastar_bundle()).
  * \param bdl A const-reference to a SBA* bundle of parameters, see make_sbastar_bundle().
  * \param init_relaxation The initial temperature of the Simulated Annealing when used 
  *        as the deciding factor between using RRT* or SBA* samples.
  */
template <typename SBAStarBundle>
inline void generate_anytime_lazy_bnb_sbastar(const SBAStarBundle& bdl, typename SBAStarBundle::vertex_type goal_vertex, double init_relaxation) {
  BOOST_CONCEPT_ASSERT((ASBAStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  
  detail::initialize_sbastar_nodes(*(bdl.m_g), bdl.m_vis, bdl.m_distance, bdl.m_predecessor, bdl.m_key);
  
  generate_anytime_lazy_bnb_sbastar_no_init(bdl, goal_vertex, init_relaxation);
  
};






/**
  * This function template generates a roadmap to connect a goal location to a start location
  * using the Anytime-SBA*-RRT* algorithm, without initialization of the existing graph.
  * \tparam SBAStarBundle A SBA* bundle type (see make_sbastar_bundle()).
  * \tparam RandomSampler This is a random-sampler over the topology (see pp::RandomSamplerConcept).
  * \param bdl A const-reference to a SBA* bundle of parameters, see make_sbastar_bundle().
  * \param get_sample A random sampler of positions in the space.
  * \param init_relaxation The initial relaxation factor to use when computing the ASBA* key values.
  *        Should be greater than 0, the recommeded value is 10.
  * \param SA_init_temperature The initial temperature of the Simulated Annealing when used 
  *        as the deciding factor between using RRT* or SBA* samples.
  */
template <typename SBAStarBundle, 
          typename RandomSampler>
inline void generate_anytime_sbarrtstar_no_init(const SBAStarBundle& bdl, 
                                                RandomSampler get_sample, 
                                                double init_relaxation,
                                                double SA_init_temperature = 1.0) {
  BOOST_CONCEPT_ASSERT((SBARRTStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  BOOST_CONCEPT_ASSERT((ASBAStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  
  detail::generate_anytime_sbarrtstar_no_init_impl(
    *(bdl.m_g), bdl.m_start_vertex, *(bdl.m_super_space), bdl.m_vis, motion_graph_connector(), 
    bdl.m_hval, bdl.m_position, bdl.m_weight, bdl.m_density, bdl.m_constriction, 
    bdl.m_distance, bdl.m_predecessor, bdl.m_key, get_sample, 
    bdl.m_select_neighborhood, init_relaxation, SA_init_temperature);
};

/**
  * This function template generates a roadmap to connect a goal location to a start location
  * using the Anytime-SBA*-RRT* algorithm, with initialization of the existing graph to (re)start the search.
  * \tparam SBAStarBundle A SBA* bundle type (see make_sbastar_bundle()).
  * \param bdl A const-reference to a SBA* bundle of parameters, see make_sbastar_bundle().
  * \param get_sample A random sampler of positions in the space.
  * \param init_relaxation The initial temperature of the Simulated Annealing when used 
  *        as the deciding factor between using RRT* or SBA* samples.
  * \param SA_init_temperature The initial temperature of the Simulated Annealing when used 
  *        as the deciding factor between using RRT* or SBA* samples.
  */
template <typename SBAStarBundle,
          typename RandomSampler>
inline void generate_anytime_sbarrtstar(const SBAStarBundle& bdl, 
                                        RandomSampler get_sample, 
                                        double init_relaxation,
                                        double SA_init_temperature = 1.0) {
  BOOST_CONCEPT_ASSERT((SBARRTStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  BOOST_CONCEPT_ASSERT((ASBAStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  
  detail::initialize_sbastar_nodes(*(bdl.m_g), bdl.m_vis, bdl.m_distance, bdl.m_predecessor, bdl.m_key);
  
  generate_anytime_sbarrtstar_no_init(bdl, get_sample, init_relaxation, SA_init_temperature);
  
};


/**
  * This function template generates a roadmap to connect a goal location to a start location
  * using the Anytime-SBA*-RRT* algorithm, without initialization of the existing graph.
  * \tparam SBAStarBundle A SBA* bundle type (see make_sbastar_bundle()).
  * \tparam RandomSampler This is a random-sampler over the topology (see pp::RandomSamplerConcept).
  * \param bdl A const-reference to a SBA* bundle of parameters, see make_sbastar_bundle().
  * \param get_sample A random sampler of positions in the space.
  * \param init_relaxation The initial relaxation factor to use when computing the ASBA* key values.
  *        Should be greater than 0, the recommeded value is 10.
  * \param SA_init_temperature The initial temperature of the Simulated Annealing when used 
  *        as the deciding factor between using RRT* or SBA* samples.
  */
template <typename SBAStarBundle, 
          typename RandomSampler>
inline void generate_anytime_lazy_sbarrtstar_no_init(const SBAStarBundle& bdl, 
                                                      RandomSampler get_sample, 
                                                      double init_relaxation,
                                                      double SA_init_temperature = 1.0) {
  BOOST_CONCEPT_ASSERT((SBARRTStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  BOOST_CONCEPT_ASSERT((ASBAStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  
  detail::generate_anytime_sbarrtstar_no_init_impl(
    *(bdl.m_g), bdl.m_start_vertex, *(bdl.m_super_space), bdl.m_vis, lazy_node_connector(), 
    bdl.m_hval, bdl.m_position, bdl.m_weight, bdl.m_density, bdl.m_constriction, 
    bdl.m_distance, bdl.m_predecessor, bdl.m_key, get_sample, 
    bdl.m_select_neighborhood, init_relaxation, SA_init_temperature);
};

/**
  * This function template generates a roadmap to connect a goal location to a start location
  * using the Anytime-Lazy-SBA*-RRT* algorithm, with initialization of the existing graph to (re)start the search.
  * \tparam SBAStarBundle A SBA* bundle type (see make_sbastar_bundle()).
  * \param bdl A const-reference to a SBA* bundle of parameters, see make_sbastar_bundle().
  * \param get_sample A random sampler of positions in the space.
  * \param init_relaxation The initial temperature of the Simulated Annealing when used 
  *        as the deciding factor between using RRT* or SBA* samples.
  * \param SA_init_temperature The initial temperature of the Simulated Annealing when used 
  *        as the deciding factor between using RRT* or SBA* samples.
  */
template <typename SBAStarBundle,
          typename RandomSampler>
inline void generate_anytime_lazy_sbarrtstar(const SBAStarBundle& bdl, 
                                              RandomSampler get_sample, 
                                              double init_relaxation,
                                              double SA_init_temperature = 1.0) {
  BOOST_CONCEPT_ASSERT((SBARRTStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  BOOST_CONCEPT_ASSERT((ASBAStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  
  detail::initialize_sbastar_nodes(*(bdl.m_g), bdl.m_vis, bdl.m_distance, bdl.m_predecessor, bdl.m_key);
  
  generate_anytime_lazy_sbarrtstar_no_init(bdl, get_sample, init_relaxation, SA_init_temperature);
  
};


/**
  * This function template generates a roadmap to connect a goal location to a start location
  * using the Anytime-SBA*-RRT* algorithm, without initialization of the existing graph.
  * \tparam SBAStarBundle A SBA* bundle type (see make_sbastar_bundle()).
  * \tparam RandomSampler This is a random-sampler over the topology (see pp::RandomSamplerConcept).
  * \param bdl A const-reference to a SBA* bundle of parameters, see make_sbastar_bundle().
  * \param get_sample A random sampler of positions in the space.
  * \param init_relaxation The initial relaxation factor to use when computing the ASBA* key values.
  *        Should be greater than 0, the recommeded value is 10.
  * \param SA_init_temperature The initial temperature of the Simulated Annealing when used 
  *        as the deciding factor between using RRT* or SBA* samples.
  */
template <typename SBAStarBundle, 
          typename RandomSampler>
inline void generate_anytime_lazy_bnb_sbarrtstar_no_init(const SBAStarBundle& bdl, 
                                                          typename SBAStarBundle::vertex_type goal_vertex,
                                                          RandomSampler get_sample, 
                                                          double init_relaxation,
                                                          double SA_init_temperature = 1.0) {
  BOOST_CONCEPT_ASSERT((SBARRTStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  BOOST_CONCEPT_ASSERT((ASBAStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  
  detail::generate_anytime_sbarrtstar_no_init_impl(
    *(bdl.m_g), bdl.m_start_vertex, *(bdl.m_super_space), bdl.m_vis, 
    branch_and_bound_connector<typename SBAStarBundle::graph_type>(
      *(bdl.m_g),
      bdl.m_start_vertex, 
      goal_vertex
    ), 
    bdl.m_hval, bdl.m_position, bdl.m_weight, bdl.m_density, bdl.m_constriction, 
    bdl.m_distance, bdl.m_predecessor, bdl.m_key, get_sample, 
    bdl.m_select_neighborhood, init_relaxation, SA_init_temperature);
};

/**
  * This function template generates a roadmap to connect a goal location to a start location
  * using the Anytime-Lazy-SBA*-RRT* algorithm, with initialization of the existing graph to (re)start the search.
  * \tparam SBAStarBundle A SBA* bundle type (see make_sbastar_bundle()).
  * \param bdl A const-reference to a SBA* bundle of parameters, see make_sbastar_bundle().
  * \param get_sample A random sampler of positions in the space.
  * \param init_relaxation The initial temperature of the Simulated Annealing when used 
  *        as the deciding factor between using RRT* or SBA* samples.
  * \param SA_init_temperature The initial temperature of the Simulated Annealing when used 
  *        as the deciding factor between using RRT* or SBA* samples.
  */
template <typename SBAStarBundle,
          typename RandomSampler>
inline void generate_anytime_lazy_bnb_sbarrtstar(const SBAStarBundle& bdl, 
                                                  typename SBAStarBundle::vertex_type goal_vertex,
                                                  RandomSampler get_sample, 
                                                  double init_relaxation,
                                                  double SA_init_temperature = 1.0) {
  BOOST_CONCEPT_ASSERT((SBARRTStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  BOOST_CONCEPT_ASSERT((ASBAStarVisitorConcept<typename SBAStarBundle::visitor_type,typename SBAStarBundle::graph_type,typename SBAStarBundle::topology_type>));
  
  detail::initialize_sbastar_nodes(*(bdl.m_g), bdl.m_vis, bdl.m_distance, bdl.m_predecessor, bdl.m_key);
  
  generate_anytime_lazy_bnb_sbarrtstar_no_init(bdl, goal_vertex, get_sample, init_relaxation, SA_init_temperature);
  
};






};

};

#endif
















