#include <iostream>
#include "Student.hpp"

Student::Student(std::string name, uint16_t group)
:	name(std::move(name)), group(group)
{
	std::cerr << "Создан студент " << this->name
	          << ", группа " << group << '\n';
}

Student::Student(Student &&rvalue)
:	name(std::move(rvalue.name))
,	group(rvalue.group)
,	subjects(std::move(rvalue.subjects))
,	len(rvalue.len)
,	cap(rvalue.cap)
,	average(rvalue.average)
,	d_grades(rvalue.d_grades)
{}

Student::~Student()
{
	if (this->name.empty())
		return;
	std::cerr << "Уничтожен студент " << this->name
	          << ", группа " << this->group << '\n';
}

const std::string &Student::get_name() const
{
	return this->name;
}

uint16_t Student::get_group() const
{
	return this->group;
}

float Student::get_average_grade() const
{
	return this->average;
}

bool Student::is_bad() const
{
	return this->d_grades != 0;
}

std::partial_ordering Student::operator<=>(const Student &other) const
{
	return this->average <=> other.average;
}

void Student::set_grade(std::string discipline, Grade grade)
{
	size_t i = this->len;
	while (i > 0) {
		auto &pair = this->subjects[--i];
		if (pair.first == discipline) {
			this->d_grades -= pair.second == Grade::D;
			this->d_grades += grade == Grade::D;

			this->average -= static_cast<float>(pair.second) / this->len;
			this->average += static_cast<float>(grade) / this->len;

			pair.second = grade;
			return;
		}
	}

	if (this->len == this->cap) {
		const auto new_cap = this->cap * 2 + 1;
		const auto new_ptr = new std::pair<std::string, Grade>[new_cap];
		for (size_t j = 0; j < this->len; ++j)
			new_ptr[j] = this->subjects[j];
		this->subjects.reset(new_ptr);
		this->cap = new_cap;
	}

	const auto new_sum = this->average * this->len + static_cast<float>(grade);
	this->subjects[this->len++] = { discipline, grade };
	this->average = new_sum / this->len;

	this->d_grades += grade == Grade::D;
}

void Student::erase_grade(const std::string &discipline)
{
	size_t i = this->len;
	while (i > 0) {
		auto &pair = this->subjects[--i];
		if (pair.first == discipline) {
			const auto grade = pair.second;
			for (size_t j = i + 1; j < this->len; ++j)
				this->subjects[j - 1] = this->subjects[j];
			const auto old_sum = this->average * this->len;
			this->d_grades -= grade == Grade::D;
			--this->len;
			this->average = this->len != 0 ?
				(old_sum - static_cast<float>(grade)) / this->len : 0;
			return;
		}
	}
}

void Student::operator+=(std::pair<std::string, Grade> discipline_grade)
{
	this->set_grade(std::move(discipline_grade.first),
	                discipline_grade.second);
}

void Student::operator-=(const std::string &discipline)
{
	this->erase_grade(discipline);
}
