#pragma once

#include <unordered_map>
#include <string>
#include <functional>
//#include <boost/program_options/options_description.hpp>
//#include <boost/program_options/positional_options.hpp>
//#include <boost/program_options/parsers.hpp>
//#include <boost/program_options/variables_map.hpp>

namespace Unvirt {

	//struct CmdRegisteryEntry {
	//	boost::program_options::options_description mOptDesc;
	//	boost::program_options::positional_options_description mPosOptDesc;
	//	std::function<void(boost::program_options::options_description&, boost::program_options::variables_map&)> mBindProc;
	//};

	class InteractiveCmd {
	public:
		InteractiveCmd();
		~InteractiveCmd();

		void Run(void) { ; }

	private:
		//std::unordered_map<std::string, CmdRegisteryEntry> mCmdDispatcher;

		
	};

}
