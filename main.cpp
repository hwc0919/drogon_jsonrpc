#include <drogon/HttpAppFramework.h>
#include <getopt.h>
#include <string>

using namespace drogon;

static std::string configFile = "./config.json";

int ParseOptions(int argc, char * const argv[]);
void PrintHelp(int argc, char * const argv[]);
void beginningAdvice();

int main(int argc, char * argv[])
{
    int ret = ParseOptions(argc, argv);
    if (ret != 1)
    {
        return ret;
    }
    // Load config file
    HttpAppFramework & app = drogon::app();
    app.loadConfigFile(configFile);

    LOG_INFO << "******************** Server Start ********************";
    app.registerBeginningAdvice(beginningAdvice);

    // Start application
    app.run();
    return 0;
}

void PrintHelp(int argc, char * const argv[])
{
    printf("Usage: %s [options...]\n"
           "  -c, --config-file     Json config file\n"
           "  -h, --help            Print help\n"
           "  -v, --version         Print build version\n"
           "\n",
           argv[0]);
}

int ParseOptions(int argc, char * const argv[])
{
    static struct option longopts[] = {
        { "config-file", required_argument, nullptr, 'c' },
        { "help", no_argument, nullptr, 'h' },
        { "version", no_argument, nullptr, 'v' },
        { nullptr, 0, nullptr, 0 }
    };

    int ch = -1;
    while ((ch = getopt_long(argc, argv, "c:hv", longopts, nullptr)) != -1)
    {
        switch (ch)
        {
            case 'c': {
                configFile = optarg;
                break;
            }
            case 'h': {
                PrintHelp(argc, argv);
                return 0;
            }
            default: {
                return -1;
            }
        }
    }

    return 1;
}

void beginningAdvice()
{
    // add code here
}
