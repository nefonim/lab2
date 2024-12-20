#include "Set.hpp"
#include "Student.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

static inline void ignore_newline(std::istream &stream)
{
	stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static char get_command()
{
	std::cout << ">>> ";
	char c;
	std::cin >> c;
	if (c != '\n')
		ignore_newline(std::cin);
	return c;
}

static std::ostream &operator<<(std::ostream &out, const Student &student)
{
	return out << student.get_name() << ", " << student.get_group() << ": "
		<< student.get_average_grade();
}

static std::string ask_name()
{
	std::cout << "Фамилия и инициалы: ";
	std::string result;
	std::getline(std::cin, result);
	return result;
}

static std::string ask_discipline()
{
	std::cout << "Предмет: ";
	std::string result;
	std::getline(std::cin, result);
	return result;
}

static Student ask_student()
{
	const auto name = ask_name();

	std::cout << "Группа: ";
	uint16_t group;
	std::cin >> group;
	ignore_newline(std::cin);

	auto result = Student(std::move(name), group);
	std::cout << "Действия:\n"
		"+ Добавить оценку по предмету\n"
		"- Удалить оценку по предмету\n";
	while (true) {
		const auto cmd = get_command();
		switch (cmd) {
		case '+': {
			const auto disc = ask_discipline();

			std::cout << "Оценка (2/3/4/5): ";
			int grade;
			std::cin >> grade;
			ignore_newline(std::cin);

			if (grade < 2 || grade > 5)
				break;
			result.set_grade(std::move(disc), static_cast<Grade>(grade));
			break;
		}
		case '-': {
			const auto disc = ask_discipline();
			result.erase_grade(disc);
			break;
		}
		default:
			return result;
		};
	}
}

int main()
{
	auto students = Set<Student>();
	std::cout << "Действия:\n"
		"+ Добавить нового студента\n"
		"- Удалить определённого студента\n"
		"@ Вывести всех студентов в порядке возрастания среднего балла\n"
		"? Вывести студентов, имеющих оценку 2\n";
	while (!std::cin.eof()) {
		const auto cmd = get_command();
		switch (cmd) {
		case '+': {
			students.insert(ask_student());
			break;
		}
		case '-': {
			const auto name = ask_name();
			const auto end = students.end();
			for (auto i = students.begin(); i != end; ) {
				auto prev_i = i;
				++i;
				if ((*prev_i).get_name() == name)
					prev_i.erase();
			}
			break;
		}
		case '@':
			for (const auto &student: students)
				std::cout << student << '\n';
			break;
		case '?': {
			auto empty = true;
			for (const auto &student: students) {
				if (!student.is_bad())
					continue;
				std::cout << student << '\n';
				empty = false;
			}
			if (empty)
				std::cout << "Плохих студентов не обнаружено.\n";
			break;
		}
		default:
			return 1;
		}
	}
	return 0;
}