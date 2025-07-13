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

	crk::vector<int> caca;

	std::ofstream dump_log("log.txt");

	std::ostream &dumpstream = dump_log; // = std::cout;

	mem_Dump(dumpstream);
	caca.push_back(1);
	mem_Dump(dumpstream);
	caca.reserve(16);
	mem_Dump(dumpstream);
	caca.push_back(4);
	mem_Dump(dumpstream);
	{
		dumpstream << "Creo string" << std::endl;
		crk::string teststring = "culosexo"; 
		teststring += "pepesasasasasa";
		mem_Dump(dumpstream);
	}
	mem_Dump(dumpstream);

	return 0;
}