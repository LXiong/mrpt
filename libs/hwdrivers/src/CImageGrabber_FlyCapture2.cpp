/* +---------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)               |
   |                          http://www.mrpt.org/                             |
   |                                                                           |
   | Copyright (c) 2005-2014, Individual contributors, see AUTHORS file        |
   | See: http://www.mrpt.org/Authors - All rights reserved.                   |
   | Released under BSD License. See details in http://www.mrpt.org/License    |
   +---------------------------------------------------------------------------+ */

#include <mrpt/hwdrivers.h> // Precompiled header

#if MRPT_HAS_FLYCAPTURE2
	#include <FlyCapture2.h>
	using namespace FlyCapture2;
#endif

#define CHECK_FC2_ERROR(_err) { if (_err != PGRERROR_OK) { THROW_EXCEPTION_CUSTOM_MSG1("FlyCapture2 error:\n%s",_err.GetDescription()) } }
#define FC2_CAM  reinterpret_cast<FlyCapture2::Camera*>(m_camera)
#define FC2_CAM_INFO  reinterpret_cast<FlyCapture2::CameraInfo*>(m_camera_info)

using namespace mrpt::hwdrivers;

#if MRPT_HAS_FLYCAPTURE2
// Declare a table to convert strings to their #define values:
struct fc2_str_val {
	const char* str;
	int val;
};

const fc2_str_val fc2_vals[] = {
	{ "VIDEOMODE_160x120YUV444",VIDEOMODE_160x120YUV444},
	{ "VIDEOMODE_320x240YUV422",VIDEOMODE_320x240YUV422},
	{ "VIDEOMODE_640x480YUV411",VIDEOMODE_640x480YUV411},
	{ "VIDEOMODE_640x480YUV422",VIDEOMODE_640x480YUV422},
	{ "VIDEOMODE_640x480RGB",VIDEOMODE_640x480RGB},
	{ "VIDEOMODE_640x480Y8",VIDEOMODE_640x480Y8},
	{ "VIDEOMODE_640x480Y16",VIDEOMODE_640x480Y16},
	{ "VIDEOMODE_800x600YUV422",VIDEOMODE_800x600YUV422},
	{ "VIDEOMODE_800x600RGB",VIDEOMODE_800x600RGB},
	{ "VIDEOMODE_800x600Y8",VIDEOMODE_800x600Y8},
	{ "VIDEOMODE_800x600Y16",VIDEOMODE_800x600Y16},
	{ "VIDEOMODE_1024x768YUV422",VIDEOMODE_1024x768YUV422},
	{ "VIDEOMODE_1024x768RGB",VIDEOMODE_1024x768RGB},
	{ "VIDEOMODE_1024x768Y8",VIDEOMODE_1024x768Y8},
	{ "VIDEOMODE_1024x768Y16",VIDEOMODE_1024x768Y16},
	{ "VIDEOMODE_1280x960YUV422",VIDEOMODE_1280x960YUV422},
	{ "VIDEOMODE_1280x960RGB",VIDEOMODE_1280x960RGB},
	{ "VIDEOMODE_1280x960Y8",VIDEOMODE_1280x960Y8},
	{ "VIDEOMODE_1280x960Y16",VIDEOMODE_1280x960Y16},
	{ "VIDEOMODE_1600x1200YUV422",VIDEOMODE_1600x1200YUV422},
	{ "VIDEOMODE_1600x1200RGB",VIDEOMODE_1600x1200RGB},
	{ "VIDEOMODE_1600x1200Y8",VIDEOMODE_1600x1200Y8},
	{ "VIDEOMODE_1600x1200Y16",VIDEOMODE_1600x1200Y16},
	// -----------
	{ "FRAMERATE_1_875",FlyCapture2::FRAMERATE_1_875},
	{ "FRAMERATE_3_75",FlyCapture2::FRAMERATE_3_75},
	{ "FRAMERATE_7_5",FlyCapture2::FRAMERATE_7_5},
	{ "FRAMERATE_15",FlyCapture2::FRAMERATE_15},
	{ "FRAMERATE_30",FlyCapture2::FRAMERATE_30},
	{ "FRAMERATE_60",FlyCapture2::FRAMERATE_60},
	{ "FRAMERATE_120",FlyCapture2::FRAMERATE_120},
	{ "FRAMERATE_240",FlyCapture2::FRAMERATE_240},
	// ------------------
	{ "DROP_FRAMES",DROP_FRAMES},
	{ "BUFFER_FRAMES",BUFFER_FRAMES}
};

template <typename T>
T fc2_defstr2num(const std::string &str)
{
	const std::string s = mrpt::utils::trim(str);
	for (unsigned int i=0;i<sizeof(fc2_vals)/sizeof(fc2_vals[0]);i++)
	{
		if (strCmpI(fc2_vals[i].str,s.c_str()))
			return static_cast<T>(fc2_vals[i].val);
	}
	THROW_EXCEPTION_CUSTOM_MSG1("Error: Unknown FlyCapture2 constant: %s",s.c_str())
}
#endif


//  Options: TCaptureOptions_bumblebee 
// -------------------------------------------------------------
TCaptureOptions_FlyCapture2::TCaptureOptions_FlyCapture2() :
	camera_index (0),
	open_by_guid (false),
	videomode(), //("VIDEOMODE_640x480Y8"),
	framerate(), // ("FRAMERATE_30"),
	grabmode("BUFFER_FRAMES"),
	grabTimeout(-1)
{
	memset(camera_guid,0,4*sizeof(camera_guid[0]));
}

// ---------------------------------------------------------------
/** Default constructor */
CImageGrabber_FlyCapture2::CImageGrabber_FlyCapture2() : 
	m_camera(NULL),
	m_camera_info(NULL)
{
}

/** Constructor + open */
CImageGrabber_FlyCapture2::CImageGrabber_FlyCapture2( const TCaptureOptions_FlyCapture2 &options ) : 
	m_camera(NULL),
	m_camera_info(NULL)
{
	this->open(options);
}

/** Destructor */
CImageGrabber_FlyCapture2::~CImageGrabber_FlyCapture2()
{
	this->close();
}


/** Tries to open the camera with the given options. Raises an exception on error. \sa close() */
void CImageGrabber_FlyCapture2::open( const TCaptureOptions_FlyCapture2 &options, const bool startCapture )
{
#if MRPT_HAS_FLYCAPTURE2
    Error error;

	cout << "[CImageGrabber_FlyCapture2::open] FlyCapture2 version: " << CImageGrabber_FlyCapture2::getFC2version() << std::endl;

	this->close();
	this->m_options = options;

	// Determine camera to open:
	// -----------------------------------
	PGRGuid guid;
	if (m_options.open_by_guid)
	{
		// Open by GUID:
		for (int i=0;i<4;i++) 
			guid.value[i] = m_options.camera_guid[i];
	}
	else
	{
		// Open by camera index:
		BusManager busMgr;
		unsigned int numCameras;
		error = busMgr.GetNumOfCameras(&numCameras);
		CHECK_FC2_ERROR(error)

		if (m_options.camera_index>=numCameras)
			THROW_EXCEPTION(mrpt::format("Error: camera_index to open is '%u', but only '%u' cameras were detected in the system.",m_options.camera_index,numCameras))

		error = busMgr.GetCameraFromIndex(m_options.camera_index, &guid);
		CHECK_FC2_ERROR(error)
	}

	// Connect to camera:
	m_camera = new FlyCapture2::Camera();
	m_camera_info = new FlyCapture2::CameraInfo();

	cout << mrpt::format("[CImageGrabber_FlyCapture2::open] Opening camera with GUID= %04X%04X%04X%04X...\n", guid.value[0],guid.value[1],guid.value[2],guid.value[3]);
    error = FC2_CAM->Connect(&guid);
	CHECK_FC2_ERROR(error)
	error=FC2_CAM->GetCameraInfo(FC2_CAM_INFO);
	CHECK_FC2_ERROR(error)

	const FlyCapture2::CameraInfo *ci = FC2_CAM_INFO;

	cout << mrpt::format(
	"[CImageGrabber_FlyCapture2::open] Camera connected ok:\n"
		" Serial number - %u\n"
		" Camera model - %s\n"
		" Camera vendor - %s\n"
		" Sensor - %s\n"
		" Resolution - %s\n"
		" Firmware version - %s\n"
		" Firmware build time - %s\n\n",
		ci->serialNumber,
		ci->modelName,
		ci->vendorName,
		ci->sensorInfo,
		ci->sensorResolution,
		ci->firmwareVersion,
		ci->firmwareBuildTime );

	// Set camera config:
	if (!m_options.videomode.empty() && !m_options.framerate.empty())
	{
		FlyCapture2::VideoMode vidMode = fc2_defstr2num<FlyCapture2::VideoMode>(m_options.videomode);
		FlyCapture2::FrameRate vidRate = fc2_defstr2num<FlyCapture2::FrameRate>(m_options.framerate);
		bool isSupported = false;
		error = FC2_CAM->GetVideoModeAndFrameRateInfo(vidMode,vidRate, &isSupported);
		CHECK_FC2_ERROR(error)

		if (!isSupported)
		{
			FlyCapture2::VideoMode curVidMode;
			FlyCapture2::FrameRate curVidRate;
			error = FC2_CAM->GetVideoModeAndFrameRate(&curVidMode,&curVidRate);

			THROW_EXCEPTION(mrpt::format("Camera mode '%s' + '%s' is not supported by this camera. Current mode is %d, current rate is %d.",m_options.videomode.c_str(),m_options.framerate.c_str(),static_cast<int>(curVidMode),static_cast<int>(curVidRate) ))
		}

		error = FC2_CAM->SetVideoModeAndFrameRate(vidMode,vidRate);
		CHECK_FC2_ERROR(error)
	}

	{
		FlyCapture2::VideoMode curVidMode;
		FlyCapture2::FrameRate curVidRate;
		error = FC2_CAM->GetVideoModeAndFrameRate(&curVidMode,&curVidRate);
		if (error==PGRERROR_OK)
			cout << mrpt::format("[CImageGrabber_FlyCapture2::open] Current camera mode is %d, current rate is %d.\n",static_cast<int>(curVidMode),static_cast<int>(curVidRate) );
	}


	// Set trigger:
	MRPT_TODO("Trigger mode")
	FlyCapture2::TriggerMode trig;
//	error = FC2_CAM->SetTriggerMode(&trig);
//	CHECK_FC2_ERROR(error)

	// Set configs:
	FlyCapture2::FC2Config fc2conf;
	FC2_CAM->GetConfiguration(&fc2conf);
	CHECK_FC2_ERROR(error)

	fc2conf.grabMode = fc2_defstr2num<FlyCapture2::GrabMode>(m_options.grabmode);
	if (m_options.grabTimeout>=0)
		fc2conf.grabTimeout = m_options.grabTimeout;

	error = FC2_CAM->SetConfiguration( &fc2conf);
	CHECK_FC2_ERROR(error)

	// Autoexposure:
    {
		FlyCapture2::Property p;
		p.type = FlyCapture2::AUTO_EXPOSURE;
		p.onOff = true;
		error = FC2_CAM->SetProperty (&p);
	}

	// Brightness:
    {
		FlyCapture2::Property p;
		p.type = FlyCapture2::BRIGHTNESS;
		p.autoManualMode = true; // true=auto
		//p.absControl = true;
		//p.absValue = Brightness;
		error = FC2_CAM->SetProperty (&p);
	}

    {
		FlyCapture2::Property p;
		p.type = FlyCapture2::SHUTTER;
		p.autoManualMode = true; // true=auto
	    //p.absControl = true;
	    //p.absValue = Shutter;
	    //p.onOff = false;
		error = FC2_CAM->SetProperty (&p);
	}

    {
		FlyCapture2::Property p;
		p.type = FlyCapture2::GAIN;
		p.autoManualMode = true; // true=auto
	    //p.absControl = true;
	    //p.absValue = Gain;
	    //p.onOff = false;
		error = FC2_CAM->SetProperty (&p);
	}

	// Framecounter:
	EmbeddedImageInfo eii;
	error = FC2_CAM->GetEmbeddedImageInfo(&eii);
	if (error == PGRERROR_OK)
	{
		if (eii.frameCounter.available) eii.frameCounter.onOff = true; 
		if (eii.timestamp.available)    eii.timestamp.onOff = true;
		if (eii.exposure.available)     eii.exposure.onOff = true;
		if (eii.brightness.available)   eii.brightness.onOff = true;

		// Enable all:
		FC2_CAM->SetEmbeddedImageInfo(&eii);
	}

	
	// Start:
	if (startCapture)
		this->startCapture();
#else
	THROW_EXCEPTION("MRPT compiled without support for FlyCapture2")
#endif
}

/** Start the actual image capture of the camera. Must be called after open(), only when "startCapture" was set to false. */
void CImageGrabber_FlyCapture2::startCapture()
{
#if MRPT_HAS_FLYCAPTURE2
	if (!m_camera) { THROW_EXCEPTION("Camera is not opened. Call open() first.") }
	
	FlyCapture2::Error error = FC2_CAM->StartCapture();
	CHECK_FC2_ERROR(error)

#else
	THROW_EXCEPTION("MRPT compiled without support for FlyCapture2")
#endif
}

/** Starts a synchronous capture of several cameras, which must have been already opened. */
void CImageGrabber_FlyCapture2::startSyncCapture( int numCameras, const CImageGrabber_FlyCapture2 **cameras_array )
{
#if MRPT_HAS_FLYCAPTURE2
	MRPT_TODO("Impl.")
	//StartSyncCapture()
#else
	THROW_EXCEPTION("MRPT compiled without support for FlyCapture2")
#endif
}

/** Stop capture. */
void CImageGrabber_FlyCapture2::stopCapture()
{
#if MRPT_HAS_FLYCAPTURE2
	if (m_camera)
	{
		Error error;

		// Stop grabbing:
		error = FC2_CAM->StopCapture();
		CHECK_FC2_ERROR(error)
	}
	else
	{
		std::cerr << "[CImageGrabber_FlyCapture2::stopCapture] Warning: called on an non-opened camera.\n";
	}
#else
	THROW_EXCEPTION("MRPT compiled without support for FlyCapture2")
#endif
}


/** Closes the opened camera, if any. Called automatically on object destruction. */
void CImageGrabber_FlyCapture2::close()
{
#if MRPT_HAS_FLYCAPTURE2
	try {
		this->stopCapture();
	}
	catch (...) { }

	// Disconnect the camera
	try {
		if (m_camera) FC2_CAM->Disconnect();
	}
	catch (...) { }

	// Delete objects:
	try { if (m_camera) delete FC2_CAM;  } catch (...) {} 
	try { if (m_camera_info) delete FC2_CAM_INFO; } catch (...) {} 

	m_camera=NULL;
	m_camera_info=NULL;

#else
	THROW_EXCEPTION("MRPT compiled without support for FlyCapture2")
#endif
}


/** Returns the PGR FlyCapture2 library version */
std::string CImageGrabber_FlyCapture2::getFC2version()
{
#if MRPT_HAS_FLYCAPTURE2
    FlyCapture2::FC2Version fc2Version;
    FlyCapture2::Utilities::GetLibraryVersion( &fc2Version );
    return mrpt::format("%d.%d.%d.%d", fc2Version.major, fc2Version.minor, fc2Version.type, fc2Version.build);
#else
	THROW_EXCEPTION("MRPT compiled without support for FlyCapture2")
#endif
}


// Grab image from the camera. This method blocks until the next frame is captured.
// return: false on any error.
bool CImageGrabber_FlyCapture2::getObservation( mrpt::slam::CObservationImage &out_observation )
{
#if MRPT_HAS_FLYCAPTURE2
	if (!m_camera) { 
		std::cerr << "[CImageGrabber_FlyCapture2::getObservation] Camera is not opened. Call open() first.\n"; 
		return false;
	}

	try
	{
		FlyCapture2::Error error;
		FlyCapture2::Image image;
	

		error = FC2_CAM->RetrieveBuffer( &image );
		CHECK_FC2_ERROR(error)

		FlyCapture2::TimeStamp timestamp = image.GetTimeStamp();
		out_observation.timestamp = mrpt::utils::time_tToTimestamp( timestamp.seconds + 1e-6*timestamp.microSeconds );

		// White balance, etc.
		FlyCapture2::ImageMetadata imd = image.GetMetadata();

		FlyCapture2::PixelFormat pf = image.GetPixelFormat();

		FlyCapture2::Image image_converted;
		//error = image.Convert(PIXEL_FORMAT_MONO8,&image_converted);
		error = image.Convert(PIXEL_FORMAT_RGB8,&image_converted);
		CHECK_FC2_ERROR(error)

		MRPT_TODO("proper convert")
		image_converted.Save("prueba.tif");
		out_observation.image.loadFromFile("prueba.tif");

		return true;
	}
	catch( std::exception &e)
	{
		std::cerr << "[CImageGrabber_FlyCapture2::getObservation] Error:\n" << e.what() << std::endl; 
		return false;
	}
#else
	THROW_EXCEPTION("MRPT compiled without support for FlyCapture2")
#endif
}


