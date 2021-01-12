/* Class: Instance
 * ----------
 * TIE-02201/TIE-02207 SPRING 2020
 * ----------
 *
 * Name: Paul Ahokas
 * Student number: 283302
 * UserID: 1693
 * E-Mail: paul.ahokas@tuni.fi
 *
 *
 * Class that represent a single instance.
 *
 * Includes instance name, course which it belongs to, starting date
 * and list of students sign up for the instance
 * */
#ifndef INSTANCE_HH
#define INSTANCE_HH

#include "account.hh"
#include "date.hh"
#include "utils.hh"
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

// Forward-declaration of Course, so that instance can point to the course it belongs to.
class Course;

const std::string ALREADY_REGISTERED = "Error: Student has already registered on this course.";
const std::string LATE = "Error: Can't sign up on instance after the starting date.";
const std::string INDENT = "    ";

class Instance
{
public:
    /**
     * @brief Instance
     * @param name unique name for the instance, used as id
     * @param course points to the course it belongs to
     */
    Instance(std::string name, Course* course);

    /**
     * @brief print
     * Prints out information about instance
     */
    void print();

    /**
     * @brief print_students
     * Prints out infromation about signed up students
     */
    void print_students();

    /**
     * @brief is_named
     * @param name
     * @return true if given name is instance's name, false otherwise
     */
    bool is_named(std::string name);

    /**
     * @brief get_name
     * @return instance's name
     */
    std::string get_name();

    /**
     * @brief get_course
     * @return Course instance belongs to
     */
    Course* get_course();

    /**
     * @brief add_student
     * @param n_student
     * @param today
     * @return true if adding student to instance was successful, false otherwise
     * Adds student to instance, prints error msg if student was already registered
     * or late
     */
    bool add_student(Account* n_student, Date today);


private:
    //Name of the instance
    std::string name_;
    //Course in which instance belongs to
    Course* course_;
    //Starting date
    Date date_;
    //Students signed up for instance
    std::vector <Account*> students_;

};

#endif // INSTANCE_HH
