#ifndef USER_CLASS
#define USER_CLASS
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
using namespace std;

class User;
class Contact;
class Self;
class Not_Complete;

class User
{
public:
  User(const string &_username, const string &_name, const string &_surname, const int &_id) : username_{_username}, name_{_name}, surname_{_surname}, id_{_id},
                                                                                               debts{std::vector<std::shared_ptr<User>>()} {};

  virtual ~User() = default;

  string name() const { return name_; }
  int Id() { return id_; }
  string surname() const { return surname_; }
  string username() const { return username_; }
  shared_ptr<vector<shared_ptr<User>>> get_debts();
  shared_ptr<vector<shared_ptr<User>>> get_his_debts(const string &_username) const;
  std::string debts_to_str();

  void insert_end(const vector<shared_ptr<User>> &new_debts);
  void push_back(shared_ptr<User> const &user);
  int size() { return debts.size(); }

  virtual bool change_debt(const string &_username, const double &debt) = 0;
  virtual shared_ptr<User> clone() = 0;

private:
  string username_;
  string name_;
  string surname_;
  int id_;

protected:
  vector<shared_ptr<User>> debts;
};

bool operator==(const User &left, const User &right)
{
  return left.username() == right.username();
};

void User::insert_end(vector<shared_ptr<User>> const &new_debts)
{
  debts.insert(debts.end(), new_debts.begin(), new_debts.end());
  std::sort(debts.begin(), debts.end(), [](const shared_ptr<User> &left, const shared_ptr<User> &right) -> bool {
    return left->name() < right->name();
  });
}

void User::push_back(shared_ptr<User> const &user)
{
  debts.push_back(user->clone());
  std::sort(debts.begin(), debts.end(), [](const shared_ptr<User> &left, const shared_ptr<User> &right) -> bool {
    return left->name() < right->name();
  });
}

shared_ptr<vector<shared_ptr<User>>> User::get_debts()
{
  shared_ptr<vector<shared_ptr<User>>> tmp = make_shared<vector<shared_ptr<User>>>();
  for (auto &d : debts)
  {
    tmp->push_back(d->clone());
  }
  return tmp;
}

shared_ptr<vector<shared_ptr<User>>> User::get_his_debts(const string &_username) const
{
  if (!debts.empty())
  {
    for (auto &cont : debts)
    {
      if (_username == cont->username())
      {
        return cont->get_debts();
      }
    }
  }
  shared_ptr<vector<shared_ptr<User>>> fr;
  return fr;
}

class Not_Complete : public User, public std::enable_shared_from_this<Not_Complete>
{
public:
  Not_Complete() = default;
  Not_Complete(const string &_username, const string &_name, const string &_surname, const double &_debt) : User(_username, _name, _surname, -1), debt_{_debt} {}
  ~Not_Complete() = default;

  virtual bool change_debt(const string &str, const double &_debt) final;
  virtual shared_ptr<User> clone() final;
  shared_ptr<User> login(const string &_username, const string &passw);
  shared_ptr<User> create(const string &_username, const string &_name, const string &surname, const string &email);
  double debt() { return debt_; }

private:
  double debt_;
};

bool Not_Complete::change_debt(const string &str, const double &_debt)
{
  debt_ += _debt;
  return true;
}

shared_ptr<User> Not_Complete::clone()
{
  return this->shared_from_this();
}

shared_ptr<User> Not_Complete::login(const string &_username, const string &passw)
{
  return make_shared<Not_Complete>("notC", "notC", "notC", -1);
}
shared_ptr<User> Not_Complete::create(const string &_username, const string &_name, const string &surname, const string &email)
{
  return make_shared<Not_Complete>("notC", "notC", "notC", -1);
}

class Contact : public User, public std::enable_shared_from_this<Contact>
{
public:
  Contact(const string &_username, const string &_name,
          const string &_surname, const int &_id, const double &_debt) : User(_username, _name, _surname, _id), debt_{_debt}
  {
  }

  virtual ~Contact() = default;

  double debt() { return debt_; }
  virtual bool change_debt(const string &_username, const double &_debt) final;
  virtual shared_ptr<User> clone() final;

private:
  double debt_;
};

bool Contact::change_debt(const string &_username, const double &_debt)
{
  if (_username != "" && debts.size() > 0)
  {
    debt_ += _debt;
    double deb = _debt;
    for (auto &d : debts)
    {
      shared_ptr<Contact> cnt = std::dynamic_pointer_cast<Contact>(d);
      if (cnt)
      {
        if (cnt->username() == _username)
        {
          if (deb < 0)
          {
            cnt->change_debt("", abs(deb));
          }
          if (deb > 0)
          {
            cnt->change_debt("", (deb - (deb * 2)));
          }
        }
      }
    }
  }
  else
  {
    debt_ += _debt;
  }
  return true;
}

shared_ptr<User> Contact::clone()
{
  return this->shared_from_this();
}

class Self : public User, public std::enable_shared_from_this<Self>
{
public:
  Self(const string &_username, const string &_name,
       const string &_surname, const int &_id, const string &_email) : User(_username, _name, _surname, _id), email_{_email}, total_debt{0},
                                                                       update_list{std::vector<std::shared_ptr<User>>()}
  {
  }
  virtual ~Self() = default;

  string email() const { return email_; }
  double total() const { return total_debt; }
  void deleteContact(const std::string &);

  bool refresh();

  shared_ptr<vector<shared_ptr<User>>> get_update();

  virtual bool change_debt(const string &_username, const double &debt) final;
  virtual shared_ptr<User> clone() final;

private:
  string email_;
  double total_debt;
  vector<shared_ptr<User>> update_list;
};

void Self::deleteContact(const std::string &user)
{
  for (auto &c : debts)
  {
    std::shared_ptr<Contact> cnt = std::dynamic_pointer_cast<Contact>(c);
    std::shared_ptr<Not_Complete> nc = std::dynamic_pointer_cast<Not_Complete>(c);
    if (cnt && user == cnt->username())
    {
      if (cnt->debt() == 0)
        debts.erase(std::find(debts.begin(), debts.end(), cnt));
    }
    if (nc && user == nc->username())
    {
      if (nc->debt() == 0)
        debts.erase(std::find(debts.begin(), debts.end(), nc));
    }
  }
}

shared_ptr<User> Self::clone()
{
  return this->shared_from_this();
}

shared_ptr<vector<shared_ptr<User>>> Self::get_update()
{
  shared_ptr<vector<shared_ptr<User>>> tmp = make_shared<vector<shared_ptr<User>>>();
  for (auto &d : update_list)
  {
    tmp->push_back(d->clone());
  }
  update_list.empty();
  return tmp;
}

bool Self::refresh()
{
  total_debt = 0;
  for (auto &user : debts)
  {
    shared_ptr<Contact> cont = std::dynamic_pointer_cast<Contact>(user);
    if (cont)
    {
      total_debt += cont->debt();
    }
    shared_ptr<Not_Complete> nc = std::dynamic_pointer_cast<Not_Complete>(user);
    if (nc)
    {
      total_debt += nc->debt();
    }
  }
  return true;
}

bool Self::change_debt(const string &_username, const double &debt)
{
  for (auto &d : debts)
  {
    shared_ptr<Contact> cnt = std::dynamic_pointer_cast<Contact>(d);
    shared_ptr<Not_Complete> nc = std::dynamic_pointer_cast<Not_Complete>(d);

    if (cnt)
    {
      if (cnt->username() == _username)
      {
        cnt->change_debt(this->username(), debt);
        update_list.push_back(cnt->clone());
        return true;
      }
    }
    else if (nc)
    {
      if (nc->username() == _username)
      {
        nc->change_debt("", debt);
        return true;
      }
    }
  }

  // TODO: Implement following code to settle up debts by multiple peers
  /*
     for(auto & user: debts){

     shared_ptr<Contact> cont = std::dynamic_pointer_cast<Contact>(user);
     shared_ptr<Not_Complete> nc =
     std::dynamic_pointer_cast<Not_Complete>(user);
     if(nc){
     if(nc->username() == _username){
     return cont->change_debt("", debt);
  //update_list.push_back(user); // not complete behöver inte uppdateras
  }
  }
  if(cont){
  if(cont->username() == _username){ //kontakten vars skuld ska ändras
  vector<shared_ptr<User> > common_debts{};
  shared_ptr<Contact> his_me;
  shared_ptr<vector<shared_ptr<User> > > gd1 = cont->get_debts();
  for(auto& cont_debt: *gd1){ //kontaktens kontakter
  shared_ptr<Contact> cnt =
  std::dynamic_pointer_cast<Contact>(cont_debt);//ska
  if(cnt){
  if(cnt->username() == username()){
  his_me = std::dynamic_pointer_cast<Contact>(cnt);
  }
  shared_ptr<vector<shared_ptr<User> > > gd2 = cnt->get_debts();
  for(auto& find_self: *gd2){ //om någon av dessa har
  //skuld till användaren
  shared_ptr<Contact> me =
  std::dynamic_pointer_cast<Contact>(find_self);
  if(me){
  if(me->username() == username()){
  common_debts.push_back(cnt->clone());
  }
  }
  }

  }
  } // Lös skulder mellan *this, cont och common_debts här:

   *
   * Se om både *this och cont har negativ/positiv skuld till
   samma kontakt

   * Om den skulden är större eller lika med den skuld som ska
   läggas till, sätt hela skulden till denne kontakt

   * Annars ska den skulden lösas och en ny kontakt ska letas efter

   * upprepa tills inget är kvar av summan eller tills det inte finns
   några kontakter kvar

   * ändra cont:s skuld med det som är kvar av skulden
   /
  //cont->change_debt("", debt);
  //update_list.push_back(user);
  //return true;
  if(common_debts.empty()){
  cont->change_debt(username(), debt);
  //update_list.push_back(user);
  update_list.push_back(cont->clone());
  return true;
  }else{
  double rest{debt};
  for(auto & his_common: common_debts){
  for(auto & my_common: debts){
  shared_ptr<Contact> his_com =
  std::dynamic_pointer_cast<Contact>(his_common);
  shared_ptr<Contact> my_com =
  std::dynamic_pointer_cast<Contact>(my_common);

  if(my_com->username() == his_com->username()){
  if((my_com->debt()<0) && (his_com->debt() <0)){
    //hittat en där båda är skyldiga
    if(rest <0){// - på mina pengar
      if(rest >= his_com->debt()){//om mindre än hans totala skuld
        //då den är negativ
        his_com->change_debt(this->username(), abs(rest));
        my_com->change_debt(this->username(), rest);
        rest += his_com->debt();
        user->change_debt(this->username(), rest);
        //update_list.push_back(his_com);
        update_list.push_back(my_com->clone());
        update_list.push_back(user->clone());
        cout << "rest >= his_com\n";
        return true;

      }else{//om mer än hans skuld -''-
        rest -= his_com->debt(); //hans är
        //negativ och är mindre än debt
        cout << "his_com: debt: " <<
          his_com->debt() << endl;
        my_com->change_debt(this->username(),his_com->debt());

        his_com->change_debt(this->username(),abs(his_com->debt()));

        //update_list.push_back(his_com);
        update_list.push_back(my_com->clone());
        cout << "his/my_com: " <<
          his_com->username() << "  " <<
          my_com->username() << endl;
      }
    }else{//       + på mina pengar

    }
  }else if((my_com->debt()>0 && his_com->debt()>0) ){
    //hittat en där båda har positiv skuld
    if(debt <0){// - på mina pengar

    }else{//       + på mina pengar

    }
  }
}
}
}
if(his_me) 
  cout << "if his_me\n";
  his_me->change_debt(this->username(), rest);
  user->change_debt(this->username(), rest);
  cout << "cont: " << cont->username() << endl;
  //update_list.push_back(his_me);
  update_list.push_back(user->clone());
  return true;
  }
}

}
}*/
  return false;
}

std::string User::debts_to_str()
{
  shared_ptr<vector<shared_ptr<User>>> tmp = this->get_debts();
  std::string str{""};

  for (auto &debt : *tmp)
  {
    shared_ptr<Contact> cnt = std::dynamic_pointer_cast<Contact>(debt);
    shared_ptr<Not_Complete> nc = std::dynamic_pointer_cast<Not_Complete>(debt);
    if (nc)
    {
      str += nc->name() + "," + nc->surname() + "," + std::to_string(nc->debt()) + ":";
    }
    if (cnt)
      str += std::to_string(cnt->Id()) + "," + std::to_string(cnt->debt()) + ":";
  }

  return str;
}

#endif
