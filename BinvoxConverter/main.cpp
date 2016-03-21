//
// This example program reads a .binvox file and writes
// an ASCII version of the same file called "voxels.txt"
//
// 0 = empty voxel
// 1 = filled voxel
// A newline is output after every "dim" voxels (depth = height = width = dim)
//
// Note that this ASCII version is not supported by "viewvox" and "thinvox"
//
// The x-axis is the most significant axis, then the z-axis, then the y-axis.
//

#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

typedef unsigned char byte;

static int version;
static int depth, height, width;
static int gridsize;
static byte *voxels = nullptr;
static float tx, ty, tz;
static float scale;
string filespec;


bool read_header(ifstream &input)
{
	string line;
	input >> line;  // #binvox
	if (line.compare("#binvox") != 0) {
		cout << "Error: first line reads [" << line << "] instead of [#binvox]" << endl;
		return false;
	}
	input >> version;
	cout << "reading binvox version " << version << endl;

	depth = -1;
	bool doneReadingHeader = false;
	while (input.good() && !doneReadingHeader) {
		input >> line;
		if (line.compare("data") == 0) {
			doneReadingHeader = true;
		}
		else if (line.compare("dim") == 0) {
			input >> depth >> height >> width;
		}
		else if (line.compare("translate") == 0) {
			input >> tx >> ty >> tz;
		}
		else if (line.compare("scale") == 0) {
			input >> scale;
		}
		else {
			cout << "  unrecognized keyword [" << line << "], skipping" << endl;
			char c;
			do {  // skip until end of line
				c = input.get();
			} while (input.good() && (c != '\n'));

		}
	}
	if (!doneReadingHeader) {
		cout << "  error reading header" << endl;
		return false;
	}
	if (depth == -1) {
		cout << "  missing dimensions in header" << endl;
		return false;
	}
	return true;
}

/*
 returns true if it could read the binvoxfile
 otherwise returns false
*/
bool read_binvox()
{
	ifstream input = ifstream(filespec.c_str(), ios::in | ios::binary);

	// read header
	bool headerReadCorrectly = read_header(input);
	if(!headerReadCorrectly)
	{
		return false;
	}

	gridsize = width * height * depth;
	voxels = new byte[gridsize];
	if (!voxels) {
		cout << "  error allocating memory" << endl;
		return false;
	}

	//
	// read voxel data
	//
	byte value;
	byte count;
	int index = 0;
	int end_index = 0;
	int nr_voxels = 0;

	input.unsetf(ios::skipws);  // need to read every byte now (!)
	input >> value;  // read the linefeed char \n(10)
//	input >> value;
	while ((end_index < gridsize) && input.good()) {
		input >> value >> count;

		if (input.good()) {
			end_index = index + count;
			if (end_index > gridsize) return false;
			for (int i = index; i < end_index; i++) voxels[i] = value;

			if (value) nr_voxels += count;
			index = end_index;
		}  // if file still ok

	}  // while

	input.close();
	cout << "  read " << nr_voxels << " voxels" << endl;

	return true;

}

void checkArgCount(int argc)
{
	if (argc != 2) {
		cout << "Usage: read_binvox <binvox filename>" << endl << endl;
		exit(1);
	}
}

void checkBinvoxFileCorrectlyRead(bool binvoxFileCorrectlyRead, char **argv)
{
	if (!binvoxFileCorrectlyRead) {
		cout << "Error reading [" << argv[1] << "]" << endl << endl;
		exit(1);
	}
}

void writeASCIIHeader(ofstream &out)
{
	out << "#binvox ASCII data" << endl;
	out << "dim " << depth << " " << height << " " << width << endl;
	out << "translate " << tx << " " << ty << " " << tz << endl;
	out << "scale " << scale << endl;
	out << "data" << endl;
}

int main(int argc, char **argv)
{
	checkArgCount(argc);

	filespec = argv[1];
	bool binvoxFileCorrectlyRead = read_binvox();
	checkBinvoxFileCorrectlyRead(binvoxFileCorrectlyRead, argv);

	//
	// now write the data to as ASCII
	//
	ofstream out = ofstream(filespec + "_voxels.txt");
	if (!out.good()) {
		cout << "Error opening [voxels.txt]" << endl << endl;
		exit(1);
	}

	cout << "Writing voxel data to ASCII file..." << endl;
	writeASCIIHeader(out);


	//X-coord slowest
	for (int i = 0; i < gridsize; i++) {
		if ((i % (depth*height)) == 0) {
			out << "x-coord: " << i / (depth * height) << endl;
		}
		out << (char)(voxels[i] + '0') << " ";
		if (((i + 1) % width) == 0)
		{
			out << endl;
		}
		if (((i + 1) % (depth *width)) == 0) {
			out << endl;
		}
	}

/*	//Y coord slices
	for (int y = 0; y < height; y++)
	{
		out << "y-coord: " << y << endl;
		out << "     x: ";
		for (int x = 0; x < depth; x++) { out << x << " "; } out << endl;
		for (int z = 0; z < width; z++)
		{
			out << "z = " << z << " : ";
			for (int x = 0; x < depth; x++)
			{
				int i =  x * width*height + z * height + y;
				out << (char)(voxels[i] + '0') << " ";
			}
			out << endl;
		}
		out << endl;
	}*/
	
	delete voxels;
	out.close();

	cout << "done" << endl << endl;

	return 0;
}