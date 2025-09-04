#include "option_parser.hpp"

#include "settings_manager.hpp"
#include "spdlog_wrapper.hpp"

namespace UTILS
{
std::shared_ptr<OptionParser> OptionParser::m_instance = nullptr;
std::mutex					  OptionParser::m_mutex;

std::shared_ptr<OptionParser> OptionParser::instance()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_instance == nullptr)
	{
		SPD_DEBUG_CLASS(COMMON::d_settings_group_utils, "Initializing option parser.");
		m_instance = std::shared_ptr<OptionParser>(new OptionParser());
	}

	return m_instance;
}

OptionParser::OptionParser()
{
	this->initialize();
}

void OptionParser::initialize()
{
	this->m_options = std::make_unique<cxxopts::Options>(COMMON::d_project_name, COMMON::d_project_description);
	this->m_options->allow_unrecognised_options();
}

void OptionParser::setDescription(const std::string& app_name, const std::string& app_description)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	this->m_options = std::make_unique<cxxopts::Options>(app_name, app_description);

	return;
}

OptionParser::~OptionParser()
{
	this->m_options.reset();
	this->m_parsed_options.reset();

	return;
}

void OptionParser::parseOptions(const int argc, const char** argv)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (!this->m_options)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Unable to parse options, Option Parser is not initialized.");
	}

	try
	{
		this->m_parsed_options = std::make_unique<cxxopts::ParseResult>(this->m_options->parse(argc, argv));

		for (const auto& unmatched_argument : this->m_parsed_options->unmatched())
		{
			SPD_WARN_CLASS(COMMON::d_settings_group_options, fmt::format("Unsupported argument passed: {}", unmatched_argument));
		}

		return;
	}
	catch (const cxxopts::exceptions::exception& e)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, fmt::format("Error parsing command line options: {}", e.what()));
	}
	catch (...)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Unknown error occurred while parsing arguments.");
	}

	exit(1);
}

bool OptionParser::hasOption(const std::string& name) const
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!this->m_parsed_options)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Options have not been parsed yet. Call parseOptions() first.");
		return false;
	}

	return this->m_parsed_options->count(name) > 0;
}

void OptionParser::addOption(const std::string& name, const std::string& description)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!this->m_options)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Unable to add option, Option Parser is not initialized.");
		return;
	}

	this->m_options->add_options()(name, description);

	return;
}

size_t OptionParser::getOptionCount(const std::string& name) const
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!this->m_parsed_options)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Options have not been parsed yet. Call parseOptions() first.");
		return 0;
	}

	return this->m_parsed_options->count(name);
}

void OptionParser::logHelp() const
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!this->m_options)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Unable to get help message, Option Parser is not initialized.");
		return;
	}

	std::string		   help_message = this->m_options->help();
	std::istringstream iss(help_message);
	std::string		   line;

	while (std::getline(iss, line))
	{
		spdlog::info(" {}", line);
	}
}

void OptionParser::debugLog() const
{
	std::lock_guard<std::mutex> lock(m_mutex);

	auto settings_manager = UTILS::SettingsManager::instance();

	auto arguments = this->m_parsed_options->arguments();

	SPD_INFO_CLASS(COMMON::d_settings_group_options, "===========================================================");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "\tProject Information");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "-----------------------------------------------------------");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, fmt::format("\tProject Name: {:<24} {}", COMMON::d_project_name, COMMON::d_project_version));
	SPD_INFO_CLASS(COMMON::d_settings_group_options, fmt::format("\tCompile Time: {}", COMMON::d_compile_time));
	SPD_INFO_CLASS(COMMON::d_settings_group_options, fmt::format("\tCompiler:     {:<24} {}", COMMON::d_compiler_id, COMMON::d_compiler_version));
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "===========================================================");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "\tCommand-Line Arguments");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "-----------------------------------------------------------");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, fmt::format("\tArgument Count: {}", arguments.size()));

	for (int i = 0; i < arguments.size(); ++i)
	{
		auto key_temp = arguments.at(i).key();
		auto val_temp = arguments.at(i).value();

		SPD_INFO_CLASS(COMMON::d_settings_group_options, fmt::format("\tArgument [{}]: {} = {}", i, key_temp, val_temp));
	}

	auto dumped_settings = settings_manager->dump() | std::ranges::views::split('\n') | std::ranges::to<std::vector<std::string>>();

	SPD_INFO_CLASS(COMMON::d_settings_group_options, "===========================================================");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "\tSettings");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "-----------------------------------------------------------");
	for (auto& line : dumped_settings)
	{
		SPD_INFO_CLASS(COMMON::d_settings_group_options, fmt::format("\t{}", line));
	}
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "===========================================================");
}
} // namespace UTILS
