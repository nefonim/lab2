#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char **argv)
{
	if (argc < 2)
		return 1;
	const auto filename = argv[1];

	auto file = std::ifstream(filename);
	if (!file.is_open())
		return 1;
	while (!file.eof()) {
		std::string line;
		std::getline(file, line);

		auto ss = std::stringstream(line);
		auto contains_number = false;
		while (!ss.eof()) {
			std::string numstring;
			ss >> numstring;

			const auto is_two_digit_num = numstring.size() == 2
				&& numstring[0] >= '1'
				&& numstring[0] <= '9'
				&& numstring[1] >= '0'
				&& numstring[1] <= '9';
			if (is_two_digit_num) {
				contains_number = true;
				break;
			}
		}

		if (contains_number)
			std::cout << line << '\n';
	}
	return 0;
}