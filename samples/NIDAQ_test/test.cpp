/* +---------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)               |
   |                          http://www.mrpt.org/                             |
   |                                                                           |
   | Copyright (c) 2005-2014, Individual contributors, see AUTHORS file        |
   | See: http://www.mrpt.org/Authors - All rights reserved.                   |
   | Released under BSD License. See details in http://www.mrpt.org/License    |
   +---------------------------------------------------------------------------+ */

#include <mrpt/base.h>
#include <mrpt/hwdrivers/CNationalInstrumentsDAQ.h>

using namespace mrpt;
using namespace mrpt::utils;
using namespace mrpt::slam;
using namespace mrpt::hwdrivers;
using namespace std;

// ------------------------------------------------------
//				Test_NIDAQ
// ------------------------------------------------------
void Test_NIDAQ()
{
	CNationalInstrumentsDAQ	 daq;

	// Load config:
#if 0
	//daq.loadConfig( CConfigFile( "./DAQ_example.ini") ,"DAQ1" );
#else
	// Or set params programatically:

	// Define a task with analog inputs:
	CNationalInstrumentsDAQ::TaskDescription task;
	task.has_ai = true;
	task.ai.physicalChannel = "Dev1/ai0:7";
	task.ai.physicalChannelCount = 8; // Must be the number of channels encoded in the "physicalChannel" string.
	task.ai.terminalConfig  = "DAQmx_Val_RSE";
	task.ai.minVal = -10;
	task.ai.maxVal =  10;

	daq.task_definitions.push_back(task);

#endif

	printf("[Example] Initializing DAQ...\n");
	daq.initialize();
	printf("[Example] Init passed.\n");

	printf("\n ** Press any key to stop grabbing ** \n");

	while (!mrpt::system::os::kbhit())
	{
		std::vector<mrpt::slam::CObservationRawDAQPtr> readObs;
		bool hardError;

		try
		{
			daq.readFromDAQ( readObs, hardError );
		}
		catch (std::exception &e)
		{
			cerr << e.what() << endl;
			hardError = true;
		}

		if (hardError)
			printf("[TEST] Hardware error=true!!\n");

		if (!readObs.empty())
		{
			// Look for analog readings:
			for (size_t i=0;i<readObs.size();i++)
			{
				if (readObs[i]->AIN_double.empty())
					continue; // Skip

				const size_t nSamplPerChan = readObs[i]->AIN_double.size() / readObs[i]->AIN_channel_count;
				cout << "Read " << nSamplPerChan << " samples. a[0]=" << readObs[i]->AIN_double[0] << endl;
			}
		}

		mrpt::system::sleep(1);
	};
}

int main()
{
	try
	{
		Test_NIDAQ();
		return 0;

	} catch (std::exception &e)
	{
		std::cout << "EXCEPTION: " << e.what() << std::endl;
		return -1;
	}
}

