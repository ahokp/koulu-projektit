/*
 * Name: Paul Ahokas
 * Student number: 283302
 * UserID: 1693
 * E-Mail: paul.ahokas@tuni.fi
 *
 */


#include "instance.hh"
#include "course.hh"
#include "utils.hh"

Instance::Instance(std::string name, Course* course):
name_(name), course_(course), date_(utils::today.get_day(),
       utils::today.get_month(), utils::today.get_year())
{

}

void Instance::print()
{
    std::cout << name_ <<std::endl;
    std::cout << INDENT << "Starting date: ";
    date_.print();
    std::cout << std::endl;
    std::cout << INDENT << "Amount of students: " << students_.size() <<std::endl;
}

void Instance::print_students()
{
    for(auto student : students_){
        std::cout << INDENT;
        student->print();
    }
}

bool Instance::is_named(std::string name)
{
    if(name_ == name){
        return true;
    }
    return false;
}

std::string Instance::get_name()
{
    return name_;
}

Course* Instance::get_course()
{
    return course_;
}

bool Instance::add_student(Account *n_student, Date today)
{
    for(auto student : students_){
        // Student has already registered
        if(student->get_acc_num() == n_student->get_acc_num()){
            std::cout << ALREADY_REGISTERED <<std::endl;
            return false;
        }
    }

    // Student is too late for sign up
    if(date_.operator<(today)){
        std::cout << LATE <<std::endl;
        return false;
    }
    // Student added to the instance
    students_.push_back(n_student);
    return true;
}

