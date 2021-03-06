/**
 * \file test_CRS_IK.cpp
 *
 * This application tests the inverse kinematics for the CRS A465 manipulator models.
 *
 * \author Mikael Persson, <mikael.s.persson@gmail.com>
 * \date March 2012
 */


#include "CRS_A465_models.hpp"
#include "CRS_A465_2D_analog.hpp"

#include "topologies/direct_kinematics_topomap.hpp"
#include "topologies/inverse_kinematics_topomap.hpp"

#include "recorders/tsv_recorder.hpp"

#include <iomanip>

int main() {
  
  ReaK::robot_airship::CRS_A465_model_builder builder;
  
  builder.load_kte_from_file("models/CRS_A465_raw_components.xml");
  builder.load_limits_from_file("models/CRS_A465_limits.xml");
  
  typedef ReaK::robot_airship::CRS_A465_model_builder::joint_space_type JointSpaceType;
  typedef ReaK::robot_airship::CRS_A465_model_builder::end_effector_space_type EESpaceType;
  
  JointSpaceType j_space = builder.get_joint_space();
  EESpaceType ee_space = builder.get_end_effector_space();
  
  ReaK::shared_ptr< ReaK::kte::manipulator_kinematics_model > model = builder.get_manipulator_kin_model();
  
  ReaK::shared_ptr< ReaK::optim::cost_evaluator > ik_cost_func = 
    ReaK::kte::create_clik_mixed_cost(
      ReaK::kte::create_clik_quad_cost(
        builder.preferred_posture, 
        builder.joint_lower_bounds,
        builder.joint_upper_bounds,
        *model),
      ReaK::kte::create_clik_bent_joints(3,5)
    );
  
  ReaK::pp::manip_clik_kin_map ik_map(
    model, ik_cost_func,
    10.0, // aRadius
    0.1,  // aMu
    200,  // aMaxIter
    1e-4, // aTol
    5e-2, // aEta
    0.95  // aTau
  );
  
  /*
  ReaK::pp::manip_ik_knn_starts_map<
    ReaK::pp::manip_clik_calc_factory<
      JointSpaceType,
      ReaK::pp::clik_mixed_cost_factory<
        ReaK::pp::clik_bent_joints_cost_factory
      >
    >,
    JointSpaceType,
    EESpaceType
  > ik_knn_map(
           model,
	   ReaK::pp::manip_clik_calc_factory<
             JointSpaceType,
             ReaK::pp::clik_mixed_cost_factory<
               ReaK::pp::clik_bent_joints_cost_factory
             >
           >( ReaK::shared_ptr< JointSpaceType >(&j_space, ReaK::null_deleter()),
	      ReaK::pp::clik_mixed_cost_factory<
                ReaK::pp::clik_bent_joints_cost_factory
              >(ReaK::pp::clik_bent_joints_cost_factory(3,5)),
	      10.0, // aRadius
              0.1, // aMu
              200, // aMaxIter
              1e-4, // aTol
              5e-2, // aEta
              0.95  // aTau
	    ),
	   ReaK::shared_ptr< JointSpaceType >(&j_space, ReaK::null_deleter()),
	   ReaK::shared_ptr< EESpaceType >(&ee_space, ReaK::null_deleter()),
	   1000, 10
	 );*/
  ReaK::pp::manip_direct_kin_map dk_map(model);
  
  typedef ReaK::pp::topology_traits< EESpaceType >::point_type EEPointType;
  typedef ReaK::pp::topology_traits< JointSpaceType >::point_type JointPointType;
  
  ReaK::frame_3D<double> ee_f = ReaK::frame_3D<double>(
    ReaK::weak_ptr< ReaK::pose_3D<double> >(),
    ReaK::vect<double,3>(-0.6, -0.6, 0.0),
    ReaK::axis_angle<double>(0.0,ReaK::vect<double,3>(1.0, 0.0, 0.0)).getQuaternion(),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0));
  EEPointType ee_x;
  
  JointPointType j_x;
  JointPointType j_zero;
  using ReaK::get;
  get<0>(j_zero) = ReaK::vect<double,7>(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  get<1>(j_zero) = ReaK::vect<double,7>(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  
#if 0
  ReaK::frame_3D<double> ee_fs[10];
  
  ee_fs[0] = ReaK::frame_3D<double>(
    ReaK::weak_ptr< ReaK::pose_3D<double> >(),
    ReaK::vect<double,3>(2.0, 0.5, 0.5),
    ReaK::axis_angle<double>(0.5 * M_PI,ReaK::vect<double,3>(0.0, 0.0, 1.0)).getQuaternion(),
    ReaK::vect<double,3>(0.1, 0.1, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.1),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0));
    
  ee_fs[1] = ReaK::frame_3D<double>(
    ReaK::weak_ptr< ReaK::pose_3D<double> >(),
    ReaK::vect<double,3>(0.5053, 0.2, 0.6667),
    ReaK::axis_angle<double>(0.0,ReaK::vect<double,3>(1.0, 0.0, 0.0)).getQuaternion(),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0));
    
  ee_fs[2] = ReaK::frame_3D<double>(
    ReaK::weak_ptr< ReaK::pose_3D<double> >(),
    ReaK::vect<double,3>(1.8316, -0.3333, 0.5333),
    ReaK::axis_angle<double>(0.0,ReaK::vect<double,3>(1.0, 0.0, 0.0)).getQuaternion(),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0));
    
  ee_fs[3] = ReaK::frame_3D<double>(
    ReaK::weak_ptr< ReaK::pose_3D<double> >(),
    ReaK::vect<double,3>(1.8316, 0.3333, 0.5333),
    ReaK::axis_angle<double>(0.0,ReaK::vect<double,3>(1.0, 0.0, 0.0)).getQuaternion(),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0));
    
  ee_fs[4] = ReaK::frame_3D<double>(
    ReaK::weak_ptr< ReaK::pose_3D<double> >(),
    ReaK::vect<double,3>(1.8316, 0.06667, 0.4),
    ReaK::axis_angle<double>(0.0,ReaK::vect<double,3>(1.0, 0.0, 0.0)).getQuaternion(),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0));
    
  ee_fs[5] = ReaK::frame_3D<double>(
    ReaK::weak_ptr< ReaK::pose_3D<double> >(),
    ReaK::vect<double,3>(2.7158, 0.2, 0.4),
    ReaK::axis_angle<double>(0.0,ReaK::vect<double,3>(1.0, 0.0, 0.0)).getQuaternion(),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0));
    
  ee_fs[6] = ReaK::frame_3D<double>(
    ReaK::weak_ptr< ReaK::pose_3D<double> >(),
    ReaK::vect<double,3>(2.7158, -0.2, 0.4),
    ReaK::axis_angle<double>(0.0,ReaK::vect<double,3>(1.0, 0.0, 0.0)).getQuaternion(),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0));
    
  ee_fs[7] = ReaK::frame_3D<double>(
    ReaK::weak_ptr< ReaK::pose_3D<double> >(),
    ReaK::vect<double,3>(2.667, -0.0667, 0.5684),
    ReaK::axis_angle<double>(0.0,ReaK::vect<double,3>(1.0, 0.0, 0.0)).getQuaternion(),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0));
    
  ee_fs[8] = ReaK::frame_3D<double>(
    ReaK::weak_ptr< ReaK::pose_3D<double> >(),
    ReaK::vect<double,3>(0.8, -0.0667, 0.3789),
    ReaK::axis_angle<double>(0.0,ReaK::vect<double,3>(1.0, 0.0, 0.0)).getQuaternion(),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0));
    
  ee_fs[9] = ReaK::frame_3D<double>(
    ReaK::weak_ptr< ReaK::pose_3D<double> >(),
    ReaK::vect<double,3>(3.133, -0.0667, 0.6316),
    ReaK::axis_angle<double>(0.0,ReaK::vect<double,3>(1.0, 0.0, 0.0)).getQuaternion(),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0),
    ReaK::vect<double,3>(0.0, 0.0, 0.0));
  
  for(std::size_t i = 0; i < 10; ++i) {
  
    std::cout << "\n\n Testing with EE-frame: " << std::endl;
    std::cout << ee_fs[i].Position << std::endl
              << ee_fs[i].Quat << std::endl
              << ee_fs[i].Velocity << std::endl
              << ee_fs[i].AngVelocity << std::endl;
    for(std::size_t j = 0; j < 10; ++j) {
      try {
        ee_x = dk_map.map_to_space(j_space.random_point(), j_space, ee_space);
        set_frame_3D(ee_x, ee_fs[i]);
        j_x = ik_map.map_to_space(ee_x, ee_space, j_space);
        ReaK::frame_3D<double> f_x = get_frame_3D(dk_map.map_to_space(j_x, j_space, ee_space));
        ReaK::frame_3D<double> ee_err = f_x.getFrameRelativeTo( ReaK::shared_ptr< const ReaK::frame_3D<double> >(&ee_fs[i], ReaK::null_deleter()));
        if( ( norm_2( ee_err.Position ) < 0.01 ) &&
	    ( ReaK::axis_angle<double>(ee_err.Quat).angle() < 0.02 ) ) {
          std::cout << f_x.Position << std::endl
                    << f_x.Quat << std::endl
                    << f_x.Velocity << std::endl
                    << f_x.AngVelocity << std::endl;
	  break;
        };
      } catch(ReaK::singularity_error& e) {
        std::cout << "ERROR: Singularity Detected!" << std::endl;
      } catch(ReaK::maximum_iteration& e) {
        std::cout << "ERROR: Maximum Iterations reached!" << std::endl;
      };
    };
    
  };
  
#else
  
  ReaK::recorder::tsv_recorder rec("models/CRS_A465_workspace.tsv");
  rec << "x" << "y" << "z" << "yaw" << "pitch" << "value" << ReaK::recorder::data_recorder::end_name_row;
  
  for(std::size_t i = 0; i < 20; ++i) {
    ee_f.Position[1] = -0.6;
    for(std::size_t j = 0; j < 10; ++j) {
      ee_f.Position[2] = 0.0;
      for(std::size_t k = 0; k < 10; ++k) {
	for(std::size_t l = 0; l < 10; ++l) {
	  for(std::size_t m = 0; m < 10; ++m) {
	    std::cout << "\r" << std::setw(4) << i 
	                      << std::setw(4) << j 
	                      << std::setw(4) << k 
	                      << std::setw(4) << l 
	                      << std::setw(4) << m; std::cout.flush();
	    rec << ee_f.Position[0] << ee_f.Position[1] << ee_f.Position[2]
	        << (l * 2.0 * M_PI / 10.0) << (m * 2.0 * M_PI / 10.0);
	    
	    try {
	      set_frame_3D(ee_x, ee_f);
	      j_x = ik_map.map_to_space(ee_x, ee_space, j_space);
	      rec << 1.0;
	    } catch(ReaK::optim::infeasible_problem& e) { RK_UNUSED(e);
	      rec << 0.0;
	    };
	    /*
	    bool did_succeed = false;
	    for(std::size_t n = 0; n < 10; ++n) {
	      try {
	        ee_x = dk_map.map_to_space(j_space.random_point(), j_space, ee_space);
	        set_frame_3D(ee_x, ee_f);
	        j_x = ik_map.map_to_space(ee_x, ee_space, j_space);
	        ee_x = dk_map.map_to_space(j_x, j_space, ee_space);
	        ReaK::frame_3D<double> ee_fx = get_frame_3D(ee_x);
	        ReaK::frame_3D<double> ee_err = ee_fx.getFrameRelativeTo( ReaK::shared_ptr< const ReaK::frame_3D<double> >(&ee_f, ReaK::null_deleter()));
	        if( ( norm_2( ee_err.Position ) < 0.01 ) &&
		    ( ReaK::axis_angle<double>(ee_err.Quat).angle() < 0.02 ) ) {
	          did_succeed = true;
		  break;
		};
	      } catch(ReaK::singularity_error& e) { };
	    };
	    if(did_succeed)
	      rec << 1.0;
	    else
	      rec << 0.0;
	    */
	    rec << ReaK::recorder::data_recorder::end_value_row;
	    ee_f.Quat *= ReaK::axis_angle<double>(2.0 * M_PI / 10.0, ReaK::vect<double,3>(0.0,1.0,0.0)).getQuaternion();
	  };
	  ee_f.Quat *= ReaK::axis_angle<double>(2.0 * M_PI / 10.0, ReaK::vect<double,3>(0.0,0.0,1.0)).getQuaternion();
	};
	ee_f.Position[2] += 1.2 / 9.0;
      };
      ee_f.Position[1] += 1.2 / 9.0;
    };
    ee_f.Position[0] += 4.2 / 19.0;
  };
  rec << ReaK::recorder::data_recorder::flush;
  
#endif
  /*
  ReaK::robot_airship::CRS_A465_2D_model_builder builder2D;
  
  builder2D.load_kte_from_file("models/CRS_A465_2D_raw_components.xml");
  builder2D.load_limits_from_file("models/CRS_A465_2D_limits.xml");
  
  
  typedef ReaK::robot_airship::CRS_A465_2D_model_builder::joint_space_type JointSpaceType2D;
  typedef ReaK::robot_airship::CRS_A465_2D_model_builder::end_effector_space_type EESpaceType2D;
  
  JointSpaceType2D j_space2D = builder2D.get_joint_space();
  EESpaceType2D ee_space2D = builder2D.get_end_effector_space();
  
  ReaK::shared_ptr< ReaK::kte::manipulator_kinematics_model > model2D = builder2D.get_manipulator_kin_model();
  
  ReaK::pp::manip_inverse_kin_map<
    ReaK::pp::manip_clik_calc_factory<
      JointSpaceType2D,
      ReaK::pp::clik_quad_cost_factory
    >
  > ik_map_2D(model2D,
	      ReaK::pp::manip_clik_calc_factory<
                JointSpaceType2D,
                ReaK::pp::clik_quad_cost_factory
              >( ReaK::shared_ptr< JointSpaceType2D >(&j_space2D, ReaK::null_deleter()),
	         ReaK::pp::clik_quad_cost_factory(builder2D.preferred_posture)));
  ReaK::pp::manip_direct_kin_map dk_map_2D(model2D);
  
  typedef ReaK::pp::topology_traits< EESpaceType2D >::point_type EEPointType2D;
  typedef ReaK::pp::topology_traits< JointSpaceType2D >::point_type JointPointType2D;
  
  EEPointType2D ee_x_2D;
  set_frame_2D(ee_x_2D, ReaK::frame_2D<double>(ReaK::weak_ptr< ReaK::pose_2D<double> >(),
					    ReaK::vect<double,2>(2.0, 0.5),
					    ReaK::rot_mat_2D<double>(M_PI * 0.5),
					    ReaK::vect<double,2>(0.1, 0.1),
					    0.1,
					    ReaK::vect<double,2>(0.0, 0.0),
					    0.0,
					    ReaK::vect<double,2>(0.0, 0.0),
					    0.0));
  
  JointPointType2D j_x_2D;
  
  try {
    j_x_2D = ik_map_2D.map_to_space(ee_x_2D, ee_space2D, j_space2D);
    ReaK::frame_2D<double> f_x_2D = get_frame_2D(dk_map_2D.map_to_space(j_x_2D, j_space2D, ee_space2D));
    std::cout << f_x_2D.Position << std::endl
              << f_x_2D.Rotation.getAngle() << std::endl
              << f_x_2D.Velocity << std::endl
              << f_x_2D.AngVelocity << std::endl;
  } catch(std::exception& e) {
    std::cout << "ERROR: An exception occurred during 2D IK problem: " << e.what() << std::endl;
  };
  */
  
  return 0;
};







