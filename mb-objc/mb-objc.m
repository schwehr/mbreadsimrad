#import <Foundation/Foundation.h>

#import "simrad.h"

int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	NSString *fileName = @"/Users/schwehr/projects/src/mbreadsimrad/0018_20050728_153458_Heron.all";
	NSData *data = [NSData dataWithContentsOfFile:fileName];
	if (!data) {
		NSLog(@"ERROR: Unable to open file: %@",fileName);
		exit(EXIT_FAILURE);
	}

	unsigned int size;
#if 0
	size_t dgStart = 0;
	[data getBytes:&size range:NSMakeRange(dgStart, 4)];

	SimradDg *dg = [[SimradDg alloc] initWithData:data dgStart:4 size:size];
	NSLog(@"Datagram: %@",dg);
#endif

	int dgCount;
	size_t dgStart;
	for (dgStart = 0, dgCount=0; dgStart < data.length; dgCount++) {
		if (dgCount > 30) {
			NSLog(@"Early exit");
			break;
		}
		[data getBytes:&size range:NSMakeRange(dgStart, 4)];
		SimradDg *dg = [[SimradDg alloc] initWithData:data dgStart:dgStart+4 size:size];
		if (dg.dgId != SIMRAD_DG_CLOCK) {
			NSLog(@"Datagram: %@",dg);
		}

		
		dgStart += size+4;
	}
	NSLog(@"Done.  Read num datagrams: %d",dgCount);
    [pool drain];
    return 0;
}
