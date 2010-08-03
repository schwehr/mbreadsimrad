/* Kurt Schwehr - Aug 2010 - BSD License
   MB Nuts and Bolts course at CCOM, UNH
   Assignment 1: read basic datagram statistics from an EM3002
   g++ -g -Wall -o asn1-cpp asn1.cpp  && ../0018_20050728_153458_Heron.all

   SEE ALSO:
     http://www.ldeo.columbia.edu/res/pi/MB-System/formatdoc/
     http://www.ldeo.columbia.edu/res/pi/MB-System/formatdoc/EM_Datagram_Formats_RevM.pdf
     man ascii
     man fread
     man fseek
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

using namespace std;

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
    "69 Single beam echo sounder depth ",
    "70 Raw range and beam angles (old)",
    "71 Surface sound speed",
    "72 Headings",
    "73 Installation parameters",
    "74 Mechanical transducer tilts",
    "75 Central beams echogram",
    "76 unknown",    "77 unknown",
    "78 Raw range and beam angle 78 ",
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
    "110 Network attitude velocity 110",
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


void print_summary (const vector<int> &id_count);

void print_summary (const vector<int> &id_count) {
    cout << "\nDATAGRAM SUMMARY:" << endl;
    int datagram_count = 0;
    for (int i=0; i<256; i++) {
        datagram_count += id_count[i];
        if (id_count[i]>0) {
            cout << /*i << ": " <<*/ id_count[i] << "  \t" << datagram_names[i] << endl;
        }
    }
    cout << "total datagrams: " << datagram_count << endl;
}
int main(int argc, char *argv[]) {
#if 0
    // Make sure you understand the size of C++ integer types
    cout << ((sizeof(long) == 4)?"32":"64") << " bit mode" << endl;
    cout << "sizeof(char)" << sizeof(char) << endl;
    cout << "sizeof(short)" << sizeof(short) << endl;
    cout << "sizeof(int)" << sizeof(int) << endl;
    cout << "sizeof(long)" << sizeof(long) << endl;
    cout << "sizeof(long long)" << sizeof(long long) << endl;
#endif

    assert(sizeof(char) == 1); // If not, our world will fall apart.
    assert(sizeof(short) == 2); // If not, our world will fall apart.
    assert(sizeof(int) == 4); // If not, our world will fall apart.

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

    size_t num_read;
    unsigned int size;

    const unsigned char STX = 2; // Start-of-transmission
    unsigned char stx;
    const unsigned char ETX = 3; // End-of-transmission
    unsigned char etx;

    unsigned char id;

    const size_t MAX_PAYLOAD = 1024*(32+1); // Needs to handle the MAX UDP size.  I think that is 32K
    unsigned char buf[MAX_PAYLOAD];  // A huge buf

    unsigned short checksum;

    vector<int> id_count(256,0);

    int datagram_count = 0;
    while (true) {//0 < file_size - ftell(infile)) {
        datagram_count ++;
        //cout << endl;
        //cout << "  what? "<< ftell(infile) << " ... " << file_size - ftell(infile) << " feof: " << feof(infile) << endl;
        num_read = fread(&size, 4, 1, infile);
        //cout << num_read << " size: " << size << endl;
        if (!num_read and feof(infile)) {
            cout << "found end of file" << endl;
            break;
        }
        assert(1 == num_read);
        assert(MAX_PAYLOAD >= size); // Watch for buffer overflow


        num_read = fread(&stx, 1, 1, infile);
        //cout << num_read << " stx: decimal: " << int(stx) << " hex: " << std::hex << int(stx) << std::dec << endl;
        assert(1 == num_read);
        assert(STX == stx);

        num_read = fread(&id, 1, 1, infile);
        //cout << num_read << " id: " << id << " decimal: " << int(id) << " hex: " << std::hex << int(id) << std::dec<< endl;
        assert(1 == num_read);

        // size == 1 for STX, 1 for id, number of bytes in the payload, 1 for ETX, 2 for checksum
        const size_t payload_size = size-5;
        num_read = fread(&buf, payload_size, 1, infile); // Pull in the data payload from the datagram
        assert(1 == num_read);

        /*
        if (0 != fseek(infile, size, SEEK_CUR)) {
            perror("Unable to seek past the data");
        }
        */

        num_read = fread(&etx, 1, 1, infile);
        //cout << num_read << " etx: decimal: " << int(etx) << " hex: " << std::hex << int(etx) << std::dec << endl;
        assert(1 == num_read);
        assert(ETX == etx);

        num_read = fread(&checksum, 2, 1, infile);
        //cout << num_read << " checksum: " << checksum << endl;
        assert(1 == num_read);

        // Verify the checksum
        {
            unsigned short sum = id; // ID byte is included in the checksum, but read separately
            for (size_t i=0; i<payload_size; i++) {
                sum += buf[i];
            }
            //cout << " checksum: " << checksum << " computed: " << sum << endl;
            assert (checksum == sum);
        }

        //cout << int(id) << " " << id_count[int(id)] << " -> ";
        id_count[int(id)] ++;
        //cout << id_count[int(id)] << endl;

        //cerr << "EARLY" << endl;
        //exit(EXIT_FAILURE);
    }

    cout << "datagram count = " << datagram_count << endl;
    print_summary (id_count);
}
