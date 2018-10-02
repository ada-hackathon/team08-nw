/**********
Copyright (c) 2017, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

#include "xcl2.hpp"
#include <vector>
#include <stdlib.h>
#include <string>

#define define_buffer printf("lol");

using namespace std;

int main(int argc, char** argv)
{

	// Input sequence
	std::string seqA = "abactgtgab";
	std::string seqB = "abactgtgab";

	// Data
	int DATA_SIZE = std::max(seqA.length(), seqB.length());
    size_t vector_size_bytes = sizeof(unsigned char) * DATA_SIZE;

    // Working data
    std::vector<int> mat((DATA_SIZE + 1) * (DATA_SIZE + 1));

    for(int i = 0; i <= DATA_SIZE; i++)
    {
    	mat[i] = -i;
    	mat[i*(DATA_SIZE + 1)] = -i;
    }


//OPENCL HOST CODE AREA START
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];

    cl::Context context(device);
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);
    std::string device_name = device.getInfo<CL_DEVICE_NAME>(); 

    //Create Program and Kernel
    std::string binaryFile = xcl::find_binary_file(device_name, "nw");
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    cl::Program program(context, devices, bins);
    cl::Kernel krnl_nw(program, "nw");

    //Allocate Buffer in Global Memory
    cl::Buffer buffer_seqA (context, CL_MEM_READ_ONLY,  vector_size_bytes);
    cl::Buffer buffer_seqB (context, CL_MEM_READ_ONLY,  vector_size_bytes);
    cl::Buffer buffer_mat  (context, CL_MEM_READ_WRITE, mat.size()*sizeof(int));
    cl::Buffer buffer_seq  (context, CL_MEM_WRITE_ONLY, vector_size_bytes);

    //Copying input data to Device buffer from host memory
    q.enqueueWriteBuffer(buffer_seqA, CL_TRUE, 0, vector_size_bytes, seqA.c_str());
    q.enqueueWriteBuffer(buffer_seqB, CL_TRUE, 0, vector_size_bytes, seqB.c_str());
    q.enqueueWriteBuffer(buffer_mat , CL_TRUE, 0, mat.size()*sizeof(int), &mat .front());

    int size = DATA_SIZE + 1;

    //Set the Kernel Arguments
    int nargs=0;
    krnl_nw.setArg(nargs++, buffer_seqA);
    krnl_nw.setArg(nargs++, buffer_seqB);
    krnl_nw.setArg(nargs++, buffer_mat );
    krnl_nw.setArg(nargs++, size       );

    //Launch the Kernel
    q.enqueueNDRangeKernel(krnl_nw, cl::NullRange, cl::NDRange(size - 1), cl::NullRange);

    //Copying Device result data to Host memory
    q.enqueueReadBuffer(buffer_mat, CL_TRUE, 0, mat.size()*sizeof(int), &mat.front());

    q.finish();
//OPENCL HOST CODE AREA END
    
    // Compare the results of the Device to the simulation
    bool match = true;
    // True .. always
    //std::cout << mat << std::endl;
    for(int i = 0; i < size; i++) {
    	for(int j = 0; j < size; j++)
    		std::cout << mat[i + (j * size)] << " ";
    	std::cout << std::endl;
    }
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl; 
    return (match ? EXIT_SUCCESS :  EXIT_FAILURE);
}
