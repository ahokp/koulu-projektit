/*
 * Name: Paul Ahokas
 * Student number: 283302
 * UserID: 1693
 * E-Mail: paul.ahokas@tuni.fi
 *
 */


#include "account.hh"
#include "utils.hh"
#include "instance.hh"
#include "course.hh"

Account::Account(std::string full_name, std::string email, int account_number):
    full_name_(full_name),
    last_name_(""),
    first_name_(""),
    email_(email),
    account_number_(account_number),
    credits_(0)
{
    std::vector<std::string> name = utils::split(full_name_, ' ');
    last_name_ = name.back();
    first_name_ = name.front();
}


void Account::print() const
{
    std::cout << account_number_ << ": "
              << first_name_ << " "
              << last_name_  << ", "
              << email_ << std::endl;
}

std::string Account::get_email()
{
    return email_;
}

int Account::get_acc_num()
{
    return account_number_;
}

void Account::sign_instance(Instance *s_instance)
{
    current_.push_back(s_instance);
    std::cout << SIGNED_UP << std::endl;
}

void Account::complete_course(Instance *c_instance, Course *c_course,int creds)
{
    auto it = std::find(current_.begin(), current_.end(), c_instance);

    // Checks if student has signed up for given instance
    if( it != current_.end()){

        // Checks if student has already completed given course
        if(std::find(completed_.begin(), completed_.end(),
                     c_course) == completed_.end()){

            completed_.push_back(c_course);

            int i = std::distance(current_.begin(), it);
            current_.erase(current_.begin() + i);

            credits_ += creds;

            std::cout << COMPLETED <<std::endl;
            return;
        }
    }
    std::cout << NO_SIGNUPS << std::endl;
}

void Account::print_study_state(bool completed_only)
{
    if(not completed_only){
        std::cout << "Current:" <<std::endl;

        for(auto c_instance : current_){

            Course* cur_course = c_instance->get_course();
            cur_course->print_info(false);
            std::cout << " " << c_instance->get_name() <<std::endl;
        }
        std::cout << "Completed:" <<std::endl;
    }

    for(auto course : completed_){
        course->print_info(true);
    }
    std::cout << "Total credits: " << credits_ << std::endl;
}


