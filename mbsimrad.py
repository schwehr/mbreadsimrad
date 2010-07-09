#!/usr/bin/env python2.7
from __future__ import print_function
# Requires python 2.6 or 2.7
# Read MB datagrams from a simrad multibeam.  Starting with 2010 Healy em122 data.

import mmap   # load the file into memory directly so it looks like a big array
import struct # For decoding the fields within a datagram
import os, sys
import datetime

class SimradError(Exception):
    pass

def decode_date(date):
    year = date / 10000
    month = (date % 10000) / 100
    day = date % 100
    return year,month,day

def decode_time(time):
    hour = time / 360000
    millisec = time % 100
    microsec = millisec * 1000
    remainder = (time % 360000) / 100
    minute = remainder / 60
    second = remainder % 60
    return hour, minute, second, microsec # microsec for python datetime

def date_and_time_to_datetime(date,time):
    year, month, day = decode_date(date)
    hour, min, sec, microsec = decode_time(time)
    return datetime.datetime(year, month, day, hour, min, sec, microsec)

class Clock(object):
    def __init__(self,data,offset=0):
        self.model = struct.unpack('H',data[offset+6:offset+8])[0]
        date,time,self.counter = struct.unpack('IIH',data[offset+8:offset+18])
        self.timestamp = date_and_time_to_datetime(date,time)
        #self.serial,date,time,pps = struct.unpack('HIIB',data[offset+18:offset+18+11])
        # Python struct can't handle non-aligned reads it seems
        self.serial = struct.unpack('H',data[offset+18:offset+20])[0]
        date,time,pps = struct.unpack('IIB',data[offset+20:offset+20+9])
        self.timestamp_ext = date_and_time_to_datetime(date,time)
        self.pps = bool(pps)

    def __str__(self): return self.__unicode__()
    def __unicode__(self):
        #print (self.__dict__)
        return 'Clock: {timestamp} -- {model} {counter} {serial} {timestamp_ext} {pps}'.format(**self.__dict__)

datagram_type_lut = {
    'D': ('depth',None),
    'X': ('XYZ',None),
    'K': ('Central beams echogram',None),
    'F': ('Raw range and beam angles (old)',None),
    'f': ('Raw range and beam angles (new)',None),
    'N': ('Raw range and beam angle 78 ',None),
    'S': ('Seabed image ',None),
    'Y': ('Seabed image data 89 ',None),
    'k': ('Water column ',None),
    'I': ('Installation parameters',None),
    'i': ('installation parameters',None), # same as I and r
    'r': ('remote information',None), # same as I and i
    'R': ('Runtime parameters',None),
    'U': ('Sound speed profile ',None),
    'A': ('Attitude ',None),
    'n': ('Network attitude velocity  110',None),
    'C': ('Clock',Clock),
    'h': ('Depth (pressure) or height ',None),
    'H': ('Headings',None),
    'P': ('Positions',None),
    'E': ('Single beam echo sounder depth ',None),
    'T': ('Tide',None),
    'G': ('Surface sound speed',None),
    'U': ('Sound speed profile',None),
    'W': ('Kongsberg Maritime SSP output',None),
    'J': ('Mechanical transducer tilts',None),
    '3': ('Extra Parameters',None),
    '0': ('PU Id outputs',None),
    '1': ('PU Status output',None),
    'B': ('PU BIST result',None), # Built in self test
    }

class Simrad(object):
    def __init__(self,filename):
        self.filename = filename
        tmp_file = open(filename, 'r+')
        self.size = os.path.getsize(filename)
        self.data = mmap.mmap(tmp_file.fileno(), self.size, access=mmap.ACCESS_READ)

    def __iter__(self):
        iter = SimradIterator(self)
        return iter

class Datagram(object):
    def __init__(self, data, offset):
        'Data is a memory block with a data gram that starts at offset bytes into the block'
        self.data = data
        self.offset = offset
        self.length = struct.unpack('I',data[offset:offset+4])[0]
        #print ('length:',self.length)
        #print ('STX:',ord(data[offset+4]))
        assert(2== ord(data[offset+4])) #struct.unpack('B',data[offset+4])
        self.dgram_type = data[offset+5]
        #print ('dgram_type:',self.dgram_type,datagram_type_lut[self.dgram_type][0])

        #for i in range(self.length-5,self.length+5):
        #    print (self.length,i,':',data[offset+i],ord(data[offset+i]))

        assert(3 == ord(data[offset+self.length+1])) # End marker
        self.checksum_reported = struct.unpack('H',data[offset+self.length+2:offset+self.length+4])[0]
        self.checksum_actual = sum(map(ord,data[offset+5:offset+self.length]))
        #print ('checksum:',self.checksum_actual,self.checksum_reported)

    def __str__(self): return self.__unicode__()
    def __unicode__(self):
        return 'dg: {length} {dg_type}, {dg_name}'.format(
            length = self.length,
            dg_type = self.dgram_type,
            dg_name = datagram_type_lut[self.dgram_type][0],
            )

    def checksum_valid(self):
        return self.checksum_reported == self.checksum_actual

    def next(self):
        'return the offset value for the next datagram'
        return self.offset + self.length + 4

    def get_object(self):
        'Try to return a class instance for the datagram or None if not yet written'
        cls = datagram_type_lut[self.dgram_type][1]
        if cls is None: return None # Means not yet decoded
        return cls(self.data, self.offset)

class SimradIterator(object):
    def __init__(self,simrad):
        self.data = simrad.data
        self.offset = 0
        self.size = simrad.size
    def __iter__(self):
        return self
    def next(self):
        if self.offset > self.size:
            raise StopIteration
        dg = Datagram(self.data,self.offset)
        self.offset = dg.next()
        return dg

def main():
    simrad = Simrad('0034_20100604_005123_Healy.all')
    for count,dg in enumerate(simrad):
        #print (count, dg.length, dg.dgram_type, )
        obj = dg.get_object()
        if obj is None: continue
        print (count, str(dg), str(obj) )


if __name__ == '__main__':
    main()
