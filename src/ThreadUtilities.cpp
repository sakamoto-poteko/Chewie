#include "ThreadUtilities.h"

////////////////////////////////////////////////////////////////////////////////////
//template<>
void addThreadedObject(TTree* object, int nThreads)
{
    TTree* tmp;
    std::string name = object->GetName();
    for(int threadNumber=0; threadNumber<nThreads; threadNumber++)
    {
        tmp = THREADED(object);
        if(threadNumber==0)
        {
            object->Delete();
            object = tmp;
            object->SetName(name.c_str());
        }
        else
        {
            object->CopyEntries(tmp);
            tmp->Delete();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
std::string ThreadUtilities::itoa(int integer)
{
    char asciiVal[8];
    *sput_i(integer, asciiVal) = '\0';
    return std::string(asciiVal);
}

////////////////////////////////////////////////////////////////////////////////////
char* ThreadUtilities::sput_i(int integer, char *string)
{
    if (integer / 10 != 0)
    {
        string = sput_i(integer / 10, string);
    }
    *string++ = (char)('0' + integer % 10);
    return string;
}
