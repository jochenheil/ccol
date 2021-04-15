#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <boost/json/src.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

namespace json = boost::json;

typedef std::unordered_map<std::string, std::string> colorMap;

colorMap bashColors = {
    {"dark gray", "1;30"},
    {"light red", "1;31"},
    {"light green", "1;32"},
    {"yellow", "1;33"},
    {"light blue", "1;34"},
    {"light purple", "1;35"},
    {"light cyan", "1;36"},
    {"white", "1;37"}
};

std::string unColor = "\033[0m";

colorMap parse_file(const std::string & filename) {
    std::ifstream ifs (filename , std::ifstream::in);
    std::string content((std::istreambuf_iterator<char>(ifs) ), (std::istreambuf_iterator<char>()));
    json::error_code ec;
    json::value jv = json::parse(content, ec);
    if(ec) {
        std::cout << "Parsing failed: " << ec.message() << std::endl;
    }

    colorMap colorMapping;
    auto const& obj = jv.get_object();
    if(! obj.empty()) {
        auto it = obj.begin();
        for(;;) {
            std::string keyString = it->key().to_string();
            // ToDo: Check if it->value() is of type string!
            std::string valueString = json::value_to<std::string>(it->value());
            boost::algorithm::to_lower(valueString);
            colorMapping.insert(std::make_pair(keyString, valueString));
            if(++it == obj.end())
                break;
        }
    }
    return colorMapping;
}

int main(int argc, char* argv[]) {

    std::string configFile;

    try {
        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("file,f", boost::program_options::value<std::string>(&configFile)->required(), "regex-to-color mapping as JSON file")
        ;

        boost::program_options::variables_map vm;   
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);    

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }
    }
    catch(std::exception& error) {
        std::cerr << "ERROR: " << error.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "ERROR: Unknown exception!\n";
    }

    boost::regex token;
    boost::smatch hits;
    std::string color, prefix, suffix, repl;
    colorMap colorMapping = parse_file(configFile);
    for (std::string line; std::getline(std::cin, line);) {
        for (const auto& kv : colorMapping) {
            token = boost::regex((boost::format("(%s)") % kv.first).str());
            color = kv.second;
            prefix = (boost::format("\033[%sm") % bashColors[color]).str();
            suffix = unColor;
            line = boost::regex_replace(line, token, [&](auto& match)->std::string{
                return (boost::format("%s%s%s") % prefix % match.str() % unColor).str();
            });
            
        }
        std::cout << line << std::endl;
    }

    return 0;
}
