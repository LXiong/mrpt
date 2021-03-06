/* +---------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)               |
   |                          http://www.mrpt.org/                             |
   |                                                                           |
   | Copyright (c) 2005-2014, Individual contributors, see AUTHORS file        |
   | See: http://www.mrpt.org/Authors - All rights reserved.                   |
   | Released under BSD License. See details in http://www.mrpt.org/License    |
   +---------------------------------------------------------------------------+ */

#include <mrpt/hmtslam.h>
#include <mrpt/utils.h>


using namespace mrpt::utils;
using namespace mrpt::hmtslam;


void registerAllClasses_mrpt_hmtslam();

CStartUpClassesRegister  mrpt_hmtslam_class_reg(&registerAllClasses_mrpt_hmtslam);


/*---------------------------------------------------------------
					registerAllClasses_mrpt_hmtslam
  ---------------------------------------------------------------*/
void registerAllClasses_mrpt_hmtslam()
{
	registerClass( CLASS_ID(CHMTSLAM) );
	registerClass( CLASS_ID(CLSLAMParticleData) );
	registerClass( CLASS_ID(CHierarchicalMHMap) );
	registerClass( CLASS_ID(CHMHMapArc) );
	registerClass( CLASS_ID(CHMHMapNode) );
	registerClass( CLASS_ID(CRobotPosesGraph ) );
	registerClass( CLASS_ID(THypothesisIDSet) );
	registerClass( CLASS_ID(CLocalMetricHypothesis) );
}


