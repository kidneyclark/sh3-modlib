#include <sh3modlib.h>

#include <crk/vector.h>
#include <crk/string.h>

#include <iostream>
#include <fstream>

int main(int argc, char **argv)
{
	mem_CreateContext();
	COM_REGISTER_CRK(string);
	COM_REGISTER_CRK(vector);

	crk::vector<int> vec;

	std::ofstream dump_log("log.txt");

	std::ostream &dumpstream = dump_log; // = std::cout;

	mem_Dump(dumpstream);
	vec.push_back(1);
	mem_Dump(dumpstream);
	vec.reserve(16);
	mem_Dump(dumpstream);
	vec.push_back(4);
	mem_Dump(dumpstream);
	{
		dumpstream << "string creation" << std::endl;
		crk::string teststring = "hello"; 
		teststring += "concat";
		mem_Dump(dumpstream);
	}
	mem_Dump(dumpstream);

	return 0;
}