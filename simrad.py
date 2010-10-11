#!/usr/bin/env python2.7
from __future__ import print_function

# License: BSD, Kurt Schwehr Aug/Sep 2010

# Requires python 2.6 or 2.7
# Read MB datagrams from a simrad multibeam.  Starting with 2010 Healy em122 data.

import mmap   # load the file into memory directly so it looks like a big array
import struct # For decoding the fields within a datagram
import os, sys
import datetime

STX = 2
ETX = 3

datagram_names = {
    0x30: 'PU Id outputs',		# char: '0'	dec:  48
    0x31: 'PU Status output',		# char: '1'	dec:  49
    0x33: 'Extra Parameters',		# char: '3'	dec:  51
    0x41: 'Attitude',			# char: 'A'	dec:  65
    0x42: 'PU BIST result',		# char: 'B'	dec:  66
    0x43: 'Clock',			# char: 'C'	dec:  67
    0x44: 'depth',			# char: 'D'	dec:  68
    0x45: 'Single beam depth',		# char: 'E'	dec:  69
    0x46: 'Range and angles (old)',	# char: 'F'	dec:  70
    0x47: 'Surface sound speed',	# char: 'G'	dec:  71
    0x48: 'Headings',			# char: 'H'	dec:  72
    0x49: 'Installation params',	# char: 'I'	dec:  73
    0x4a: 'Mech transducer tilts',	# char: 'J'	dec:  74
    0x4b: 'Central beams echogram',	# char: 'K'	dec:  75
    0x4e: 'Range and angle 78',		# char: 'N'	dec:  78
    0x50: 'Positions',			# char: 'P'	dec:  80
    0x52: 'Runtime parameters',		# char: 'R'	dec:  82
    0x53: 'Seabed image',		# char: 'S'	dec:  83
    0x54: 'Tide',			# char: 'T'	dec:  84
    0x55: 'Sound speed profile',	# char: 'U'	dec:  85
    0x57: 'SSP output',			# char: 'W'	dec:  87
    0x58: 'XYZ',			# char: 'X'	dec:  88
    0x59: 'Seabed image data 89',	# char: 'Y'	dec:  89
    0x66: 'Range and angles (new)',	# char: 'f'	dec:  102
    0x68: 'Depth or height',		# char: 'h'	dec:  104
    0x69: 'installation params',	# char: 'i'	dec:  105
    0x6b: 'Water column',		# char: 'k'	dec:  107
    0x6e: 'Network attitude velocity',	# char: 'n'	dec:  110
    0x72: 'remote information',		# char: 'r'	dec:  114
}


class SimradError(Exception):
    pass

class SimradErrorBadChecksum(SimradError):
    pass

def date_and_time_to_datetime(date_raw,ms_raw):
    year = date_raw / 10000
    month = (date_raw % 10000) / 100
    day = date_raw % 100

    millisec = ms_raw % 1000
    microsec = millisec * 1000

    second = (ms_raw / 1000) % 60
    minute = (ms_raw / 60000) % 60
    hour = (ms_raw / 3600000) % 24

    return datetime.datetime(year, month, day, hour, minute, second, microsec)


class Datagram(object):
    def __init__(self, data, offset, size):
        '''offset: points to the start of the data (STX) right after the size field in a file
        size: length of the datagram from STX through to and including the checksum
        '''
        #assert (STX == ord(data[offset]))
        #assert (ETX == ord(data[offset+size-3]))
        self.dg_id = ord(data[offset+1])
        #checksum_reported = struct.unpack('H',data[offset+size-2:offset+size])[0]
        # Checksum is of the data between STX and ETX
        #checksum_computed = sum(map(ord,data[offset+1:offset+size-3])) % (256*256)
        #assert (checksum_reported == checksum_computed)

        # This header is in every datagram
        self.model = struct.unpack('H',data[offset+2:offset+4])[0]
        date_raw, ms_raw, self.counter = struct.unpack('IIH',data[offset+4:offset+14])
        self.timestamp = date_and_time_to_datetime(date_raw,ms_raw)
        self.serial  = struct.unpack('H',data[offset+14:offset+16])[0]
        return

    def __str__(self): return self.__unicode__()
    def __unicode__(self):
        try:
            name = datagram_names[self.dg_id]
        except:
            name = 'unknown_%d' % self.dg_id
        return 'Unhandled datagram {dg_id} {dg_id_hex}: {name} {model} {timestamp} {counter} {serial}'.format(
            name=name, dg_id_hex = hex(self.dg_id), **self.__dict__)
    
class Clock(Datagram):
    def __init__(self, data, offset, size):
        # Python struct can't handle non-aligned reads it seems
        Datagram.__init__(self, data, offset, size)
        date_raw,ms_raw,pps = struct.unpack('IIB',data[offset+16:offset+16+9])
        self.timestamp_external = date_and_time_to_datetime(date_raw, ms_raw)
        self.pps = bool(pps)
        return

    def __str__(self): return self.__unicode__()
    def __unicode__(self):
        return 'Clock: {timestamp} {timestamp_external} {pps}'.format(**self.__dict__)

class Position(Datagram):
    def __init__(self, data, offset, size):
        Datagram.__init__(self, data, offset, size)
        lat_raw, lon_raw = struct.unpack('ii',data[offset+16:offset+24])
        self.y = lat_raw / 2e7
        self.x = lon_raw / 1e7
        self.fix_qual, self.sog_cms, cog, heading, self.pos_descriptor, byte_count = struct.unpack('HHHHBB',data[offset+24:offset+34])
        self.cog = cog * 1e-2
        self.heading = heading * 1e-2
        self.input_str = data[offset+34:offset+34+byte_count]
        
    def __str__(self): return self.__unicode__()
    def __unicode__(self):
        return 'Pos: {x} {y} {fix_qual} {sog_cms} {cog} {heading}'.format(**self.__dict__)

class RuntimeParam(Datagram):
    def __init__(self, data, offset, size):
        Datagram.__init__(self, data, offset, size)

        # FIX: not verified
        self.operator_status = data[offset+16]
        self.proc_status = data[offset+17]
        self.bsp_status = data[offset+18]
        self.sonar_head_status = data[offset+19]
        self.mode = data[offset+20]
        self.filter_id = data[offset+21]
        
        self.depth_min = struct.unpack('H',data[offset+22:offset+24])[0]
        self.depth_max = struct.unpack('H',data[offset+24:offset+26])[0]

        self.absorp_coeff = struct.unpack('H',data[offset+26:offset+28])[0]
        self.tx_pulse_len = struct.unpack('H',data[offset+28:offset+30])[0]
        self.tx_beamwidth_deg = struct.unpack('H',data[offset+30:offset+32])[0] / 10.
        self.tx_power_re_max = struct.unpack('h',data[offset+32:offset+34])[0]

        self.rx_beamwidth_deg = ord(data[offset+32]) / 10.
        self.rx_bandwidth_hz = ord(data[offset+33]) * 50

        self.mode2_or_fixed_gain_db = ord(data[offset+34])
        self.tvg_crossover_deg = ord(data[offset+35])
        self.source_of_ss_at_transducer = ord(data[offset+36])
        self.max_port_swath_m = struct.unpack('H',data[offset+37:offset+39])[0]
        self.beam_spacing = ord(data[offset+39])
        self.max_port_cov_deg = struct.unpack('H',data[offset+37:offset+39])[0]

        self.yaw_pitch_stablization_mode = ord(data[39])

        self.max_port_swath_m = struct.unpack('H',data[offset+40:offset+42])[0]
        self.max_port_cov_deg = struct.unpack('H',data[offset+42:offset+44])[0]

        self.tx_tilt_def = struct.unpack('H',data[offset+44:offset+46])[0]
        self.filter_id2 = ord(data[offset+46])
        etx = ord(data[offset+47])
        if (etx!= ETX):
            print ('RuntimePara_ETX_FAILURE: need to fix datagram',etx,size,47)
            #sys.exit('bye')
        # FIX: if EM 1002, durotong SSD
        
    def __str__(self): return self.__unicode__()
    def __unicode__(self):
        return 'RuntimeParam: {depth_min} {depth_max}'.format(**self.__dict__)

class InstParam(Datagram):
    def __init__(self, data, offset, size):
        Datagram.__init__(self, data, offset, size)

        # FIX: not verified

        # This overwrites the normal header paramers
        self.survey_line_num = struct.unpack('H',data[offset+12:offset+14])[0]
        # system seria number here
        self.serial_sonar_head = struct.unpack('H',data[offset+18:offset+20])[0]
        params_str = data[offset+20:offset+size-3].rstrip(chr(0))
        #print ('params_str:', params_str)
        #self.__dict__.update(parse_param_str)
        self.parse_param_str(params_str)
        print ('InstParam:',self.__dict__)

    def parse_param_str(self,s):
        fields = s.rstrip(',').split(',')
        #r = dict()
        for field in fields:
            name,value = field.split('=')
            try:
                value = float(value)
            except:
                try:
                    value = float(int)
                except:
                    pass
            #print name, value, type(value), ' --- ',field
            self.__dict__[name] = value


    def __str__(self): return self.__unicode__()
    def __unicode__(self):
        return 'InstParam: '.format(**self.__dict__)


class Svp(Datagram):
    def __init__(self, data, offset, size):
        Datagram.__init__(self, data, offset, size)

        # FIX: not verified

        date_raw,ms_raw = struct.unpack('II',data[offset+16:offset+24])
        self.timestamp_svp = date_and_time_to_datetime(date_raw, ms_raw)
        num_entries = struct.unpack('H',data[offset+24:offset+26])[0]
        depth_res_cm = struct.unpack('H',data[offset+26:offset+28])[0]
        
        print (num_entries,depth_res_cm)

        entries = []
        for i in range(num_entries):
            depth,ss_dms = struct.unpack('II',data[offset+28+i*4:offset+28+8+i*4])
            print ('  svp_entry:',i,depth,ss_dms)
        assert (0==ord(data[offset+28+num_entries*4]))
        assert (ETX==ord(data[offset+28+num_entries*4+1]))
                

        sys.exit('Early SVP')
    def __str__(self): return self.__unicode__()
    def __unicode__(self):
        return 'SVP: '.format(**self.__dict__)


datagram_classes = {
    0x43: Clock,			# char: 'C'	dec:  67
    0x49: InstParam,			# char: 'I'	dec:  73
    0x50: Position,			# char: 'P'	dec:  80
    0x52: RuntimeParam,			# char: 'R'	dec:  82
    0x55: Svp,				# char: 'U'	dec:  85
}

class SimradFile(object):
    def __init__(self,filename):
        self.filename = filename
        tmp_file = open(filename, 'r+')
        self.size = os.path.getsize(filename)
        self.data = mmap.mmap(tmp_file.fileno(), self.size, access=mmap.ACCESS_READ)
    def __iter__(self):
        iter = SimradIterator(self)
        return iter

class SimradIterator(object):
    def __init__(self,simrad):
        self.data = simrad.data
        self.offset = 0
        self.file_size = simrad.size
    def __iter__(self):
        return self
    def next(self):
        if self.offset >= self.file_size:
            raise StopIteration
        dg_length = struct.unpack('I',self.data[self.offset:self.offset+4])[0]
        # Ignore STX
        dg_id = ord(self.data[self.offset+5])
        if dg_id in datagram_classes:
            # Factory to build classes
            dg = datagram_classes[dg_id](self.data, self.offset+4, dg_length)
        else:
            # Return a basically useless container...
            dg = Datagram(self.data, self.offset+4, dg_length) 
        self.offset += 4 + dg_length
        return dg

def loop_datagrams(simrad_file):
    for count, dg in enumerate(simrad_file):
        #if count > 200: break
        #print ( dg_id, '\t',dg_length, ':\t', datagram_names[dg_id])
        print (str(dg))
        pass

def shiptrack_kml(simrad,outfile):
    o = outfile
    o.write('''<?xml version="1.0" encoding="UTF-8"?>
<kml xmlns="http://www.opengis.net/kml/2.2" xmlns:gx="http://www.google.com/kml/ext/2.2" xmlns:kml="http://www.opengis.net/kml/2.2" xmlns:atom="http://www.w3.org/2005/Atom">
<Placemark>
	<name>line</name>
	<LineString>
		<coordinates>\n''')
    for count,dg in enumerate(simrad):
        #print (dg)
        if dg.dg_id != ord('P'): continue
        o.write('{x},{y}\n'.format(**dg.__dict__))

    o.write('''		</coordinates>
	</LineString>
</Placemark>
</kml>
''')


    
def main():
    simrad_file = SimradFile('0018_20050728_153458_Heron.all')
    #simrad_file = SimradFile('0034_20100604_005123_Healy.all')
    loop_datagrams(simrad_file)
    #shiptrack_kml(simrad_file,file('out.kml','w'))

if __name__ == '__main__':
    main()
