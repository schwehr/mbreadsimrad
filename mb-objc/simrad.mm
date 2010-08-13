// -*- C++ -*- Tell emacs this is C++, but it really is Objective-C++
// Kurt Schwehr - Aug 2010 - BSD License
// MB Nuts and Bolts course at CCOM, UNH
/* http://geosoft.no/development/cppstyle.html
 http://www.icce.rug.nl/documents/cplusplus/ 
 http://developer.apple.com/mac/library/documentation/Cocoa/Conceptual/ObjectiveC/Articles/ocCPlusPlus.html
 */
#import <Foundation/Foundation.h>

#import "simrad.h"
#include <iostream>

using namespace std;

const unsigned char STX = 2; // Start-of-transmission
const unsigned char ETX = 3; // End-of-transmission


#define GET_U4(start,offset) ((unsigned int) *((unsigned int *) (start+offset)))
#define GET_U2(start,offset) ((unsigned short) *((unsigned short *) (start+offset)))
#define GET_U1(start,offset) ((unsigned char) *((unsigned char *) (start+offset)))

unsigned short dg_checksum(const unsigned char *dg_data, const size_t size);
time_t emtime2unixtime(const unsigned int date, const unsigned int millisec);
bool emtime2tm_struct (const unsigned int date, const unsigned int millisec, struct tm &t);

const char *datagram_names[256] = {
    "0 unknown",    "1 unknown",    "2 unknown",    "3 unknown",    "4 unknown",    "5 unknown",
    "6 unknown",    "7 unknown",    "8 unknown",    "9 unknown",    "10 unknown",    "11 unknown",
    "12 unknown",    "13 unknown",    "14 unknown",    "15 unknown",    "16 unknown",    "17 unknown",
    "18 unknown",    "19 unknown",    "20 unknown",    "21 unknown",    "22 unknown",    "23 unknown",
    "24 unknown",    "25 unknown",    "26 unknown",    "27 unknown",    "28 unknown",    "29 unknown",
    "30 unknown",    "31 unknown",    "32 unknown",    "33 unknown",    "34 unknown",    "35 unknown",
    "36 unknown",    "37 unknown",    "38 unknown",    "39 unknown",    "40 unknown",    "41 unknown",
    "42 unknown",    "43 unknown",    "44 unknown",    "45 unknown",    "46 unknown",    "47 unknown",
    "48 PU Id outputs",
    "49 PU Status output",
    "50 unknown",
    "51 Extra Parameters",
    "52 unknown",    "53 unknown",    "54 unknown",    "55 unknown",    "56 unknown",    "57 unknown",
    "58 unknown",    "59 unknown",    "60 unknown",    "61 unknown",    "62 unknown",    "63 unknown",
    "64 unknown",
    "65 Attitude ",
    "66 PU BIST result",
    "67 Clock",
    "68 depth",
    "69 Single beam depth ",
    "70 Range and beam angles (old)",
    "71 Surface sound speed",
    "72 Headings",
    "73 Installation parameters",
    "74 Mechanical transducer tilts",
    "75 Central beams echogram",
    "76 unknown",    "77 unknown",
    "78 Range and beam angle",
    "79 unknown",
    "80 Positions",
    "81 unknown",
    "82 Runtime parameters",
    "83 Seabed image ",
    "84 Tide",
    "85 Sound speed profile",
    "86 unknown",
    "87 Kongsberg Maritime SSP output",
    "88 XYZ",
    "89 Seabed image data 89 ",
    "90 unknown",    "91 unknown",    "92 unknown",    "93 unknown",    "94 unknown",    "95 unknown",
    "96 unknown",    "97 unknown",    "98 unknown",    "99 unknown",    "100 unknown",    "101 unknown",
    "102 Raw range and beam angles (new)",
    "103 unknown",
    "104 Depth (pressure) or height ",
    "105 installation parameters",
    "106 unknown",
    "107 Water column ",
    "108 unknown",    "109 unknown",
    "110 Network attitude velocity",
    "111 unknown",    "112 unknown",    "113 unknown",
    "114 remote information",
    "115 unknown",    "116 unknown",    "117 unknown",    "118 unknown",    "119 unknown",    "120 unknown",
    "121 unknown",    "122 unknown",    "123 unknown",    "124 unknown",    "125 unknown",    "126 unknown",
    "127 unknown",    "128 unknown",    "129 unknown",    "130 unknown",    "131 unknown",    "132 unknown",
    "133 unknown",    "134 unknown",    "135 unknown",    "136 unknown",    "137 unknown",    "138 unknown",
    "139 unknown",    "140 unknown",    "141 unknown",    "142 unknown",    "143 unknown",    "144 unknown",
    "145 unknown",    "146 unknown",    "147 unknown",    "148 unknown",    "149 unknown",    "150 unknown",
    "151 unknown",    "152 unknown",    "153 unknown",    "154 unknown",    "155 unknown",    "156 unknown",
    "157 unknown",    "158 unknown",    "159 unknown",    "160 unknown",    "161 unknown",    "162 unknown",
    "163 unknown",    "164 unknown",    "165 unknown",    "166 unknown",    "167 unknown",    "168 unknown",
    "169 unknown",    "170 unknown",    "171 unknown",    "172 unknown",    "173 unknown",    "174 unknown",
    "175 unknown",    "176 unknown",    "177 unknown",    "178 unknown",    "179 unknown",    "180 unknown",
    "181 unknown",    "182 unknown",    "183 unknown",    "184 unknown",    "185 unknown",    "186 unknown",
    "187 unknown",    "188 unknown",    "189 unknown",    "190 unknown",    "191 unknown",    "192 unknown",
    "193 unknown",    "194 unknown",    "195 unknown",    "196 unknown",    "197 unknown",    "198 unknown",
    "199 unknown",    "200 unknown",    "201 unknown",    "202 unknown",    "203 unknown",    "204 unknown",
    "205 unknown",    "206 unknown",    "207 unknown",    "208 unknown",    "209 unknown",    "210 unknown",
    "211 unknown",    "212 unknown",    "213 unknown",    "214 unknown",    "215 unknown",    "216 unknown",
    "217 unknown",    "218 unknown",    "219 unknown",    "220 unknown",    "221 unknown",    "222 unknown",
    "223 unknown",    "224 unknown",    "225 unknown",    "226 unknown",    "227 unknown",    "228 unknown",
    "229 unknown",    "230 unknown",    "231 unknown",    "232 unknown",    "233 unknown",    "234 unknown",
    "235 unknown",    "236 unknown",    "237 unknown",    "238 unknown",    "239 unknown",    "240 unknown",
    "241 unknown",    "242 unknown",    "243 unknown",    "244 unknown",    "245 unknown",    "246 unknown",
    "247 unknown",    "248 unknown",    "249 unknown",    "250 unknown",    "251 unknown",    "252 unknown",
    "253 unknown",    "254 unknown",    "255 unknown",
};


unsigned short compute_checksum(const unsigned char *dg_data, const size_t size) {
    // Checksum of bytes between the STX and ETX characters
    assert(dg_data);
    assert(size < 128000); // Assumption of size
    unsigned short sum = 0;
    for (size_t i=5; i<4+size-3; i++) 
        sum += GET_U1(dg_data,i);
    return sum;
}

bool emtime2tm_struct (const unsigned int date, const unsigned int millisec, struct tm &t);

// Convert a date_raw and millisec_raw to a time.h / struct tm
// Using boost would be a lot more efficient
bool emtime2tm_struct (const unsigned int date,
					   const unsigned int millisec, 
					   struct tm &t) {
    const int tm_year = date / 10000;
    const int tm_mon = (date % 10000) / 100;
    const int tm_mday = date % 100;
	
    const int tm_hour = millisec / (1000 * 60 * 60);
    const int tm_min = (millisec % (1000 * 60 * 60)) / (1000 * 60) ;
    const int tm_sec = (millisec % (1000 * 60)) / 1000 ;
	
    // Grrr... Hate going through a string to make this work
    char buf[256];
    snprintf(buf, 256, "%4d-%02d-%02dT%02d:%02d:%02dZ", tm_year, tm_mon, tm_mday,
             tm_hour, tm_min, tm_sec);
	
    if (0 == strptime(buf, "%Y-%m-%dT%H:%M:%SZ", &t)) {
        perror("Unable to parse ISO time");
		cerr << "  " << buf << endl;
        return false;
    }
    t.tm_zone = "UTC";
    t.tm_gmtoff = 0;
    return true;
}

//time_t emtime2unixtime(const unsigned int date, const unsigned int millisec);
time_t emtime2unixtime(const unsigned int date,
					   const unsigned int millisec) {
    struct tm t;
    if (! emtime2tm_struct (date, millisec, t)) {
        cerr << "Unable to convert time in emtime2unixtime " << date << " " << millisec << endl;
        return 0;
    }
    return timegm(&t);
}

double emtime2unixtime_double(const unsigned int date, const unsigned int millisec) {
    int sec = emtime2unixtime(date, millisec);
    double sec_decimal = (millisec % 1000) / 1000.;
    return sec + sec_decimal;
}

@implementation SimradDgClock
-(NSString *)description {
	return [NSString stringWithFormat:@"DgClock: %.4lf %.4lf dt: %.4lf pps: %s", 
			timestamp, timestamp_sensor, timestamp - timestamp_sensor, (pps?"on":"off")];
}
@synthesize timestamp_sensor, pps;
-(id)initWithSimradDg:(SimradDg *)dg data:(NSData *)data dgStart:(size_t)dgStart size:(unsigned int)size {
	//NSLog(@"In SimradDgClock initWithSimradDg");
	self = [super initWithSimradDg:dg];
	if (!self) return self;
	unsigned int date, ms;
	[data getBytes:&date range:NSMakeRange(dgStart+16, 4)];
    [data getBytes:&ms   range:NSMakeRange(dgStart+20, 4)];
    timestamp_sensor = emtime2unixtime_double(date, ms);
	//NSLog(@"time external: %d %d %0.4lf", date, ms, timestamp_sensor);
	[data getBytes:&pps range:NSMakeRange(dgStart+24, 1)];
    //pps = bool(GET_U1(data.data+dgStart+24,1));
	return self;
}
@end

@implementation SimradDgPos
-(NSString *)description {
	return [NSString stringWithFormat:@"DgPos: %.4lf %.4lf cog: %.4lf heading: %lf", x, y, cog, heading];
}
-(id)initWithSimradDg:(SimradDg *)dg data:(NSData *)data dgStart:(size_t)dgStart size:(unsigned int)size {
	NSLog(@"In SimradDgPos initWithSimradDg");
	self = [super initWithSimradDg:dg];
	if (!self) return self;
	unsigned int tmpU4;
	unsigned short tmpU2;
	[data getBytes:&tmpU4 range:NSMakeRange(dgStart+16, 4)]; y = tmpU4 / 2.e7;
	[data getBytes:&tmpU4 range:NSMakeRange(dgStart+20, 4)]; x = tmpU4 / 1.e7;
	[data getBytes:&fixQualCM range:NSMakeRange(dgStart+24, 2)];
	[data getBytes:&sogCMS range:NSMakeRange(dgStart+26, 2)];
	[data getBytes:&tmpU2 range:NSMakeRange(dgStart+28, 2)]; cog = tmpU2 * 1e-2;
	[data getBytes:&tmpU2 range:NSMakeRange(dgStart+30, 2)]; heading = tmpU2 * 1e-2;
	//[data getBytes:& range:NSMakeRange(dgStart+, )];
	return self;
}

@end



@implementation SimradDg
@synthesize dgId, em_model, clock_counter, timestamp, ping_counter, serial_num;
- (NSString *)description {
    return [NSString stringWithFormat: @"unhandled datagram: dgId: %d name: %s", dgId, datagram_names[dgId]];
}
-(id)initWithSimradDg:(SimradDg *)dg {
	self = [super init];
    if (!self) return self;

	// Copy over the parameters.  There has to be a way to do a copy of an object
	dgId = dg.dgId;
	em_model = dg.em_model;
	clock_counter = dg.clock_counter;
	timestamp = dg.timestamp;
    ping_counter = dg.ping_counter;
    serial_num = dg.serial_num;
	return self;
}

-(id)initWithData:(NSData *)data dgStart:(size_t)dgStart size:(unsigned int)size {
	// dgStart must start after the size, right at the beginning of the STX
	
    self = [super init];
    if (!self) return self;
	// STX is at zero
    [data getBytes:&dgId  range:NSMakeRange(dgStart+1, 1)];
    [data getBytes:&em_model range:NSMakeRange(dgStart+2, 2)];
	//NSLog(@"id: %d model: %d",dgId, em_model);
	
	unsigned int date_raw, ms_raw;
	[data getBytes:&date_raw range:NSMakeRange(dgStart+4, 4)];
	[data getBytes:&ms_raw   range:NSMakeRange(dgStart+8, 4)];
	timestamp = emtime2unixtime_double(date_raw, ms_raw); // Stand in for time
	//NSLog(@"FIX: are these correct?2 %d %d %lf", date_raw, ms_raw, timestamp);
	
    [data getBytes:&ping_counter range:NSMakeRange(dgStart+10, 2)];
    [data getBytes:&serial_num range:NSMakeRange(dgStart+12, 2)];
	
	id self_old = self;
	switch (dgId) {
#if 0
		case SIMRAD_DG_RUNTIME_PARAMETERS:
			NSLog(@"Trying to create a RUNTIME PARAM");
			self = [[SimradDgRuntimeParam alloc] initWithSimradDg:self];
			break;
#endif
		case SIMRAD_DG_CLOCK:
			//NSLog(@"Trying to create a CLOCK");
			self = [[SimradDgClock alloc] initWithSimradDg:self data:data dgStart:dgStart size:size];
			break;
		case SIMRAD_DG_POSITIONS:
			NSLog(@"Trying to create a POS");
			self = [[SimradDgPos alloc] initWithSimradDg:self data:data dgStart:dgStart size:size];
			break;

		default:
			// NOP - Leave it a generic/unknown datagram
			break;
	}
	if (self != self_old) [self_old release];
    return self;
}

@end // SimradDg
