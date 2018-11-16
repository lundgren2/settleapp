#include <iostream>
#include <vector>
#include <memory>
#include "user.h"

using namespace std;

int main()
{
  Self self("TestMe'", "Foo", "Bar", 5, "test@test.se");
  vector<shared_ptr<User>> nd;
  for (int i{0}; i < 10; ++i)
  {
    shared_ptr<User> baba = make_shared<Contact>("test", "maja", "hej", i, i);
    nd.push_back(baba);
  }

  self.insert_end(nd);
  self.refresh();

  cout << self.name() << " " << self.surname() << " " << self.email() << " " << self.total() << endl;

  self.push_back(make_shared<Contact>("fafa", "petrus", "elis", 100, 0));
  self.change_debt("fafa", 1000);

  self.refresh();

  cout << self.name() << " " << self.surname() << " " << self.email() << " " << self.total() << endl;
  for (auto &it : *self.get_debts())
  {
    shared_ptr<Contact> pt = dynamic_pointer_cast<Contact>(it);
    cout << pt->name() << " " << pt->surname() << " " << pt->debt() << endl;
  }
  self.change_debt("fafa", -5000);
  self.refresh();
  for (auto &ite : *self.get_debts())
  {
    shared_ptr<Contact> ptr = dynamic_pointer_cast<Contact>(ite);
    cout << ptr->name() << " " << ptr->surname() << " " << ptr->debt() << endl;
  }
  cout << self.name() << " " << self.surname() << " " << self.email() << " " << self.total() << endl;
  return 0;
}
