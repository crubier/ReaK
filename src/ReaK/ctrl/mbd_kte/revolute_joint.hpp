/**
 * \file revolute_joint.hpp
 *
 * This library declares the KTE models for revolute joints in 2D and 3D space. These
 * models implement a model of a single degree-of-freedom angular joint (i.e. revolute joint),
 * allowing no displacement and only rotation about its predefined, fixed axis.
 *
 * \author Mikael Persson, <mikael.s.persson@gmail.com>
 * \date March 2010
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

#ifndef REAK_REVOLUTE_JOINT_HPP
#define REAK_REVOLUTE_JOINT_HPP

#include "base/defs.hpp"

#include "reacting_kte.hpp"
#include "kinetostatics/motion_jacobians.hpp"

namespace ReaK {

namespace kte {


/**
 * This class implements a revolute joint in 2D space. A generalized coordinate is used to represent the
 * joint's angular displacement between a base coordinate frame to an end coordinate frame.
 */
class revolute_joint_2D : public reacting_kte_gen {
  protected:
    shared_ptr< gen_coord<double> > mAngle; ///< Generalized coordinate representing the joint's angular displacement.
    shared_ptr< frame_2D<double> > mBase; ///< The coordinate frame at the base of the joint.
    shared_ptr< frame_2D<double> > mEnd; ///< The coordinate frame just after the joint transformations are applied.

    shared_ptr< jacobian_gen_2D<double> > mJacobian; ///< The Jacobian frame produced by this joint.

  public:
    
    /**
     * Sets the joint's angular coordinate.
     * \param aPtr The new joint's angular coordinate.
     */
    void setAngle(const shared_ptr< gen_coord<double> >& aPtr) { mAngle = aPtr; };
    /**
     * Returns the joint's angular coordinate.
     * \return The joint's angular coordinate.
     */
    shared_ptr< gen_coord<double> > Angle() const { return mAngle; };
    
    /**
     * Sets the joint's base frame.
     * \param aPtr The new joint's base frame.
     */
    void setBaseFrame(const shared_ptr< frame_2D<double> >& aPtr) { mBase = aPtr; };
    /**
     * Returns the joint's base frame.
     * \return The joint's base frame.
     */
    shared_ptr< frame_2D<double> > BaseFrame() const { return mBase; };
    
    /**
     * Sets the joint's output frame.
     * \param aPtr The new joint's output frame.
     */
    void setEndFrame(const shared_ptr< frame_2D<double> >& aPtr) { mEnd = aPtr; };
    /**
     * Returns the joint's output frame.
     * \return The joint's output frame.
     */
    shared_ptr< frame_2D<double> > EndFrame() const { return mEnd; };
    
    /**
     * Sets the joint's Jacobian.
     * \param aPtr The new joint's Jacobian.
     */
    void setJacobian(const shared_ptr< jacobian_gen_2D<double> >& aPtr) { mJacobian = aPtr; };
    /**
     * Returns the joint's Jacobian.
     * \return The joint's Jacobian.
     */
    shared_ptr< jacobian_gen_2D<double> > Jacobian() const { return mJacobian; };
    
    /**
     * Default constructor.
     */
    revolute_joint_2D(const std::string& aName = "") : reacting_kte_gen(aName), mAngle(), mBase(), mEnd(), mJacobian() { };

    /**
     * Parametrized constructor.
     * \param aName the name of this KTE model.
     * \param aAngle the generalized coordinate associated with the displacement of this joint.
     * \param aBase the coordinate frame at the base of the joint.
     * \param aEnd the coordinate frame just after the joint transformations are applied.
     * \param aJacobian a pointer to contain the Jacobian frame produced by this joint, default value will disable the Jacobian frame's calculation.
     */
    revolute_joint_2D(const std::string& aName,
		      const shared_ptr< gen_coord<double> >& aAngle,
		      const shared_ptr< frame_2D<double> >& aBase,
		      const shared_ptr< frame_2D<double> >& aEnd,
                      const shared_ptr< jacobian_gen_2D<double> >& aJacobian = shared_ptr< jacobian_gen_2D<double> >()) :
		      reacting_kte_gen(aName),
		      mAngle(aAngle),
		      mBase(aBase),
		      mEnd(aEnd),
                      mJacobian(aJacobian){ };

    /**
     * Default destructor.
     */
    virtual ~revolute_joint_2D() { };

    virtual void doMotion(kte_pass_flag aFlag = nothing, const shared_ptr<frame_storage>& aStorage = shared_ptr<frame_storage>());

    virtual void doForce(kte_pass_flag aFlag = nothing, const shared_ptr<frame_storage>& aStorage = shared_ptr<frame_storage>());

    virtual void clearForce();

    virtual void applyReactionForce(double aForce);

    virtual void RK_CALL save(serialization::oarchive& A, unsigned int) const {
      reacting_kte_gen::save(A,reacting_kte_gen::getStaticObjectType()->TypeVersion());
      A & RK_SERIAL_SAVE_WITH_NAME(mAngle)
        & RK_SERIAL_SAVE_WITH_NAME(mBase)
        & RK_SERIAL_SAVE_WITH_NAME(mEnd)
	& RK_SERIAL_SAVE_WITH_NAME(mJacobian);
    };

    virtual void RK_CALL load(serialization::iarchive& A, unsigned int) {
      reacting_kte_gen::load(A,reacting_kte_gen::getStaticObjectType()->TypeVersion());
      A & RK_SERIAL_LOAD_WITH_NAME(mAngle)
        & RK_SERIAL_LOAD_WITH_NAME(mBase)
        & RK_SERIAL_LOAD_WITH_NAME(mEnd)
	& RK_SERIAL_LOAD_WITH_NAME(mJacobian);
    };

    RK_RTTI_MAKE_CONCRETE_1BASE(revolute_joint_2D,0xC2100003,1,"revolute_joint_2D",reacting_kte_gen)


};

/**
 * This class implements a revolute joint in 3D space. A generalized coordinate is used to represent the
 * joint's angular displacement between a base coordinate frame to an end coordinate frame.
 */
class revolute_joint_3D : public reacting_kte_gen {
  protected:
    shared_ptr< gen_coord<double> > mAngle; ///< Generalized coordinate representing the joint's angular displacement.
    vect<double,3> mAxis; ///< Joint axis, as a fixed vector, in the coordinate system of the base frame.
    shared_ptr< frame_3D<double> > mBase; ///< The coordinate frame at the base of the joint.
    shared_ptr< frame_3D<double> > mEnd; ///< The coordinate frame just after the joint transformations are applied.

    shared_ptr< jacobian_gen_3D<double> > mJacobian; ///< The Jacobian frame produced by this joint.

  public:
    
    /**
     * Sets the joint's angular coordinate.
     * \param aPtr The new joint's angular coordinate.
     */
    void setAngle(const shared_ptr< gen_coord<double> >& aPtr) { mAngle = aPtr; };
    /**
     * Returns the joint's angular coordinate.
     * \return The joint's angular coordinate.
     */
    shared_ptr< gen_coord<double> > Angle() const { return mAngle; };
    
    /**
     * Sets the joint's axis vector (relative to base frame).
     * \param aValue The new joint's axis vector (relative to base frame).
     */
    void setAxis(const vect<double,3>& aValue) { mAxis = aValue; };
    /**
     * Returns the joint's axis vector (relative to base frame).
     * \return The joint's axis vector (relative to base frame).
     */
    vect<double,3> Axis() const { return mAxis; };
    
    /**
     * Sets the joint's base frame.
     * \param aPtr The new joint's base frame.
     */
    void setBaseFrame(const shared_ptr< frame_3D<double> >& aPtr) { mBase = aPtr; };
    /**
     * Returns the joint's base frame.
     * \return The joint's base frame.
     */
    shared_ptr< frame_3D<double> > BaseFrame() const { return mBase; };
    
    /**
     * Sets the joint's output frame.
     * \param aPtr The new joint's output frame.
     */
    void setEndFrame(const shared_ptr< frame_3D<double> >& aPtr) { mEnd = aPtr; };
    /**
     * Returns the joint's output frame.
     * \return The joint's output frame.
     */
    shared_ptr< frame_3D<double> > EndFrame() const { return mEnd; };
    
    /**
     * Sets the joint's Jacobian.
     * \param aPtr The new joint's Jacobian.
     */
    void setJacobian(const shared_ptr< jacobian_gen_3D<double> >& aPtr) { mJacobian = aPtr; };
    /**
     * Returns a const-reference to the joint's Jacobian.
     * \return The joint's Jacobian.
     */
    shared_ptr< jacobian_gen_3D<double> > Jacobian() const { return mJacobian; };

    /**
     * Default constructor.
     */
    revolute_joint_3D(const std::string& aName = "") : reacting_kte_gen(aName), mAngle(), mAxis(), mBase(), mEnd(), mJacobian() { };

    /**
     * Parametrized constructor.
     * \param aName the name of this KTE model.
     * \param aAngle the generalized coordinate associated with the displacement of this joint.
     * \param aAxis the joint axis, as a fixed vector, in the coordinate system of the base frame.
     * \param aBase the coordinate frame at the base of the joint.
     * \param aEnd the coordinate frame just after the joint transformations are applied.
     * \param aJacobian a pointer to contain the Jacobian frame produced by this joint, default value will disable the Jacobian frame's calculation.
     */
    revolute_joint_3D(const std::string& aName,
		      const shared_ptr< gen_coord<double> >& aAngle,
		      const vect<double,3>& aAxis,
		      const shared_ptr< frame_3D<double> >& aBase,
		      const shared_ptr< frame_3D<double> >& aEnd,
                      const shared_ptr< jacobian_gen_3D<double> >& aJacobian = shared_ptr< jacobian_gen_3D<double> >()) :
		      reacting_kte_gen(aName),
		      mAngle(aAngle),
		      mAxis(aAxis),
		      mBase(aBase),
		      mEnd(aEnd),
                      mJacobian(aJacobian) { };

    /**
     * Default destructor.
     */
    virtual ~revolute_joint_3D() { };

    virtual void doMotion(kte_pass_flag aFlag = nothing, const shared_ptr<frame_storage>& aStorage = shared_ptr<frame_storage>());

    virtual void doForce(kte_pass_flag aFlag = nothing, const shared_ptr<frame_storage>& aStorage = shared_ptr<frame_storage>());

    virtual void clearForce();

    virtual void applyReactionForce(double aForce);

    virtual void RK_CALL save(serialization::oarchive& A, unsigned int) const {
      reacting_kte_gen::save(A,reacting_kte_gen::getStaticObjectType()->TypeVersion());
      A & RK_SERIAL_SAVE_WITH_NAME(mAngle)
        & RK_SERIAL_SAVE_WITH_NAME(mAxis)
        & RK_SERIAL_SAVE_WITH_NAME(mBase)
        & RK_SERIAL_SAVE_WITH_NAME(mEnd)
	& RK_SERIAL_SAVE_WITH_NAME(mJacobian);
    };

    virtual void RK_CALL load(serialization::iarchive& A, unsigned int) {
      reacting_kte_gen::load(A,reacting_kte_gen::getStaticObjectType()->TypeVersion());
      A & RK_SERIAL_LOAD_WITH_NAME(mAngle)
        & RK_SERIAL_LOAD_WITH_NAME(mAxis)
        & RK_SERIAL_LOAD_WITH_NAME(mBase)
        & RK_SERIAL_LOAD_WITH_NAME(mEnd)
	& RK_SERIAL_LOAD_WITH_NAME(mJacobian);
    };

    RK_RTTI_MAKE_CONCRETE_1BASE(revolute_joint_3D,0xC2100004,1,"revolute_joint_3D",reacting_kte_gen)


};


};

};

#endif







