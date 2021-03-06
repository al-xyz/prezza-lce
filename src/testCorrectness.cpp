#include <inttypes.h>
#include "lce.hpp"
#include "util.hpp"

/* Define which tests should be done */
#define testChar
#define testBlock
#define testLCE


/* This programs checks if the extraction of blocks and characters is working correctly.
 * It also checks if LCE queries return the correct value */





/* Enter the file you want to test on here */
const std::string TESTFILE = "../text/english"; 



using namespace std;
int main(int argc, char *argv[]) {
	
	
    /* Construct LCE data structure */
    cout << "Constructing LCE data stucture" << endl;
	lce::lceDataStructure dataLCE;
	lce::buildLCEDataStructure(&dataLCE, TESTFILE);
	cout << "Prime: "; util::printInt128(dataLCE.prime);
	
	/* Load raw Text */
	cout << "Loading raw text" << endl;
	ifstream input(TESTFILE, ios::in|ios::binary);
	util::inputErrorHandling(&input);
	uint64_t dataSize = util::calculateSizeOfInputFile(&input);
    char * dataRaw = new char[dataSize];
    input.seekg(0);
    input.read(dataRaw, dataSize);
	
	cout << "Size of data: " << dataLCE.numberOfBytes << " Bytes" << endl;
	cout << "Size of data: " << dataLCE.numberOfBlocks << " Blocks" << endl;
	
	
    
#ifdef testChar
	/* Test if every character can be extracted correctly */
	cout << "Testing if every character is extracted correctly" << endl;
	for(uint64_t i = 0; i < dataLCE.numberOfBytes; i++) {
		
		if(dataRaw[i] != lce::getChar(&dataLCE, i)) {
			cout << "Wrong Char: Error at position " << i << endl;
			cout << (unsigned char) dataRaw[i] << " is not " << lce::getChar(&dataLCE, i) << endl;
		}
	}
#endif



#ifdef testBlock
	uint64_t paquet = *(uint64_t*)"\x1\x0\x0\x0\x0\x0\x0\x0"; /* To check if small endian is used */
	/* Test if every block can be extracted correctly */
	cout << "Testing if every block is extracted correctly" << endl;
	uint64_t * blockRaw = (uint64_t*) dataRaw;

        
	for(uint64_t i = 0; i < dataLCE.numberOfBlocks; i++) {
		
		uint64_t block = blockRaw[i];
		
		/* If small endian, reverse the character bytes, so it matches the endianess of uint64_t */
		if(paquet == 1){
		//reverse
			char * cArray = (char*) &block;
			char *f=&cArray[0], *b=&cArray[7];
			while(f<b){
				char tmp = *f;
				*f++ = *b;
				*b-- = tmp;
			}
		}
		
		
		if(i == dataLCE.numberOfBlocks - 1) {
			if(block != lce::getBlock(&dataLCE, i)) {
				cout << "Wrong Block: Error at position " << i << endl;
				cout << "Raw Block: "; util::printInt64(blockRaw[i] << ((dataLCE.numberOfBytes%8)*8) >> ((dataLCE.numberOfBytes%8)*8));
				cout << "Block Fingerprint: "; util::printInt64(lce::getBlock(&dataLCE, i));
			}
		}
	}
#endif
	
	

#ifdef testLCE
	/* Test if LCE Queries are correct */
	cout << "Testing if LCE queries are correct" << endl;
	uint64_t lceRaw = 0;
	uint64_t lceFast = 0;
	
	//uint64_t indexI = 102;
	//uint64_t indexJ = 247;

	for(uint64_t i = 0; i < 2000000; i++) { 
		uint64_t indexI = util::randomIndex(dataLCE.numberOfBytes);
		uint64_t indexJ = util::randomIndex(dataLCE.numberOfBytes);
		lceRaw = lce::naivelce(dataRaw, dataSize, indexI, indexJ);
		lceFast = lce::fastlce(&dataLCE, indexI, indexJ);
		
		if(lceRaw != lceFast) {
			cout << "Wrong LCE: Error at position " << indexI << " and " << indexJ << endl;
			cout << "Block at position " << (indexI/8)-2 << ": "; util::printInt64(lce::getBlock(&dataLCE, (indexI/8)-2));
			cout << "Block at position " << (indexI/8)-1 << ": "; util::printInt64(lce::getBlock(&dataLCE, (indexI/8)-1));
			cout << "Block at position " << indexI/8 << ": "; util::printInt64(lce::getBlock(&dataLCE, indexI/8));
			cout << "Block at position " << (indexJ/8)-2 << ": "; util::printInt64(lce::getBlock(&dataLCE, (indexJ/8)-2));
			cout << "Block at position " << (indexJ/8)-1 << ": "; util::printInt64(lce::getBlock(&dataLCE, (indexJ/8)-1));
			cout << "Block at position " << indexJ/8 << ": "; util::printInt64(lce::getBlock(&dataLCE, indexJ/8));

			cout << "Not matching characters: " << lce::getChar(&dataLCE, indexI) << " " << lce::getChar(&dataLCE, indexJ) << endl;

			cout << "Fingerprint at position " << (indexI/8)-2 << ": ";util::printInt64(dataLCE.fingerprints[(indexI/8)-2]); 
			cout << "Fingerprint at position " << (indexI/8)-1 << ": ";util::printInt64(dataLCE.fingerprints[(indexI/8)-1]); 
			cout << "Fingerprint at position " << indexI/8 << ": "; util::printInt64(dataLCE.fingerprints[indexI/8]);
			cout << "Fingerprint at position " << (indexJ/8)-2 << ": ";util::printInt64(dataLCE.fingerprints[(indexJ/8)-2]); 
			cout << "Fingerprint at position " << (indexJ/8)-1 << ": ";util::printInt64(dataLCE.fingerprints[(indexJ/8)-1]); 
			cout << "Fingerprint at position " << indexJ/8 << ": "; util::printInt64(dataLCE.fingerprints[indexJ/8]);
			cout << "" << endl;
			
			cout << "lceRaw:  " << lceRaw << endl;
			cout << "lceFast: " << lceFast << endl;
		}
	}
	
#endif
	
	cout << "Test ended" << endl;
	exit(EXIT_SUCCESS);
}
