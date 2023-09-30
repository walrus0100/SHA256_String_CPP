/*This C++ code calculates the SHA-256 hash of a string using CryptoPP.

g++ -O2 -Wall -std=c++17 main.cpp -o main -lcryptopp

todo:

**/

#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>

#include <unistd.h>
#include <sys/wait.h>

#include <cryptopp/sha.h>

std::string generate(int length = 10)
{
	std::string letters;

	//initialize random seed
	std::srand(std::time(0));

	//generate random characters
	for(int i = 0; i < length; i++)
	{
		//generate uppercase letters only
		letters += std::rand() % 26 + 'A';
	}

	return letters;
}


std::string string_to_hex(const std::string& input)
{
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i)
	{
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return output;
}


std::string SHA256(std::string data)
{
	CryptoPP::byte const* pbData = (CryptoPP::byte*)data.data();
	unsigned int nDataLen = data.length();
	CryptoPP::byte abDigest[CryptoPP::SHA256::DIGESTSIZE];

	CryptoPP::SHA256().CalculateDigest(abDigest, pbData, nDataLen);

	// return string((char*)abDigest);  -- BAD!!!
	return std::string((char*)abDigest, CryptoPP::SHA256::DIGESTSIZE);
}


void flushToFile(std::vector<std::string>& dataCollection, std::vector<std::string>& hashCollection, std::string filename)
{
	// Open the file for writing.
	std::ofstream file(filename);

	if (!file.bad())
	{

		// Iterate over the vector of original seeds and hashes and write each hash to the file.
		file << "Seeds: " << std::endl;
		int seedCount {1};
		for (const auto& data : dataCollection)
		{

			file << "(" << seedCount << ")" << data << std::endl ;
			seedCount++;
		}

		file << '\n' << std::endl;

		file << "Hashes: "<< std::endl;
		int hashCount{1};
		for (const auto& hash : hashCollection)
		{

			file << "(" << hashCount << ")" << "\n" << hash << std::endl;
			hashCount++;

		}

	}

	// Close the file.
	file.close();
}


int main(int argc, char *argv[])
{

	if(argc != 2)
	{
		std::cerr << " Wrong number of arguments"  << "\n"
			  << " Usage: " << argv[0]  << " flush_file" 
			  << std::endl;

		return EXIT_FAILURE;
	}

	

	//titlecard
	std::cout << R"( Your friendly neighboorhood SHA256 hash generator! :))" << std::endl;
	
	//input filename
	std::string fileName = argv[1];
	
	
	//containers
	std::string data {"empty"};
	std::string sha256 {"empty"};
	std::vector<std::string> dataCollection;
	std::vector<std::string> hashCollection;
	
	//seed length selector
	int seedLength {1};	
	std::cout << "Seed length: " << std::endl;
      	std::cin >> seedLength;
	std::cout << "Seed length will be: " << seedLength  << " characters long." <<std::endl;
         
	//How many hashes would you like
	int hashWorkCount {1};
	std::cout << "\n" << "Number of hashes: " << std::endl;
      	std::cin >> hashWorkCount;
	std::cout << "There will be: " << hashWorkCount <<" hashes."  << "\n" << std::endl;


	std::cout << "Seeds:" << std::endl;		
	//main loop will start at 1
	for (int workCount {1}; workCount <= hashWorkCount; workCount++)
	{
		data.assign(generate(seedLength));
		dataCollection.push_back(data);
	
		std::cout << workCount << ":" << data << std::endl;

		sha256.assign(string_to_hex(SHA256(data)));
		hashCollection.push_back(sha256);

		data.clear();
		sha256.clear();
		//need to wait so the srand can be seeded with new a new time
		sleep(1);

	}

	//output to terminal will start at 1
	std::cout << "\n" << "Hashes: " << std::endl;
	int iterCount {1};
	for (auto iter {hashCollection.begin()};
	iter!= hashCollection.end(); ++iter)
	{

		std::cout << iterCount << ":" << *iter << std::endl;
		iterCount++;
	}

	//flush function	
	flushToFile(dataCollection, hashCollection, fileName);

	std::cout <<"\n" << "Flushed to file named " 
		  << fileName 
		  << std::endl;

	return 0;
}
