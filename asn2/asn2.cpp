/* Kurt Schwehr - Aug 2010 - BSD License
   MB Nuts and Bolts course at CCOM, UNH
   Assignment 2: Read the clock messages
   g++ -g -Wall -o asn2-cpp asn2.cpp  && ../0018_20050728_153458_Heron.all

   SEE ALSO:
     http://www.ldeo.columbia.edu/res/pi/MB-System/formatdoc/
     http://www.ldeo.columbia.edu/res/pi/MB-System/formatdoc/EM_Datagram_Formats_RevM.pdf
         Page 68, 3.3.3 Clock
     http://rabbit.eng.miami.edu/info/functions/time.html
     man ascii
     man open
     man mmap
     man 
     man 
     man 
*/

#include <iostream>
#include <cstdio>
#include <vector>
#include <cassert>

// For file size
#include <sys/types.h> 
#include <sys/stat.h>

// mmap to read the data
#include <fcntl.h> // open
#include <errno.h>
#include <sys/mman.h>

using namespace std;

const unsigned char STX = 2; // Start-of-transmission
const unsigned char ETX = 3; // End-of-transmission
const unsigned int DG_ID_CLOCK = 67;

// FIX: make these inline function
#define GET_U4(start,offset) ((unsigned int) *((unsigned int *) (start+offset)))
#define GET_U2(start,offset) ((unsigned short) *((unsigned short *) (start+offset)))
#define GET_U1(start,offset) ((unsigned char) *((unsigned char *) (start+offset)))


unsigned short compute_checksum(const unsigned char *dg_data, const size_t size);

unsigned short compute_checksum(const unsigned char *dg_data, const size_t size) {
    // Checksum of bytes between the STX and ETX characters
    assert(dg_data);
    assert(size < 128000); // Assumption of size
    unsigned short sum = 0;
    for (size_t i=5; i<4+size-3; i++) 
        sum += GET_U1(dg_data,i);
    return sum;
}

void em_time2tm_struct (const unsigned int date, const unsigned int millisec, struct tm &t);

// Convert a date_raw and millisec_raw to a time.h / struct tm
void em_time2tm_struct (const unsigned int date,
                        const unsigned int millisec, 
                        struct tm &t) {
    t.tm_year = date / 10000;
    t.tm_mon = (date % 10000) / 100;
    t.tm_mday = date % 100;

    t.tm_hour = millisec / 360000;
    t.tm_min = (millisec % 360000) / 6000 ;
    t.tm_sec = (millisec % 360000) % 6000 ;
    t.tm_gmtoff = 0;
}

class SimradDgClock {
public:
    int size;
    // Header
    unsigned char stx;
    unsigned char id;
    unsigned short em_model;
    unsigned int date_raw;
    unsigned int millisec_raw;
    unsigned short clock_counter;
    unsigned short serial_num;
    // Actual payload
    unsigned int date_sensor_raw;
    unsigned int millisec_sensor_raw;
    unsigned char pps_raw;
    // Footer
    unsigned char etx;
    unsigned short checksum;
    SimradDgClock(const unsigned char *dg_data);  // pointer starting at size

    // Derived values
    //struct tm timestamp, timestamp_sensor;

    struct tm getTm(); // return struct tm defined in time.h
    void print_raw();
    void print_time();
    //protected:
};

SimradDgClock::SimradDgClock(const unsigned char *data) {
    size = GET_U4(data,0);
    stx = GET_U1(data,4);
    id = GET_U1(data,5);
    em_model = GET_U2(data,6);
    date_raw = GET_U4(data,8);
    millisec_raw = GET_U4(data,12);
    clock_counter = GET_U2(data,16);
    serial_num = GET_U2(data,18);
    date_sensor_raw = GET_U4(data,20);
    millisec_sensor_raw = GET_U4(data,24);
    pps_raw = GET_U1(data,28);
    etx = GET_U1(data,29);
    checksum = GET_U2(data,30);

    
}

void
SimradDgClock::print_raw() {
    cout << "SimradDgClock: "
         << "size: \t" << size << "\n"
         << "\tem_model:" << em_model << "\n"
         << "\tdate_raw: " << date_raw << "\n"
         << "\tmillisec_raw: " << millisec_raw << "\n"
         << "\tclock_counter: " << clock_counter << "\n"
         << "\tserial_num: " << serial_num << "\n"
         << "\tdate_sensor_raw: " << date_sensor_raw << "\n"
         << "\tmillisec_sensor_raw: " << millisec_sensor_raw << "\n"
         << "\tdate_sensor_raw" << date_sensor_raw << "\n"
         << "\tetx: " << int(etx) << "\n"
         << "\tchecksum: " << checksum << "\n"
         << endl;
}

void
SimradDgClock::print_time() {
    tm time, time_sensor;
    em_time2tm_struct(date_raw, millisec_raw,time);
    em_time2tm_struct(date_sensor_raw, millisec_sensor_raw, time_sensor);
    //cout << asctime(&time) << " ---- ";
    cout << asctime(&time_sensor) << endl;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "ERROR: must specify an input file";
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];

    FILE *infile = fopen(filename, "rb"); // b needed for windows machines
    if (!infile) {
        cerr << "ERROR: unable to open input file: " << filename << endl;
        perror("\tSpecific error");
        exit(EXIT_FAILURE);
    }

    struct stat sb;
    int r = stat (filename, &sb);
    if (0 != r) {perror("stat to get file size FAILED"); exit(EXIT_FAILURE);}
    const size_t file_size = sb.st_size;

    int fd = open (filename, O_RDONLY, 0);
    if (-1 == fd) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }
    cout << "fd: " << fd << " size:" << file_size << " " << PROT_READ << " " << MAP_FILE << endl;
    const unsigned char *data = (unsigned char *)mmap (0, file_size, PROT_READ,  MAP_FILE | MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == data) {
        perror("mmap failed for em raw file");
        exit(EXIT_FAILURE);
    }

    vector<SimradDgClock> clocks;

    size_t cur_pos = 0;
    while (cur_pos < file_size) {
        //datagram_count ++;
        const unsigned char *dg_data = data + cur_pos;  // The start of the current datagram
        const unsigned int size = (unsigned int) *((unsigned int *) (dg_data));
        const unsigned char stx=GET_U1(dg_data,4), id=GET_U1(dg_data,5), etx=GET_U1(dg_data,4+size-3);
        const unsigned short checksum = GET_U2(dg_data, 4+size-2);
        {
            unsigned short sum = id; // ID byte is included in the checksum, but read separately
            // Checksum of bytes between the STX and ETX characters
            for (size_t i=6; i<4+size-3; i++) {
                sum += GET_U1(dg_data,i);
            }
            if (checksum != sum) {
                cerr << "bad checksum: " << checksum << " != " << sum << endl;
                exit(EXIT_FAILURE);
                continue;
            }
            //cout << "double check:" << checksum << " " << compute_checksum(dg_data,size) << endl;;
        }

        if (DG_ID_CLOCK == id) {
            cout << "clock" << endl;
            SimradDgClock clock(dg_data);
            clock.print_raw();
            clock.print_time();
            cerr << "after" << endl;
            //clocks.push_back(SimradDgClock(dg_data));
            //cout << "early" << endl;
            //exit(1);
        }

        //cout << "datagram: size="<<size<<" id="<<int(id)<<" stx="<<int(stx)<<" etx="<<int(etx)<<" checksum="<<checksum<<  endl;
        assert(STX==stx);
        assert(ETX==etx);

        cur_pos += size + 4;
    } // while
}
