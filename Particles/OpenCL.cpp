#include "OpenCL.h"

size_t PARTICLE_BUFFER_SIZE = 1000000;
size_t GRAVITOR_BUFFER_SIZE = 8;

#define MAX_PLATFORM_ENTRIES 8
#define MAX_DEVICE_ENTRIES 16

//shared buffer for error logging
#define BUFFER_SIZE 1024
char PARAM_BUFFER[BUFFER_SIZE];
size_t ANSWER_SIZE;

const char* get_error_string(cl_int error)
{
    switch (error) {
        // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

        // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

        // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown OpenCL error";
    }
}
void check_error(cl_int error, int line)
{
    if (error != CL_SUCCESS)
    {
        std::cerr << "CL::ERROR::" << get_error_string(error) << std::endl << "LINE " << line << std::endl;
        exit(EXIT_FAILURE);
    }
}
void check_program_compile_error(cl_int error, cl_program program, cl_device_id device_id, int line)
{
    if (error == CL_SUCCESS)
        return;

    size_t len = 0;
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, NULL, NULL, &len);

    char* log = new char[len];
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, len, log, NULL);

    std::cout << log;
    exit(EXIT_FAILURE);
}
void check_kernel_compile_error(cl_int error, cl_program program, cl_device_id device_id, int line) //zelfde code als hierboven GG uninstall
{
    if (error == CL_SUCCESS)
        return;

    size_t len = 0;
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, NULL, NULL, &len);

    char* log = new char[len];
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, len, log, NULL);

    std::cout << log;
    exit(EXIT_FAILURE);
}

cl_platform_id _select_platform()
{
    unsigned int answer;

    cl_int error;

    cl_int platform_info;
    cl_uint num_platforms;
    cl_platform_id platform_arr[MAX_PLATFORM_ENTRIES];
    cl_platform_id platform_id;



    error = clGetPlatformIDs(MAX_PLATFORM_ENTRIES, platform_arr, &num_platforms);
    check_error(error, __LINE__);

    system("cls");
    std::cout << "Select a platform" << "\n" << "-----------------\n";
    for (size_t i = 0; i < num_platforms; i++)
    {
        platform_info = clGetPlatformInfo(platform_arr[i], CL_PLATFORM_NAME, BUFFER_SIZE, PARAM_BUFFER, &ANSWER_SIZE);
        std::cout << i + 1 << ": " << PARAM_BUFFER << '\n';
    }

    std::cin >> answer;

    if (answer < 1 || answer > num_platforms)
    {
        std::cout << "\nInvalid platform selected\n";
        exit(EXIT_FAILURE);
    }

    platform_id = platform_arr[answer - 1];
    return platform_id;
}
cl_device_id _select_device(cl_platform_id platform_id)
{
    unsigned int answer;
    int device_type;

    cl_int error;

    cl_int device_info;
    cl_uint num_devices;
    cl_device_id device_arr[MAX_DEVICE_ENTRIES];
    cl_device_id device_id;



    system("cls");
    std::cout << "Select a device type" << "\n" << "-----------------\n";
    std::cout << "1. CPU\n" << "2. GPU\n";
    std::cin >> answer;

    if (answer < 1 || answer > 2)
    {
        std::cout << "\nInvalid device selected\n";
        exit(EXIT_FAILURE);
    }

    switch (answer)
    {
    case 1:
        device_type = CL_DEVICE_TYPE_CPU;
        break;
    case 2:
        device_type = CL_DEVICE_TYPE_GPU;
        break;
    case 3:
        device_type = CL_DEVICE_TYPE_ALL;
        break;
    default:
        exit(EXIT_FAILURE);
    }

    error = clGetDeviceIDs(platform_id, device_type, MAX_DEVICE_ENTRIES, device_arr, &num_devices);
    check_error(error, __LINE__);



    system("cls");
    std::cout << "Select a device" << "\n" << "-----------------\n";
    for (size_t i = 0; i < num_devices; i++)
    {
        device_info = clGetDeviceInfo(device_arr[i], CL_DEVICE_NAME, BUFFER_SIZE, PARAM_BUFFER, &ANSWER_SIZE);
        std::cout << i + 1 << ": " << PARAM_BUFFER << '\n';
    }

    std::cin >> answer;

    if (answer < 1 || answer > num_devices)
    {
        std::cout << "\nInvalid device selected\n";
        exit(EXIT_FAILURE);
    }

    device_id = device_arr[answer - 1];
    return device_id;
}

void setup_cl()
{
    cl_platform_id platform_id;
    cl_device_id device_id;

    //cl_context context;
    //cl_command_queue command_queue;

    platform_id = _select_platform();
    device_id = _select_device(platform_id);
}
