// -*- C++ -*- Tell emacs this is C++
// Kurt Schwehr - Aug 2010 - BSD License
// MB Nuts and Bolts course at CCOM, UNH
/* http://geosoft.no/development/cppstyle.html
   http://www.icce.rug.nl/documents/cplusplus/ 
   http://developer.apple.com/mac/library/documentation/Cocoa/Conceptual/ObjectiveC/Articles/ocCPlusPlus.html
 */

#ifndef __SIMRAD_H__
#define __SIMRAD_H__

enum SimradDgEnum {
    SIMRAD_DG_UNKNOWN = -2,
    SIMRAD_DG_ANY = -1,
    SIMRAD_DG_PU_ID_OUTPUTS = 48,
    SIMRAD_DG_0 = 48,
    SIMRAD_DG_PU_STATUS_OUTPUT = 49,
    SIMRAD_DG_1 = 49,
    SIMRAD_DG_EXTRA_PARAMETERS = 51,
    SIMRAD_DG_3 = 51,
    SIMRAD_DG_ATTITUDE = 65,
    SIMRAD_DG_A = 65,
    SIMRAD_DG_PU_BIST_RESULT = 66,
    SIMRAD_DG_B = 66,
    SIMRAD_DG_CLOCK = 67,
    SIMRAD_DG_C = 67,
    SIMRAD_DG_DEPTH = 68,
    SIMRAD_DG_D = 68,
    SIMRAD_DG_SINGLE_BEAM_ECHO_SOUNDER_DEPTH = 69,
    SIMRAD_DG_E = 69,
    SIMRAD_DG_RAW_RANGE_AND_BEAM_ANGLES_OLD = 70,
    SIMRAD_DG_F = 70,
    SIMRAD_DG_SURFACE_SOUND_SPEED = 71,
    SIMRAD_DG_G = 71,
    SIMRAD_DG_HEADINGS = 72,
    SIMRAD_DG_H = 72,
    SIMRAD_DG_INSTALLATION_PARAMETERS = 73,
    SIMRAD_DG_I = 73,
    SIMRAD_DG_MECHANICAL_TRANSDUCER_TILTS = 74,
    SIMRAD_DG_J = 74,
    SIMRAD_DG_CENTRAL_BEAMS_ECHOGRAM = 75,
    SIMRAD_DG_K = 75,
    SIMRAD_DG_RAW_RANGE_AND_BEAM_ANGLE_78_ = 78,
    SIMRAD_DG_N = 78,
    SIMRAD_DG_POSITIONS = 80,
    SIMRAD_DG_P = 80,
    SIMRAD_DG_RUNTIME_PARAMETERS = 82,
    SIMRAD_DG_R = 82,
    SIMRAD_DG_SEABED_IMAGE_ = 83,
    SIMRAD_DG_S = 83,
    SIMRAD_DG_TIDE = 84,
    SIMRAD_DG_T = 84,
    SIMRAD_DG_SOUND_SPEED_PROFILE = 85,
    SIMRAD_DG_U = 85,
    SIMRAD_DG_KONGSBERG_MARITIME_SSP_OUTPUT = 87,
    SIMRAD_DG_W = 87,
    SIMRAD_DG_XYZ = 88,
    SIMRAD_DG_X = 88,
    SIMRAD_DG_SEABED_IMAGE_DATA_89_ = 89,
    SIMRAD_DG_Y = 89,
    SIMRAD_DG_RAW_RANGE_AND_BEAM_ANGLES_NEW = 102,
    SIMRAD_DG_f = 102,
    SIMRAD_DG_DEPTH_OR_HEIGHT_ = 104,
    SIMRAD_DG_h = 104,
    SIMRAD_DG_INSTALLATION_PARAMETERS_i = 105,
    SIMRAD_DG_i = 105,
    SIMRAD_DG_WATER_COLUMN_ = 107,
    SIMRAD_DG_k = 107,
    SIMRAD_DG_NETWORK_ATTITUDE_VELOCITY = 110,
    SIMRAD_DG_n = 110,
    SIMRAD_DG_REMOTE_INFORMATION = 114,
    SIMRAD_DG_r = 114
};

// FIX how do I expose this?
//char *datagram_names[256];




//////////////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////////////

@interface SimradDg : NSObject
{
	unsigned char dgId;
	unsigned short em_model, clock_counter;
    double timestamp; // Unix UTC time with millisec decimal places
    unsigned short ping_counter;
    unsigned short serial_num;
}
@property (readonly) unsigned char dgId;
@property (readonly) unsigned short em_model, clock_counter, ping_counter, serial_num;
@property (readonly) double timestamp;
-(id)initWithSimradDg:(SimradDg *)dg;
-(id)initWithData:(NSData *)data dgStart:(size_t)dgStart size:(unsigned int)size;
-(NSString *)description;
@end

@interface SimradDgClock : SimradDg
{
	double timestamp_sensor;
    bool pps;
}

@property (readonly) double timestamp_sensor;
@property (readonly) bool pps;
-(id)initWithSimradDg:(SimradDg *)dg data:(NSData *)data dgStart:(size_t)dgStart size:(unsigned int)size;
@end

#if 0
@interface SimradDgDepth : SimradDg
{
	vector<>;
}

@property (readonly) double ;
-(id)initWithSimradDg:(SimradDg *)dg data:(NSData *)data dgStart:(size_t)dgStart size:(unsigned int)size;
@end
#endif

#endif // __SIMRAD_H__
