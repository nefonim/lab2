#pragma once
#include <compare>
#include <cstdint>
#include <string>
#include <memory>
#include "Grade.hpp"

class Student {
	std::string name;
	uint16_t group;
	std::unique_ptr<std::pair<std::string, Grade>[]> subjects = nullptr;
	size_t len = 0;
	size_t cap = 0;
	float average = 0.0;

	size_t d_grades = 0;

public:
	Student(std::string name, uint16_t group);
	Student(Student &&);
	~Student();

	const std::string &get_name() const;
	uint16_t get_group() const;
	float get_average_grade() const;

	void set_grade(std::string discipline, Grade grade);
	void erase_grade(const std::string &discipline);

	void operator+=(std::pair<std::string, Grade> discipline_grade);
	void operator-=(const std::string &discipline);

	bool is_bad() const;
	std::partial_ordering operator<=>(const Student &other) const;
};
