#pragma  once
#include<map>
#ifdef WIN32
#pragma warning(disable: 4996)
#define snprintf _snprintf
#endif
using namespace std;

class   CConfigFileReader
{
public:
	static CConfigFileReader* getInstance();

	~CConfigFileReader();
	int initConfigName(const char * file);
    char* getConfigName(const char* name);
    int setConfigValue(const char* name, const char*  value);
private:
	CConfigFileReader();
    int _LoadFile(const char* filename);
    int _WriteFIle(const char*filename = NULL);
    void _ParseLine(char* line);
    char* _TrimSpace(char* name);

    bool m_load_ok;
    map<string, string> m_config_map;
    string m_config_file;
};
