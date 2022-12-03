#include <mutex>
#include <string>

namespace TDA
{
    class TDASystem
    {
	private:
		static std::mutex storageMutex;

    private:
        //functions used to define the settings of the server
		static void errormsg(const char* message);
		static bool isDigit(std::string str);
    };
}