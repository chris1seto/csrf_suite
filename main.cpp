#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <random>
#include "QueueBuffer.hpp"
#include "CrsfParser.hpp"

void qbTest();
void parserTest();

int main()
{
	//qbTest();
	
	parserTest();
	
    return 0;
}

void qbTest()
{
	const int qb_size = 200;
	
	uint8_t write_counter = 0;
	uint8_t read_counter = 0;
	uint32_t buffer_count = 0;
	
	uint8_t rand_buffer[qb_size];
	uint32_t test_size;
	
	uint32_t start_count;
	
	
	QueueBuffer_t test_qb;
	uint8_t qb_buffer[qb_size];
	
	std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
	
	QueueBuffer_Init(&test_qb, qb_buffer, qb_size);
	
	for (int i = 0; i < 5000000; i++)
	{
		std::uniform_int_distribution<> distr(1, qb_size - QueueBuffer_Count(&test_qb));
		test_size = distr(gen);
		
		// Load the buffer
		for (int j = 0; j < test_size; j++)
		{
			rand_buffer[j] = write_counter++;
		}
		
		if (!QueueBuffer_AppendBuffer(&test_qb, rand_buffer, test_size))
		{
			std::cout << "QueueBuffer_AppendBuffer failed 1!" << std::endl;
			return;
		}
		buffer_count += test_size;
		
		if (QueueBuffer_Count(&test_qb) != buffer_count)
		{
			std::cout << "QueueBuffer_Count failed 1!" << std::endl;
			return;
		}
		
		// Read from the buffer
		std::uniform_int_distribution<> distr_read(1, QueueBuffer_Count(&test_qb));
		test_size = distr_read(gen);
		
		if ((i * test_size) % 2 == 0)
		{
			if (!QueueBuffer_PeekBuffer(&test_qb, 0, rand_buffer, test_size))
			{
				std::cout << "QueueBuffer_PeekBuffer failed 1!" << std::endl;
				return;
			}
			QueueBuffer_Dequeue(&test_qb, test_size);
			buffer_count -= test_size;
			
			if (QueueBuffer_Count(&test_qb) != buffer_count)
			{
				std::cout << "QueueBuffer_Count failed 2!" << std::endl;
				return;
			}
			
			for (int j = 0; j < test_size; j++)
			{
				if (rand_buffer[j] != read_counter++)
				{
					std::cout << "Buffer content failed 1!" << std::endl;
					return;
				}
			}
		}
		else
		{
			for (int j = 0; j < test_size; j++)
			{
				if (!QueueBuffer_Get(&test_qb, &rand_buffer[0]))
				{
					std::cout << "QueueBuffer_Get failed 1!" << std::endl;
					return;
				}
				
				buffer_count -= 1;
				
				if (rand_buffer[0] != read_counter++)
				{
					std::cout << "Buffer content failed 1!" << std::endl;
					return;
				}
			}
		}
	}
	
	std::cout << "Test done." << std::endl;
}

void parserTest()
{
	CrsfParserStatistics_t parser_statistics;
	CrsfPacket_t new_packet;
	
	uint32_t parse_chunk_list[3] = {153, 184, 104};
	uint32_t parse_chunk_i = 0;
	
	Crsf_Init();
	
	std::cout << "Init" << std::endl;
	
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
	
    std::ifstream input("../rx_captures/alreadyconnected.log", std::ios::binary);
	std::ofstream output("../rx_captures/alreadyconnected.log.2", std::ios::binary);
	input.seekg(0, input.end);
	int total_size = input.tellg();
	input.seekg(0, input.beg);
	
	std::cout << "File size: " << total_size << " Bytes" << std::endl;
	
	int read_size = 0;
	while (read_size < total_size)
	{
		std::uniform_int_distribution<> distr(1, std::min((total_size - read_size), (int)Csrf_FreeQueueSize()));
		int chunk_size = distr(gen);
		
		//int chunk_size = parse_chunk_list[parse_chunk_i++];
		char* chunk_buffer = new char[chunk_size];
		
		std::cout << "Chunk size: " << chunk_size << std::endl;
		
		input.read(chunk_buffer, chunk_size);
		output.write(chunk_buffer, chunk_size);
		
		if (!Crsf_LoadBuffer((uint8_t*)chunk_buffer, chunk_size))
		{
			std::cout << "Buffer write failed" << std::endl;
		}
		
		while (Crsf_TryParseCrsfPacket(&new_packet, &parser_statistics))
		{
			std::cout << "disposed_bytes: " << parser_statistics.disposed_bytes
				<< ", crcs_valid_known_packets: " << parser_statistics.crcs_valid_known_packets
				<< ", crcs_valid_unknown_packets: " << parser_statistics.crcs_valid_unknown_packets
				<< ", crcs_invalid: " << parser_statistics.crcs_invalid
				<< ", invalid_known_packet_sizes: " << parser_statistics.invalid_known_packet_sizes << std::endl;
				
			if (parser_statistics.disposed_bytes)
			{
				return;
			}
		}
		
		read_size += chunk_size;
	}

    input.close();
	output.close();
	
	std::cout << "disposed_bytes: " << parser_statistics.disposed_bytes
		<< ", crcs_valid_known_packets: " << parser_statistics.crcs_valid_known_packets
		<< ", crcs_valid_unknown_packets: " << parser_statistics.crcs_valid_unknown_packets
		<< ", crcs_invalid: " << parser_statistics.crcs_invalid
		<< ", invalid_known_packet_sizes: " << parser_statistics.invalid_known_packet_sizes << std::endl;
	
}