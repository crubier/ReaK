/**
 * \file manip_SSRMS_arm.hpp
 * 
 * This library declares a class to represent a kte-based model of a SSRMS manipulator in 3D, i.e., 
 * the Canadarm-2 manipulator.
 * 
 * \author Mikael Persson, <mikael.s.persson@gmail.com>
 * \date November 2012
 */

/*
 *    Copyright 2012 Sven Mikael Persson
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

#ifndef REAK_MANIP_SSRMS_ARM_HPP
#define REAK_MANIP_SSRMS_ARM_HPP

#include "base/defs.hpp"
#include "inverse_kinematics_model.hpp"
#include "mbd_kte/kte_map_chain.hpp"

namespace ReaK {

namespace kte {



/**
 * This class to represent a kte-based model of a SSRMS manipulator in 3D, i.e., 
 * the Canadarm-2 manipulator.
 */
class manip_SSRMS_kinematics : public inverse_kinematics_model {
  private:
    shared_ptr< frame_3D<double> > m_base_frame;
    std::vector< shared_ptr< gen_coord<double> > > m_joints;
    shared_ptr< joint_dependent_frame_3D > m_EE;
    vect_n<double> link_lengths;
    vect_n<double> joint_offsets;
    shared_ptr< kte_map_chain > m_chain;
    
  public:
    
    vect_n<double> joint_lower_bounds;
    vect_n<double> joint_upper_bounds;
    
    shared_ptr< kte_map_chain > getKTEChain() const { return m_chain; };
    
    /**
     * Default constructor.
     */
    manip_SSRMS_kinematics(const std::string& aName = "",
                           const shared_ptr< frame_3D<double> >& aBaseFrame = shared_ptr< frame_3D<double> >(),
                           const vect_n<double>& aLinkLengths = vect_n<double>(0.0, 0.380, 6.850, 6.850, 0.380, 0.0), 
                           const vect_n<double>& aJointOffsets = vect_n<double>(0.0, 0.635, 0.504, 0.504, 0.504, 0.635, 0.0),
                           const vect_n<double>& aJointLowerBounds = vect_n<double>(-1.5 * M_PI, -1.5 * M_PI, -1.5 * M_PI, -1.5 * M_PI, -1.5 * M_PI, -1.5 * M_PI, -1.5 * M_PI),
                           const vect_n<double>& aJointUpperBounds = vect_n<double>( 1.5 * M_PI,  1.5 * M_PI,  1.5 * M_PI,  1.5 * M_PI,  1.5 * M_PI,  1.5 * M_PI,  1.5 * M_PI));
    
    virtual ~manip_SSRMS_kinematics() { };
    
    virtual std::size_t getJointPositionsCount() const { return 7; };
    
    virtual std::size_t getJointVelocitiesCount() const { return 7; };
    
    virtual std::size_t getJointAccelerationsCount() const { return 7; };
    
    virtual std::size_t getDependentPositionsCount() const { return 7; };
    
    virtual std::size_t getDependentVelocitiesCount() const { return 6; };
    
    virtual std::size_t getDependentAccelerationsCount() const { return 6; };
    
    virtual std::size_t getCoordsCount() const { return 7; };
    
    virtual shared_ptr< gen_coord<double> > getCoord(std::size_t i) const { 
      return m_joints[i];
    };
    
    virtual std::size_t getDependentFrames3DCount() const { return 1; };
    
    virtual shared_ptr< joint_dependent_frame_3D > getDependentFrame3D(std::size_t i) const { 
      return m_EE;
    };
    
    virtual void doDirectMotion();
    
    virtual void doInverseMotion();
    
    virtual void getJacobianMatrix(mat<double,mat_structure::rectangular>& Jac) const;
    
    virtual void getJacobianMatrixAndDerivative(mat<double,mat_structure::rectangular>& Jac, mat<double,mat_structure::rectangular>& JacDot) const;
    
    virtual vect_n<double> getJointPositions() const;
    
    virtual void setJointPositions(const vect_n<double>& aJointPositions);
    
    virtual vect_n<double> getJointVelocities() const;
    
    virtual void setJointVelocities(const vect_n<double>& aJointVelocities);
    
    virtual vect_n<double> getJointAccelerations() const;
    
    virtual void setJointAccelerations(const vect_n<double>& aJointAccelerations);
    
    virtual vect_n<double> getDependentPositions() const;
    
    virtual vect_n<double> getDependentVelocities() const;
    
    virtual vect_n<double> getDependentAccelerations() const;
    
    virtual void setDependentPositions(const vect_n<double>& aDepPositions);
    
    virtual void setDependentVelocities(const vect_n<double>& aDepVelocities);
    
    virtual void setDependentAccelerations(const vect_n<double>& aDepAccelerations);
    
    virtual void RK_CALL save(serialization::oarchive& A, unsigned int) const;
    
    virtual void RK_CALL load(serialization::iarchive& A, unsigned int);
    
    RK_RTTI_MAKE_CONCRETE_1BASE(manip_SSRMS_kinematics,0xC2100056,1,"manip_SSRMS_kinematics",inverse_kinematics_model)
    
};



};

};

#endif











