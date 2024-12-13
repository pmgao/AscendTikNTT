#include "../include/head.h"


int main() {  
  int deviceId = 0;
  aclrtContext context;
  aclrtStream stream;
  ACLRT_CHECK(aclInit(NULL));
  ACLRT_CHECK(aclrtSetDevice(deviceId));
  ACLRT_CHECK(aclrtCreateContext(&context, deviceId));
  ACLRT_CHECK(aclrtCreateStream(&stream));
  AiCoreKernel simpleAddKernel(
      std::string("/root/xkyin/TIK/AscendTikSimple/kernel_meta/"),
      std::string("simple_add.o"), std::string("simple_add__kernel0"));
  simpleAddKernel.loadKernel();
  aclrtEvent start_event;
  aclrtEvent end_event;
  aclrtCreateEvent(&start_event);
  aclrtCreateEvent(&end_event);
  int data_len = 128;

  float *d_input1;
  float *d_input2;
  float *d_output;

  ACLRT_CHECK(aclrtMalloc((void **)&d_input1, data_len * sizeof(float),
              ACL_MEM_MALLOC_NORMAL_ONLY));
  ACLRT_CHECK(aclrtMalloc((void **)&d_input2, data_len * sizeof(float),
              ACL_MEM_MALLOC_NORMAL_ONLY));
  ACLRT_CHECK(aclrtMalloc((void **)&d_output, data_len * sizeof(float),
              ACL_MEM_MALLOC_NORMAL_ONLY));

  std::vector<void *> args = {(void *)d_input1, (void *)d_input2,
                              (void *)d_output};
  aclrtRecordEvent(start_event, stream);
  
  simpleAddKernel.launchKernel(args.data(), args.size() * sizeof(void *), NULL,
                               stream);
  aclrtRecordEvent(end_event, stream);
  aclrtSynchronizeStream(stream);
  float time = 0;
  aclrtEventElapsedTime(&time, start_event, end_event);  
  std::cout <<time <<"ms" << std::endl; 
  ACLRT_CHECK(aclrtSynchronizeStream(stream));


  return 0;
}


