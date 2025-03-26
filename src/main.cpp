#include "main_program.hpp"
#include "option_parser.hpp"
#include "spdlog_wrapper.hpp"

int main(const int argc, const char** argv)
{
	SPD_INFO_CLASS(COMMON::d_settings_group_application, "Application started");

	auto option_parser = UTILS::OptionParser(COMMON::d_project_name, COMMON::d_project_description);

	option_parser.addOption("h,help", "Prints help menu.");
	option_parser.addOption("d,debug", "Prints debug info.");

	option_parser.parseOptions(argc, argv);

	if (option_parser.hasOption("h"))
	{
		option_parser.logHelp();
	}

	if (option_parser.hasOption("d"))
	{
		option_parser.debugLog(argc, argv);
	}

	SPD_INFO_CLASS(COMMON::d_settings_group_application, "Application stopped");

	return 0;
}
