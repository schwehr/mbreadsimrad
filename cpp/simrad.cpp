// Kurt Schwehr - Aug 2010 - Apache 2.0 License

#include "simrad.h"

#include <cassert>
#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>

#include <iostream>
#include <vector>

using namespace std;

double emtime2unixtime_double(const unsigned int date,
                              const unsigned int millisec);

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
        return false;
    }
    t.tm_zone = "UTC";
    t.tm_gmtoff = 0;
    return true;
}

time_t emtime2unixtime(const unsigned int date, const unsigned int millisec) {
    struct tm t;
    if (! emtime2tm_struct (date, millisec, t)) {
        cerr << "Unable to convert time in emtime2unixtime" << endl;
        return 0;
    }
    return timegm(&t);
}

double emtime2unixtime_double(const unsigned int date, const unsigned int millisec) {
    int sec = emtime2unixtime(date, millisec);
    double sec_decimal = (millisec % 1000) / 1000.;
    return sec + sec_decimal;
}


//////////////////////////////////////////////////////////////////////
// SimradFile - Loop over a file
//////////////////////////////////////////////////////////////////////

SimradFile::SimradFile(const string &filename) : cur_offset(0) {
    // b needed for windows machines.
    FILE *infile = fopen(filename.c_str(), "rb");
    if (!infile) {
        cerr << "ERROR: unable to open input file: " << filename << endl;
        perror("\tSpecific error");
        exit(EXIT_FAILURE);
    }
    //     cur_offset = 0;
    struct stat sb;
    int r = stat (filename.c_str(), &sb);
    if (0 != r) {perror("stat to get file size FAILED"); exit(EXIT_FAILURE);}
    const size_t file_size = sb.st_size;

    int fd = open (filename.c_str(), O_RDONLY, 0);
    if (-1 == fd) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    data = (unsigned char *)mmap (
        0, file_size, PROT_READ,  MAP_FILE | MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == data) {
        perror("mmap failed for em raw file");
        exit(EXIT_FAILURE);
    }

    if (0 != close(fd)) {
        perror("unable to close the file after mmap");
    }
}

SimradFile::~SimradFile() {
    if (data) {
        if (0 != munmap(data,file_size)) {
            perror("unable to munmap file");
        }
        data = 0;
    }
}

unsigned short dg_checksum(const unsigned char *dg_data, const size_t size) {
    // dg_data assumes that the packet starts with *size*.  NOT network safe
    // Checksum of bytes between the STX and ETX characters
    assert(dg_data);
    assert(size < 128000); // Assumption of size
    unsigned short sum = 0;
    for (size_t i=5; i<4+size-3; i++)
        sum += GET_U1(dg_data,i);
    return sum;
}


SimradDg *
SimradFile::next(const SimradDgEnum dg_type) {
    assert(dg_type == SIMRAD_DG_ANY); // FIX: allow this to change

    const unsigned char *dg_data = data + cur_offset;
    const unsigned int size = GET_U4(dg_data,0);
    const unsigned char stx = GET_U1(dg_data,4);
    const unsigned char id  = GET_U1(dg_data,5);
    const unsigned char etx = GET_U1(dg_data,4+size-3);
    const unsigned short checksum = GET_U2(dg_data, 4+size-2);
    if (stx != STX || etx != ETX || checksum != dg_checksum(dg_data,size)) {
        cerr << "Bad datagram.  Aborting." << endl;
        return 0;  // FIX: try to scan for the next uncorrupted packet
    }

    SimradDg *dg=0;
    switch (id) {
    case SIMRAD_DG_CLOCK:
        // Skip the size so that network packets will work later.
        dg = new SimradDgClock(dg_data+4, size);
        break;
    default:
        dg = new SimradDgUnknown(dg_data+4, size);
    }



    cur_offset += size + 4;
    return dg;
}

//////////////////////////////////////////////////////////////////////
// SimradDg
//////////////////////////////////////////////////////////////////////
#if 0
SimradDg::SimradDg(const unsigned char *data, const unsigned int size) {
    //const size_t size = GET_U4(data,0);
    assert(data);
    assert(size>0 && size < 1000000);
    assert(false);
}
#endif

SimradDgEnum
SimradDg::init(const unsigned char *data, const unsigned int size, bool network) {
    if (network) {
        // network packet
        assert(false); // Implement this
    }
    assert(STX==GET_U1(data,0));
    assert(ETX==GET_U1(data,size-3));

    em_model = GET_U2(data,2);

    const unsigned int date_raw = GET_U4(data,4);
    const unsigned int ms_raw = GET_U4(data,8);
    timestamp = emtime2unixtime_double(date_raw, ms_raw);
    //cout << "timestamp: " << timestamp << endl;

    ping_counter = GET_U2(data,12);
    serial_num = GET_U2(data,14);

    // Then comes the packet specific data payload

    return((SimradDgEnum)GET_U1(data,1)); // Return the id
}

SimradDg::~SimradDg() {
    //cout << "~SimradDg()" << endl;
    // NOP
}

//////////////////////////////////////////////////////////////////////
// SimradDgUnknown - errr... for things not yet parsed
//////////////////////////////////////////////////////////////////////

SimradDgUnknown::SimradDgUnknown(const unsigned char *data, const unsigned int size) {
    assert(data);
    assert(0<size && size<1000000);
    //id = GET_U1(data,1);
    id = init(data,size);
}
SimradDgUnknown::~SimradDgUnknown() {
    // nop
}

//////////////////////////////////////////////////////////////////////
// SimradDgClock
//////////////////////////////////////////////////////////////////////

SimradDgClock::SimradDgClock(const unsigned char *data, const unsigned int size) {
    assert(data);
    assert(0<size && size<1000000);
    assert(STX == GET_U1(data,0));
    assert(SIMRAD_DG_CLOCK == GET_U1(data,1)); // validate id
    init(data,size);

    // There is a 2nd timestamp in the clock packet
    const unsigned int date = GET_U4(data,16);
    const unsigned int ms   = GET_U4(data,20);
    timestamp_sensor = emtime2unixtime_double(date, ms);
    pps = bool(GET_U1(data,24));

}

SimradDgClock::~SimradDgClock() {
    // NOP
}

