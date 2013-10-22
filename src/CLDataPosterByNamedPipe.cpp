#include "CLDataPosterByNamedPipe.h"
#include "CLIOVector.h"

using namespace std;

CLDataPosterByNamedPipe::CLDataPosterByNamedPipe(const char* strPipeName) : m_NamedPipe(strPipeName, PIPE_FOR_WRITE)
{

}