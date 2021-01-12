/* Class: Account
  * ----------
  * Name: Paul Ahokas
  * Student number: 283302
  * UserID: 1693
  * E-Mail: paul.ahokas@tuni.fi
  *
  * Defines a student or a staff account in the university system.
  *
  * Includes study history of the account.
  * */
#ifndef ACCOUNT_HH
#define ACCOUNT_HH

#include "date.hh"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

class Instance;
class Course;

const std::string NO_SIGNUPS = "No signups found on this instance.";
const std::string SIGNED_UP = "Signed up on the course instance.";
const std::string COMPLETED = "Course completed.";

class Account
{
public:
    /**
     * @brief Account constructor
     * @param full_name as "frontname lastname"
     * @param email
     * @param account_number
     */
    Account(std::string full_name, std::string email, int account_number);

    /**
     * @brief print account info on one line
     */
    void print() const;

    /**
     * @brief get_email
     * @return email linked to this account
     */
    std::string get_email();

    /**
     * @brief get_acc_num
     * @return account number linked to this account
     */
    int get_acc_num();

    /**
     * @brief sign_instance
     * @param s_instance
     * Signs account up to given instance
     */
    void sign_instance(Instance *s_instance);

    /**
     * @brief complete_course
     * @param c_instance
     * @param c_course
     * @param creds
     * Completes given instance, course gets added to completed courses,
     * gives credits upon completion. Prints error msg if given instance
     * was not found or if course was already completed
     */
    void complete_course(Instance *c_instance, Course *c_course,int creds);

    /**
     * @brief print_study_state
     * @param completed_only
     * Prints student's study state. If given parameter is true, prints
     * only completed courses and credits, otherwise also prints current
     * instances
     */
    void print_study_state(bool completed_only);


private:
    std::string full_name_;
    std::string last_name_;
    std::string first_name_;
    std::string email_;
    const int account_number_;
    std::vector<Instance*> current_;
    std::vector<Course*> completed_;
    int credits_;
};

#endif // ACCOUNT_HH
