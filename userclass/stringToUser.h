#ifndef STRING_TO_USER
#define STRING_TO_USER
#include "user.h"
#include <memory>
#include <sstream>

std::shared_ptr<Self> string_to_self(const std::string & info){
  std::stringstream ss;
  ss.str(info);
  std::string username{""};
  std::string name{""};
  std::string surname{""};
  int id{0};
  std::string email{""};
  ss >> username >> name >> surname >> id >> email;
  
  std::shared_ptr<Self> self = make_shared<Self>(username,name,surname,id,email);
  
  
  return self;
}

shared_ptr<Contact> string_to_contact(const std::string & info, const double &
debt ){
  std::stringstream ss;
  ss.str(info);
  std::string username{""};
  std::string name{""};
  std::string surname{""};
  int id{0};
  ss >> username >> name >> surname >> id;

  std::shared_ptr<Contact> self = make_shared<Contact>(username,name,surname,id,debt);

}



#endif
