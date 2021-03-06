/* +---------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)               |
   |                          http://www.mrpt.org/                             |
   |                                                                           |
   | Copyright (c) 2005-2014, Individual contributors, see AUTHORS file        |
   | See: http://www.mrpt.org/Authors - All rights reserved.                   |
   | Released under BSD License. See details in http://www.mrpt.org/License    |
   +---------------------------------------------------------------------------+ */
#ifndef CObservationWirelessPower_H
#define CObservationWirelessPower_H

#include <mrpt/utils/CSerializable.h>
#include <mrpt/slam/CObservation.h>
#include <mrpt/poses/CPose3D.h>
#include <mrpt/poses/CPose2D.h>

namespace mrpt
{
namespace slam
{
	DEFINE_SERIALIZABLE_PRE_CUSTOM_BASE_LINKAGE( CObservationWirelessPower, CObservation, OBS_IMPEXP)

	/** This represents a measurement of the wireless strength perceived by the robot.
	 *  The signal level is given as a percentage.
	 *
	 * \sa CObservation, mrpt::hwdrivers::CWirelessPower for a software sensor capable of reading this kind of observations.
	 * \ingroup mrpt_obs_grp
	 */
	class OBS_IMPEXP CObservationWirelessPower : public CObservation
	{
		// This must be added to any CSerializable derived class:
		DEFINE_SERIALIZABLE( CObservationWirelessPower )

	 public:
		/** Constructor */
		CObservationWirelessPower( );

		 /** @name The data members
		  * @{ */

		double power;  //!< The power or signal strength as sensed by the Wifi receiver (In percentage: [0-100])
		mrpt::poses::CPose3D  sensorPoseOnRobot; //!< The location of the sensing antenna on the robot coordinate framework

		/** @} */

		void getSensorPose( CPose3D &out_sensorPose ) const;

		/** A general method to change the sensor pose on the robot.
		  *  It has no effects in this class
		  * \sa getSensorPose  */
		void setSensorPose( const CPose3D &newSensorPose );

	}; // End of class def.


	} // End of namespace
} // End of namespace

#endif
