#!/usr/bin/env python2.7
from __future__ import print_function

# Read MB datagrams from a simrad em122.  Starting with Healy 2010 data.

import mmap   # load the file into memory directly so it looks like a big array
import os
import struct

datagram_type_lut = {
    'D': ('depth',),
    'X': ('XYZ',),
    'K': ('Central beams echogram',),
    'F': ('Raw range and beam angle datagrams (old)',),
    'f': ('Raw range and beam angle datagrams (new)'),
    'N': ('Raw range and beam angle 78 datagram'),
    'S': ('Seabed image datagram'),
    'Y': ('Seabed image data 89 datagram'),
    'k': ('Water column datagram'),
    'I': ('Installation parameters',),
    'i': ('installation parameters',), # same as I and r
    'r': ('remote information',), # same as I and i
    }

class Simrad(object):
    def __init__(self,filename):
        self.filename = filename
        tmp_file = open(filename, 'r+')
        self.size = os.path.getsize(filename)
        self.data = mmap.mmap(tmp_file.fileno(), self.size, access=mmap.ACCESS_READ)

class Datagram(object):
    def __init__(self, data, offset):
        'Data is a memory block with a data gram that starts at offset bytes into the block'
        self.data = data
        self.length = struct.unpack('I',data[offset:offset+4])[0]
        print ('length:',self.length)
        print ('STX:',ord(data[offset+4]))
        assert(2== ord(data[offset+4])) #struct.unpack('B',data[offset+4])
        self.dgram_type = data[offset+5]
        print ('dgram_type:',self.dgram_type,datagram_type_lut[self.dgram_type][0])

        for i in range(self.length-5,self.length+5):
            print (self.length,i,':',data[offset+i],ord(data[offset+i]))

        assert(3 == ord(data[offset+self.length+1]))
        self.checksum = struct.unpack('H',data[offset+self.length+2:offset+self.length+4])[0]
        print ('checksum:',self.checksum)
        map(ord,data[
        
simrad = Simrad('0034_20100604_005123_Healy.all')
dg = Datagram(simrad.data,0)

