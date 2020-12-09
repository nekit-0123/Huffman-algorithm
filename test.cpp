#include "encode.h"
#include "decode.h"
#include <memory>
#include <thread>

int main(int argc, char* argv[])
{
	std::cout << "Input 'e' or 'd' (e - encode | d - decode): " << std::endl;
	std::string sAction;
	std::getline(std::cin, sAction);

	while (sAction.empty())
	{
		std::cout << "Input e or d (e - encode | d - decode): " << std::endl;
		std::getline(std::cin, sAction);
	}

	if (sAction == "E")
		sAction = "e";
	else if (sAction == "D")
		sAction = "d";


	if (sAction != "e" && sAction != "d")
	{
		std::cout << "Unknow " << std::endl;
		system("pause");
		return -1;
	}

	std::string sInputPath;
	std::string sOutputPath;

	std::cout << "Input the path and file name: " << std::endl;
	std::getline(std::cin, sInputPath);

	std::cout << "Output the path and file name: " << std::endl;
	std::getline(std::cin, sOutputPath);

	if (sInputPath.empty() || sOutputPath.empty())
	{
		std::cout << "Error: path and file name is empty" << std::endl;
		system("pause");
		return -1;	
	}
	else if (sInputPath == sOutputPath)
	{
		std::cout << "Error: these are two files with the same name" << std::endl;
		system("pause");
		return -1;
	}

	if (sAction == "e")
	{
		std::unique_ptr<CEncode> encode = std::make_unique<CEncode>();
		COMPRESSED_FILE* output(nullptr);
		FILE* input(nullptr);

		input = fopen(sInputPath.c_str(), "rb");
		if (input != nullptr)
		{
			output = encode.get()->OpenOutputCompressedFile(sOutputPath);
			if (output != nullptr)
			{		
				encode.get()->CompressFile(input, output);
				if (fclose(input) != 0)
					std::cout << "Error close file: " <<  input << std::endl;
				encode.get()->print_ratios(sInputPath, sOutputPath);
			}
			else
				encode.get()->fatal_error("Error open target file");
		}
		else 
			encode.get()->fatal_error("Error open source file");
	}
	else if (sAction == "d")
	{
		std::unique_ptr<CDecode> decode = std::make_unique<CDecode>();
		FILE* output(nullptr);
		COMPRESSED_FILE* input(nullptr);

		input = decode.get()->OpenInputCompressedFile(sInputPath);
		if (input != nullptr)
		{
			output = fopen(sOutputPath.c_str(), "wb");
			if (output != nullptr)
			{	
				decode.get()->ExpandFile(input, output);
				if (fclose(output) != 0)
					std::cout << "Error close file: " <<  output << std::endl;
			}
			else 
				decode.get()->fatal_error("Error open target file");
		}
		else
			decode.get()->fatal_error("Error open source file");
	}

	std::cout << "Completed" << std::endl;
	system("pause");

	return 0;
}