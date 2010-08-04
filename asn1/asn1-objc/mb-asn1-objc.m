#import <Foundation/Foundation.h>

int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	NSDictionary *dgNames = [NSDictionary dictionaryWithObjectsAndKeys:
							 //@"", [NSNumber numberWithInt:],
							 @"PU Id outputs", 	[NSNumber numberWithInt:48],
							 @"PU Status output", 	[NSNumber numberWithInt:49],
							 @"Extra Parameters", 	[NSNumber numberWithInt:51],
							 @"Attitude ", 	[NSNumber numberWithInt:65],
							 @"PU BIST result", 	[NSNumber numberWithInt:66],
							 @"Clock", 	[NSNumber numberWithInt:67],
							 @"depth", 	[NSNumber numberWithInt:68],
							 @"Single beam echo sounder depth ", 	[NSNumber numberWithInt:69],
							 @"Raw range and beam angles (old)", 	[NSNumber numberWithInt:70],
							 @"Surface sound speed", 	[NSNumber numberWithInt:71],
							 @"Headings", 	[NSNumber numberWithInt:72],
							 @"Installation parameters", 	[NSNumber numberWithInt:73],
							 @"Mechanical transducer tilts", 	[NSNumber numberWithInt:74],
							 @"Central beams echogram", 	[NSNumber numberWithInt:75],
							 @"Raw range and beam angle 78 ", 	[NSNumber numberWithInt:78],
							 @"Positions", 	[NSNumber numberWithInt:80],
							 @"Runtime parameters", 	[NSNumber numberWithInt:82],
							 @"Seabed image ", 	[NSNumber numberWithInt:83],
							 @"Tide", 	[NSNumber numberWithInt:84],
							 @"Sound speed profile", 	[NSNumber numberWithInt:85],
							 @"Kongsberg Maritime SSP output", 	[NSNumber numberWithInt:87],
							 @"XYZ", 	[NSNumber numberWithInt:88],
							 @"Seabed image data 89 ", 	[NSNumber numberWithInt:89],
							 @"Raw range and beam angles (new)", 	[NSNumber numberWithInt:102],
							 @"Depth (pressure) or height ", 	[NSNumber numberWithInt:104],
							 @"installation parameters", 	[NSNumber numberWithInt:105],
							 @"Water column ", 	[NSNumber numberWithInt:107],
							 @"Network attitude velocity  110", 	[NSNumber numberWithInt:110],
							 @"remote information", 	[NSNumber numberWithInt:114],
							 nil
							 ];
	NSMutableArray *count_id = [NSMutableArray new];
	for (int i; i < 256; i++) {
		[count_id addObject:[NSNumber numberWithInt:0]];
	}
							 
	NSString *fileName = @"/Users/schwehr/projects/src/mbreadsimrad/0018_20050728_153458_Heron.all";
	// Warning: this uses mmap that will crash if the file is on removable media or share that is disconnected.
	NSData *data = [NSData dataWithContentsOfFile:fileName];
	if (!data) {
		NSLog(@"ERROR: Unable to open file: %@",fileName);
		exit(EXIT_FAILURE);
	}
	
	//size_t dgStart = 0;
	//const size_t MAX_PAYLOAD = 1024 * (32+1);
	//unsigned char buf[MAX_PAYLOAD];
	NSLog(@"data.length: %d", [data length]);
	
	for (size_t dgStart = 0, dgCount=0; dgStart < data.length; dgCount++) {
		if (dgCount%1000 == 0) NSLog(@"count: %d",dgCount);
		unsigned char id, stx, etx;
		unsigned short checksum;
		unsigned int size;
		[data getBytes:&size range:NSMakeRange(dgStart, 4)];
		[data getBytes:&stx range:NSMakeRange(dgStart+4, 2)];
		[data getBytes:&id range:NSMakeRange(dgStart+5, 1)];
		[data getBytes:&etx range:NSMakeRange(dgStart+4+size-3, 1)];
		[data getBytes:&checksum range:NSMakeRange(dgStart+4+size-2, 2)];

		// Yuck!
		[count_id replaceObjectAtIndex:id withObject: [NSNumber numberWithInt:[[count_id objectAtIndex:id] integerValue]+1 ] ];
		
#if 0
		if (dgCount%100==0) {
			NSLog(@"Datagram %d : size=%d, stx=%d %x, id=%d %x, etx=%d %x, checksum=%d %x",
				  dgCount, size, stx, stx, id, id, etx, etx, checksum, checksum); 
			//NSLog(@"  name: %@", [dgNames objectForKey:[NSNumber numberWithInt:id]]);
		}
#endif
		dgStart += size+4;
	}

	for (int i; i < 256; i++) {
		int count = [[count_id objectAtIndex:i] integerValue];
		if (count > 0) {
			NSLog(@"%d \t %@",count, [dgNames objectForKey:[NSNumber numberWithInt:i]]);
		}
	}
	
    // insert code here...
    NSLog(@"Hello, World!");
    [pool drain];
    return 0;
}
