/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 30th APR 2022
 * @file sha256.cpp
 * @brief this file contains the definitions of sha256.hpp's declarations
 * @version 0.0.1 alpha [exam]
 */

#include "sha256.hpp"

using namespace _sha256_rp96_;

//public methods
sha256::sha256(char* file_data, int64_t size_data) noexcept(true):
_constants_table{ 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 }
{
  static_assert(SIZE_1GB_MESSAGE_LIST > 0);

	this->_file_data = file_data;
	this->_can_we_read_the_file = true;
	this->_hash_is_already_calc = false;
	this->_sizes_bytes_input = size_data;
	this->_sizes_bits_input = size_data * 8;

	//calculate vars for padding
	this->_number_of_zeros_for_padding = static_cast<uint32_t>((this->_sizes_bits_input % 512 >= 448) ? (1024 - ((this->_sizes_bits_input % 512) + 65)) : (512 - (65 + (this->_sizes_bits_input % 512))));
	this->_number_of_cycles_for_compute_sha256 = (this->_sizes_bits_input + 1 + this->_number_of_zeros_for_padding + 64) / 512;
	this->_bytes_already_reads = 0;

	//this->_hash_result[_SIZE_HASH_TABLE] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
	this->_hash_result[0] = 0x6a09e667;
	this->_hash_result[1] = 0xbb67ae85;
	this->_hash_result[2] = 0x3c6ef372;
	this->_hash_result[3] = 0xa54ff53a;
	this->_hash_result[4] = 0x510e527f;
	this->_hash_result[5] = 0x9b05688c;
	this->_hash_result[6] = 0x1f83d9ab;
	this->_hash_result[7] = 0x5be0cd19;
}

sha256::sha256(const sha256& s256) noexcept(true):
  _can_we_read_the_file{ s256._can_we_read_the_file },
  _hash_is_already_calc{ s256._hash_is_already_calc },
  _number_of_zeros_for_padding{ s256._number_of_zeros_for_padding },
  _number_of_cycles_for_compute_sha256{ s256._number_of_cycles_for_compute_sha256 },
  _sizes_bits_input{ s256._sizes_bits_input },
  _sizes_bytes_input{ s256._sizes_bytes_input },
  _bytes_already_reads{ s256._bytes_already_reads },
  _file_data{ s256._file_data },
  _constants_table{ 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 }
{
  this->_hash_result = s256._hash_result;
  this->_workspace = s256._workspace;
}

sha256& sha256::operator=(const sha256& s256) noexcept(true)
{
  //first i need to clear or overwrite the actual data
  this->_can_we_read_the_file = s256._can_we_read_the_file;
  this->_hash_is_already_calc = s256._hash_is_already_calc;
  this->_number_of_zeros_for_padding = s256._number_of_zeros_for_padding;
  this->_number_of_cycles_for_compute_sha256 = s256._number_of_cycles_for_compute_sha256;
  this->_sizes_bits_input = s256._sizes_bits_input;
  this->_sizes_bytes_input = s256._sizes_bytes_input;
  this->_bytes_already_reads = s256._bytes_already_reads;
  this->_file_data = s256._file_data;
  this->_hash_result = s256._hash_result;
  //std::copy_n(this->_hash_result, _SIZE_HASH_TABLE, s256._hash_result);
  this->_workspace.clear();
  this->_workspace = s256._workspace;
  return *this;
}

sha256::sha256(sha256&& s256) noexcept(true):
  _can_we_read_the_file{ s256._can_we_read_the_file },
  _hash_is_already_calc{ s256._hash_is_already_calc },
  _number_of_zeros_for_padding{ s256._number_of_zeros_for_padding },
  _number_of_cycles_for_compute_sha256{ s256._number_of_cycles_for_compute_sha256 },
  _sizes_bits_input{ s256._sizes_bits_input },
  _sizes_bytes_input{ s256._sizes_bytes_input },
  _bytes_already_reads{ s256._bytes_already_reads },
  _file_data{ s256._file_data },
  _constants_table{ 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 }
{
  this->_hash_result.swap(s256._hash_result);
  this->_file_data = s256._file_data;
  s256._file_data = nullptr;
  this->_workspace = std::move(s256._workspace);
  s256._workspace.clear();
}

sha256& sha256::operator=(sha256&& s256) noexcept(true)
{
  this->_can_we_read_the_file = s256._can_we_read_the_file;
  this->_hash_is_already_calc = s256._hash_is_already_calc;
  this->_number_of_zeros_for_padding = s256._number_of_zeros_for_padding;
  this->_number_of_cycles_for_compute_sha256 = s256._number_of_cycles_for_compute_sha256;
  this->_sizes_bits_input = s256._sizes_bits_input;
  this->_sizes_bytes_input = s256._sizes_bytes_input;
  this->_bytes_already_reads = s256._bytes_already_reads;
  this->_file_data = s256._file_data;
  this->_hash_result.swap(s256._hash_result);
  this->_workspace.clear();
  this->_workspace = std::move(s256._workspace);
  s256._workspace.clear();
  return *this;
}

void sha256::compute_hash()
{
	if (this->_can_we_read_the_file == false)
	{
		return;
	}
	else if (this->_sizes_bytes_input == 0)
	{
		this->_hash_result[0] = 0xe3b0c442;
		this->_hash_result[1] = 0x98fc1c14;
		this->_hash_result[2] = 0x9afbf4c8;
		this->_hash_result[3] = 0x996fb924;
		this->_hash_result[4] = 0x27ae41e4;
		this->_hash_result[5] = 0x649b934c;
		this->_hash_result[6] = 0xa495991b;
		this->_hash_result[7] = 0x7852b855;
		return;
	}

	std::vector<std::thread> _thread_vector;
	_thread_vector.push_back(std::thread([&]() { this->prepare_message(); }));
	_thread_vector.push_back(std::thread([&]() { this->start_read(); }));

	for (auto& _single_thread : _thread_vector)
		_single_thread.join();
	_thread_vector.clear();

	this->_hash_is_already_calc = true;
}

std::string sha256::get_SHA256_string()
{
	std::stringstream hash_result_string;

	if (this->_can_we_read_the_file == false) //we can't calculate SHA256
	{
//#pragma unroll(_SIZE_HASH_TABLE)
		for (int32_t idx = 0; idx < _SIZE_HASH_TABLE; hash_result_string << "00000000", ++idx);
		return hash_result_string.str();
	}

//#pragma unroll(_SIZE_HASH_TABLE)
	for (int32_t idx = 0; idx < _SIZE_HASH_TABLE; ++idx)
	{
		hash_result_string << this->uint32_to_hex_string_with_8char(this->_hash_result[idx]);
	}
	return hash_result_string.str();
}

std::array <uint32_t, _SIZE_HASH_TABLE> sha256::get_SHA256_uint32_array()
{
	if (this->_can_we_read_the_file == false) //we can't calculate SHA256
	{
//#pragma unroll(_SIZE_HASH_TABLE)
		for (int32_t idx = 0; idx < _SIZE_HASH_TABLE; this->_hash_result[++idx] = 0);
	}
	return this->_hash_result;
}

//private methods

//has pointer for 0..15
//plus static array for 16..63
void sha256::prepare_message()
{
	//I READ EVERTHING EXCEPT THE LAST 512 BIT
	uint64_t max = this->_number_of_cycles_for_compute_sha256 - (((this->_sizes_bits_input % 512) >= 448) == true ? (2) : (1));

	uint64_t i;
	for (i = 0; i < max; ++i) //I turn for every list block
	{
		if (safe_t_get_size() > SIZE_1GB_MESSAGE_LIST) //check if we are not consuming too much memory
		{
			--i;
			continue;
		}

		//new block
		this->safe_t_pushback();
		//this->_workspace.push_back(message()); //I create new block
		this->_workspace.back()._message_00_15 = (uint8_t*)(this->_file_data + (i * _SIZE_WORKSPACE_FILLED_WITH_MESSAGE * sizeof(uint32_t) / sizeof(uint8_t)));
		this->_bytes_already_reads += _SIZE_WORKSPACE_FILLED_WITH_MESSAGE * sizeof(uint32_t);
		this->unlock_back();
	}
	//END READ
	//START LAST 512 BIT
	this->safe_t_pushback();

	uint32_t coutdown = static_cast<uint32_t>(this->_sizes_bytes_input - this->_bytes_already_reads); //coutdown tell me the number of bytes free

	uint8_t* mex_p = new uint8_t[_SIZE_WORKSPACE_FILLED_WITH_MESSAGE * sizeof(uint32_t) / sizeof(uint8_t)];
	this->_workspace.back()._message_00_15 = mex_p;
	this->_workspace.back()._has_00_15_to_heap = true;

	for (uint32_t j = 0; j < coutdown; ++j)
	{
		this->_workspace.back()._message_00_15[j] = this->_file_data[(i * _SIZE_WORKSPACE_FILLED_WITH_MESSAGE * sizeof(uint32_t) / sizeof(uint8_t)) + j];
	}
	for (uint32_t j = coutdown; j < _SIZE_WORKSPACE_FILLED_WITH_MESSAGE * sizeof(uint32_t); ++j)
	{
		this->_workspace.back()._message_00_15[j] = 0;
	}
	//unlock_back(); //I DONT UNLOCK 'CAUSE MISS THE FINAL PADDING

	//I have read every byte of input
	//I call prepare padding
	this->prepare_padding();
}

void sha256::prepare_padding()
{
	//I will use _sizes_bits_input + _number_of_zeros_for_padding
	//this->_number_of_zeros_for_padding = static_cast<uint32_t>((this->_sizes_bits_input % 512 >= 448) ? (1024 - ((this->_sizes_bits_input % 512) + 65)) : (512 - (65 + (this->_sizes_bits_input % 512))));
	//this->_number_of_cycles_for_compute_sha256 = (this->_sizes_bits_input + 1 + this->_number_of_zeros_for_padding + 64) / 512;
	bool _need_I_another_block = ((this->_sizes_bits_input % 512) >= 448);

	if (_need_I_another_block == true)
	{
		//I need to understand if the padding start between the bits 0..31 OR 32..63
		uint64_t tmp_number_of_zeros_for_padding = (this->_number_of_zeros_for_padding % 64); //this is the first bit for padding

		uint64_t padding_start_0xE_or_0xF = 0;
		if (tmp_number_of_zeros_for_padding >= 32) //padding start between 32..63
		{
			tmp_number_of_zeros_for_padding -= 32; //now I start to count like 0..31
			padding_start_0xE_or_0xF = 0xE;
		}
		else //padding start between 0..31
		{
			padding_start_0xE_or_0xF = 0xF;
		}
		this->_workspace.back()._message_00_15[(padding_start_0xE_or_0xF * sizeof(uint32_t)) + ((sizeof(uint32_t)) - ((tmp_number_of_zeros_for_padding / 8) + 1))] |= (1 << (tmp_number_of_zeros_for_padding % 8));

		this->unlock_back();
		this->safe_t_pushback();

		uint8_t* mex_p = new uint8_t[_SIZE_WORKSPACE_FILLED_WITH_MESSAGE * sizeof(uint32_t) / sizeof(uint8_t)];
		this->_workspace.back()._message_00_15 = mex_p;
		this->_workspace.back()._has_00_15_to_heap = true;

		//I create new block
		for (uint64_t idx = 0; idx < ((_SIZE_WORKSPACE_FILLED_WITH_MESSAGE - 2) * sizeof(uint32_t)); this->_workspace.back()._message_00_15[idx++] = 0);
	}
	else
	{
		uint64_t tmp_number_of_zeros_for_padding = this->_number_of_zeros_for_padding;
		uint64_t idx;
		for (idx = (_SIZE_WORKSPACE_FILLED_WITH_MESSAGE - 2) * sizeof(uint32_t); tmp_number_of_zeros_for_padding > 8; this->_workspace.back()._message_00_15[--idx] = 0, tmp_number_of_zeros_for_padding -= 8);
		if (idx > 0)
		{
			this->_workspace.back()._message_00_15[--idx] |= 1 << tmp_number_of_zeros_for_padding;
		}
	}
	//seconds 32 bits, I have x4 8 bits
	this->_workspace.back()._message_00_15[0xF * sizeof(uint32_t)] = static_cast<uint8_t>(this->_sizes_bits_input >> (8 * (sizeof(uint32_t) - 1)));
	this->_workspace.back()._message_00_15[(0xF * sizeof(uint32_t)) + 1] = static_cast<uint8_t>(this->_sizes_bits_input >> (8 * (sizeof(uint32_t) - 2)));
	this->_workspace.back()._message_00_15[(0xF * sizeof(uint32_t)) + 2] = static_cast<uint8_t>(this->_sizes_bits_input >> (8 * (sizeof(uint32_t) - 3)));
	this->_workspace.back()._message_00_15[(0xF * sizeof(uint32_t)) + 3] = static_cast<uint8_t>(this->_sizes_bits_input);
	//first 32bits, I have x4 8 bits
	this->_workspace.back()._message_00_15[0xE * sizeof(uint32_t)] = static_cast<uint8_t>(this->_sizes_bits_input >> ((8 * (sizeof(uint32_t) - 1)) + (8 * sizeof(uint32_t))));
	this->_workspace.back()._message_00_15[(0xE * sizeof(uint32_t)) + 1] = static_cast<uint8_t>(this->_sizes_bits_input >> ((8 * (sizeof(uint32_t) - 2)) + (8 * sizeof(uint32_t))));
	this->_workspace.back()._message_00_15[(0xE * sizeof(uint32_t)) + 2] = static_cast<uint8_t>(this->_sizes_bits_input >> ((8 * (sizeof(uint32_t) - 3) + (8 * sizeof(uint32_t)))));
	this->_workspace.back()._message_00_15[(0xE * sizeof(uint32_t)) + 3] = static_cast<uint8_t>(this->_sizes_bits_input >> (8 * sizeof(uint32_t)));
	this->unlock_back();
}

void sha256::start_read()
{
	//I need to run read_data #_number_of_cycles_for_compute_sha256 times
	for (uint64_t idx = 0; idx < this->_number_of_cycles_for_compute_sha256; ++idx)
	{
		//but every times I need to wait the other thread for first block
		switch (this->safe_t_get_size()) //check size thread-safe
		{
		case 0: //if _workspace it's empty, this for-cycle doesn't matter
			--idx; //if idx == 0 -> (--idx) == 2^(64)-1, THEN (++idx) == 2^(64)-1+1 == 0
			break;
		case 1: //I start the check only if _workspace it's not empty
			while (!ctrl_front()); //check the unique block is ready to be used
			this->read_data(); //I read the first block, then clean memory
			break;
		default: //size > 1, then I can read without problems
			this->read_data(); //I read the first block, then clean memory
			break;
		}
	}
}

void sha256::read_data()
{
	///TODO: read the first block of padding
	//I follow the document FEDERAL INFORMATION PROCESSING STANDARDS PUBLICATION 180 - 4 about SECURE HASH STANDARD 256, relased in August 2015
	uint32_t a, b, c, d, e, f, g, h;

	///We start to calculate the hash about bytes read from prepare_workspace
	//Initialize the eight working variables, a, b, c, d, e, f, g, and h, with the(i - 1)st hash value
	a = this->_hash_result[0];
	b = this->_hash_result[1];
	c = this->_hash_result[2];
	d = this->_hash_result[3];
	e = this->_hash_result[4];
	f = this->_hash_result[5];
	g = this->_hash_result[6];
	h = this->_hash_result[7];

	//for firsts 16 blocks I calc like usually
	//the only difference its the *char, I need to read *4 char to get the correct data
	for (uint32_t idx_singleblock = 0; idx_singleblock < _SIZE_WORKSPACE_FILLED_WITH_MESSAGE; ++idx_singleblock)
	{
		uint32_t T1 = h + this->SIGMA1(e) + this->Ch(e, f, g) + this->_constants_table[idx_singleblock] + this->uint32_value_from_char_array(this->_workspace.front()._message_00_15, idx_singleblock);
		//uint32_t T2 = this->SIGMA0(a) + this->Maj(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		//a = T1 + T2;
		a = T1 + this->SIGMA0(a) + this->Maj(a, c, d); //this->Maj(a, b, c) it's correct if I run the function at the top of the cycle
	}

	for (uint32_t idx_singleblock = _SIZE_WORKSPACE_FILLED_WITH_MESSAGE; idx_singleblock < _SIZE_WORKING_ARRAYS; ++idx_singleblock)
	{
		uint32_t _work_tmp =
			this->sigma1(
				((idx_singleblock - 2 < _SIZE_WORKSPACE_FILLED_WITH_MESSAGE) ?
					(this->uint32_value_from_char_array(this->_workspace.front()._message_00_15, idx_singleblock - 2)) :
					(this->_workspace.front()._message_16_63[idx_singleblock - (2 + _SIZE_WORKSPACE_FILLED_WITH_MESSAGE)]))
			) +

			((idx_singleblock - 7 < _SIZE_WORKSPACE_FILLED_WITH_MESSAGE) ?
				(this->uint32_value_from_char_array(this->_workspace.front()._message_00_15, idx_singleblock - 7)) :
				(this->_workspace.front()._message_16_63[idx_singleblock - (7 + _SIZE_WORKSPACE_FILLED_WITH_MESSAGE)])) +

			this->sigma0(
				((idx_singleblock - 15 < _SIZE_WORKSPACE_FILLED_WITH_MESSAGE) ?
					(this->uint32_value_from_char_array(this->_workspace.front()._message_00_15, idx_singleblock - 15)) :
					(this->_workspace.front()._message_16_63[idx_singleblock - (15 + _SIZE_WORKSPACE_FILLED_WITH_MESSAGE)]))
			) +

			((idx_singleblock - 16 < _SIZE_WORKSPACE_FILLED_WITH_MESSAGE) ?
				(this->uint32_value_from_char_array(this->_workspace.front()._message_00_15, idx_singleblock - 16)) :
				(this->_workspace.front()._message_16_63[idx_singleblock - (16 + _SIZE_WORKSPACE_FILLED_WITH_MESSAGE)]));

		this->_workspace.front()._message_16_63[idx_singleblock - _SIZE_WORKSPACE_FILLED_WITH_MESSAGE] = _work_tmp;

		uint32_t T1 = _work_tmp + h + this->SIGMA1(e) + this->Ch(e, f, g) + this->_constants_table[idx_singleblock];
		//uint32_t T2 = this->SIGMA0(a) + this->Maj(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		//a = T1 + T2;
		a = T1 + this->SIGMA0(a) + this->Maj(a, c, d); //this->Maj(a, b, c) it's correct if I run the function at the top of the cycle
	}
	//We had calculate the partial hash about all this->_workspace.front()._message[0..63]
	//Just save the partial hash, it'll necessary for next partial hash's calc
	this->_hash_result[0] += a;
	this->_hash_result[1] += b;
	this->_hash_result[2] += c;
	this->_hash_result[3] += d;
	this->_hash_result[4] += e;
	this->_hash_result[5] += f;
	this->_hash_result[6] += g;
	this->_hash_result[7] += h;
	this->safe_t_popfront();
}

std::string sha256::uint32_to_hex_string_with_8char(uint32_t _input_value)
{
	//TODO: print the hex number always with 8 chars:
	//so the number 0x91FAAB02 become "91FAAB02" and nothing change,
	//the number 0x5 become "00000005", we will add the prefix "0" seven times
	//the number 0xA3F become "00000A3F", we will add the prefix "0" five times
	//the number 0x0 become "00000000", we will add the prefix "0" eight times

	std::stringstream _tmp_sstream;

	//accept change on input, doesn't necessary 'cause copy-pass
	uint32_t _tmp_work = _input_value;

	//if the input it's 0 we need to print x8 0's only
	if (_input_value == 0)
	{
		_tmp_sstream << "00000000";
		return _tmp_sstream.str();
	}

	//we take the input, add a prefix "0" in _tmp_sstream AND add a suffix "0" in _tmp_work (left shift x4)
	//when we know _tmp_work has x8 letter (_tmp_work >= 0x10000000) then we return the string
	while (true)
	{
		if (_tmp_work >= 0x10000000)
		{
			_tmp_sstream << std::hex << _input_value;
			return _tmp_sstream.str();
		}
		_tmp_sstream << "0";
		_tmp_work <<= 4;
	}
}

constexpr uint32_t sha256::Ch(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & y) ^ ((~x) & z);
}

constexpr uint32_t sha256::Maj(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & y) ^ (x & z) ^ (y & z);
}

constexpr uint32_t sha256::ROTR(uint32_t x, uint32_t param)
{
	return (x >> param) | (x << ((sizeof(uint32_t) * 8) - param));
}

constexpr uint32_t sha256::ROTL(uint32_t x, uint32_t param)
{
	return (x << param) | (x >> ((sizeof(uint32_t) * 8) - param));
}

constexpr uint32_t sha256::SHR(uint32_t x, uint32_t param)
{
	return x >> param;
}

constexpr uint32_t sha256::SIGMA0(uint32_t x)
{
	return ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22);
}

constexpr uint32_t sha256::SIGMA1(uint32_t x)
{
	return ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25);
}

constexpr uint32_t sha256::sigma0(uint32_t x)
{
	return ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3);
}

constexpr uint32_t sha256::sigma1(uint32_t x)
{
	return ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10);
}

uint32_t sha256::safe_t_get_size()
{
	const std::lock_guard<std::mutex> lock(this->_flag_mtx_list_p);
	return static_cast<uint32_t>(this->_workspace.size());
}

void sha256::safe_t_pushback()
{
	const std::lock_guard<std::mutex> lock(this->_flag_mtx_list_p);
	this->_workspace.push_back(message()); //I create new block
}

void sha256::safe_t_popfront() //read the flag inside the mutex and unlock after the value
{
	const std::lock_guard<std::mutex> lock(this->_flag_mtx_list_p);
	this->_workspace.pop_front();
}

void sha256::unlock_back()
{
	const std::lock_guard<std::mutex> lock(this->_workspace.back()._flag_mtx);
	this->_workspace.back()._has_00_15_read = true;
}

bool sha256::ctrl_front() //read the flag inside the mutex and unlock after the value
{
	const std::lock_guard<std::mutex> lock(this->_workspace.front()._flag_mtx);
	return this->_workspace.front()._has_00_15_read;
}

constexpr uint32_t sha256::uint32_value_from_char_array(const uint8_t* array_data, const uint32_t& index)
{
	uint32_t return_value = 0;
	for (uint32_t i = 0; i < sizeof(uint32_t); i++)
	{
		return_value = static_cast<uint32_t>((static_cast<uint8_t>(array_data[index * sizeof(uint32_t)]) << (8 * (sizeof(uint32_t) - 1))) + (static_cast<uint8_t>(array_data[(index * sizeof(uint32_t)) + 1]) << (8 * (sizeof(uint32_t) - 2))) + (static_cast<uint8_t>(array_data[(index * sizeof(uint32_t)) + 2]) << (8 * (sizeof(uint32_t) - 3))) + (static_cast<uint8_t>(array_data[(index * sizeof(uint32_t)) + 3])));
	}
	return return_value;
}
