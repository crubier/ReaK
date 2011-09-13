
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

#include "kte_map_chain.hpp"

#include "inertia.hpp"
#include "revolute_joint.hpp"
#include "rigid_link.hpp"

#include "force_actuator.hpp"
#include "joint_backlash.hpp"
#include "joint_friction.hpp"

#include "lin_alg/mat_num.hpp"

#include "recorders/ssv_recorder.hpp"

#include "serialization/xml_archiver.hpp"


using namespace ReaK;

using namespace ReaK::kte;

using namespace serialization;

int main() {

#if 1
  shared_pointer<frame_2D<double> >::type base_frame = rtti::rk_dynamic_ptr_cast< frame_2D<double> >(frame_2D<double>::Create());
  shared_pointer<frame_2D<double> >::type joint_frame = rtti::rk_dynamic_ptr_cast< frame_2D<double> >(frame_2D<double>::Create());
  shared_pointer<frame_2D<double> >::type end_frame = rtti::rk_dynamic_ptr_cast< frame_2D<double> >(frame_2D<double>::Create());
  shared_pointer<gen_coord<double> >::type joint_coord = rtti::rk_dynamic_ptr_cast< gen_coord<double> >(gen_coord<double>::Create());
  
  base_frame->Acceleration = vect<double,2>(0,9.81); //add gravity

  //create motor inertia
  shared_pointer<inertia_gen>::type motor_inertia(new inertia_gen("motor_inertia",
								  shared_pointer< joint_dependent_gen_coord >::type(new joint_dependent_gen_coord(joint_coord)),5),scoped_deleter());
  //create friction
  shared_pointer<joint_dry_microslip_gen>::type friction(new joint_dry_microslip_gen("friction",joint_coord,1E-6,2E-6,1,0.9),scoped_deleter());
  //create revolute joint
  shared_pointer<revolute_joint_2D>::type rev_joint(new revolute_joint_2D("joint1",joint_coord,base_frame,joint_frame),scoped_deleter());
  //create actuator
  shared_pointer<force_actuator_gen>::type actuator(new force_actuator_gen("actuator",joint_coord,rev_joint),scoped_deleter());
  //create link of lenght 0.5 meters
  shared_pointer<rigid_link_2D>::type link1(new rigid_link_2D("link1",joint_frame,end_frame,pose_2D<double>(weak_pointer<pose_2D<double> >::type(),vect<double,2>(0.5,0.0),rot_mat_2D<double>(0.0))),scoped_deleter());
  //create end mass of 1.0 kg (point mass only)
  shared_pointer<inertia_2D>::type mass1(new inertia_2D("mass1",
							shared_pointer< joint_dependent_frame_2D >::type(new joint_dependent_frame_2D(end_frame)),
							1.0,0.0),scoped_deleter());

  kte_map_chain adv_pendulum("adv_pendulum");

  adv_pendulum << motor_inertia << friction << rev_joint << link1 << mass1;

  {
    xml_oarchive adv_pendulum_arc("adv_pendulum.xml");
    oarchive& arc_ref = adv_pendulum_arc;
    arc_ref << joint_coord << adv_pendulum << end_frame;

  };
  
  kte_map_chain adv_motorized_pendulum("adv_motorized_pendulum");
  
  adv_motorized_pendulum << actuator << motor_inertia << friction << rev_joint << link1 << mass1;
  
  {
    xml_oarchive adv_motorized_pendulum_arc("adv_motorized_pendulum.xml");
    oarchive& arc_ref = adv_motorized_pendulum_arc;
    arc_ref << joint_coord << adv_pendulum << end_frame;
  };

#else
  shared_pointer< gen_coord<double> >::type joint_coord;
  shared_pointer<frame_2D<double> >::type end_frame;

#if 1
  kte_map_chain adv_pendulum;

  {
    xml_iarchive adv_pendulum_arc("adv_pendulum.xml");
    iarchive& arc_ref = adv_pendulum_arc;
    arc_ref >> joint_coord >> adv_pendulum >> end_frame;
  };
#else
  kte_map_chain adv_motorized_pendulum;
  
  {
    xml_iarchive adv_motorized_pendulum_arc("adv_motorized_pendulum.xml");
    iarchive& arc_ref = adv_motorized_pendulum_arc;
    arc_ref >> joint_coord >> adv_motorized_pendulum >> end_frame;
  };
#endif

  std::cout << "Pendulum model loaded.. staring simulation!" << std::endl;

#endif

  recorder::ssv_recorder output_rec("adv_pendulum_results.ssvdat");
  output_rec << "time" 
             << "q" 
             << "qd" 
             << "qdd" 
             << "f" 
             << recorder::data_recorder::end_name_row;

  double sim_time = 0.0;
  double last_sim_time = -0.01;
  
  for (;sim_time < 20;sim_time += 0.00001) {
    
    joint_coord->q_ddot = 0.0;
    adv_pendulum.doMotion();
    adv_pendulum.clearForce();
    adv_pendulum.doForce();
    double f_nl = joint_coord->f;

    joint_coord->q_ddot = 1.0;
    adv_pendulum.doMotion();
    adv_pendulum.clearForce();
    adv_pendulum.doForce();
    double f_nl_1 = joint_coord->f;

    joint_coord->q_ddot = f_nl / (f_nl - f_nl_1);

    if(sim_time >= last_sim_time + 0.01) {
      last_sim_time = sim_time;
      std::cout << "\r" << sim_time;
      std::cout.flush();
      output_rec << sim_time
                 << joint_coord->q
  	         << joint_coord->q_dot
	         << joint_coord->q_ddot
	         << f_nl
	         << recorder::data_recorder::end_value_row;
    };
    
    joint_coord->q += joint_coord->q_dot * 0.00001;
    joint_coord->q_dot += joint_coord->q_ddot * 0.00001;

  };

  output_rec << recorder::data_recorder::close;

};






