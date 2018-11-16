

#include <iostream>
#include <vector>
#include <memory>
#include "user.h"

using namespace std;

int main()
{
  Self self("TestMe", "Marwe", "Sävenhed", 5, "test@fest.com");

  shared_ptr<Contact> min_tobbe = make_shared<Contact>("toblu", "Tobias", "Lundgren", 2, -75);
  shared_ptr<Contact> hans_tobbe_min = make_shared<Contact>("BigPimpin", "Tobias", "Lundgren", 2, 75);
  //min_tobbe->push_back(min_tobbe_min->clone());

  shared_ptr<Contact> hans_tobbe = make_shared<Contact>("toblu", "Tobias", "Lundgren", 2, -50);
  hans_tobbe->push_back(hans_tobbe_min->clone());

  shared_ptr<Contact> hans = make_shared<Contact>("elire", "Elias", "Resman", 18, 0);

  shared_ptr<Contact> hans_min = make_shared<Contact>("BigPimpin", "Tobias", "Lundgren", 2, 0);

  cout << "hejtri" << endl;
  hans_tobbe->push_back(hans_tobbe_min->clone());
  min_tobbe->push_back(hans_tobbe_min->clone());

  hans->push_back(hans_min->clone());

  hans->push_back(hans_tobbe->clone());
  cout << "1 " << hans->size() << endl;
  self.push_back(min_tobbe->clone());
  cout << "2 " << self.size() << endl;
  self.push_back(hans->clone());
  cout << "3 " << self.size() << endl;

  cout << "\n\nmina: " << endl;
  shared_ptr<vector<shared_ptr<User>>> minnna = self.get_debts();

  for (auto &hannes : *minnna)
  {
    shared_ptr<Contact> huun = dynamic_pointer_cast<Contact>(hannes);
    if (huun)
      cout << huun->username() << ": " << huun->debt() << endl;
  }

  cout << "\n\nelire: " << endl;
  shared_ptr<vector<shared_ptr<User>>> haaans = self.get_hans_debts("elire");
  for (auto &hannes : *haaans)
  {
    shared_ptr<Contact> huun = dynamic_pointer_cast<Contact>(hannes);
    if (huun)
      cout << huun->username() << huun->debt() << endl;
  }
  cout << "\n\n";
  self.change_debt("elire", -100);
  cout << "change debt, bro\n";

  cout << "\n\nmina: " << endl;
  shared_ptr<vector<shared_ptr<User>>> minnna2 = self.get_debts();
  //cout << minnna->size() << endl;
  for (auto &hannes : *minnna2)
  {
    //cout << "inne i mina\n";
    shared_ptr<Contact> huun = dynamic_pointer_cast<Contact>(hannes);
    if (huun)
      cout << huun->username() << ": " << huun->debt() << endl;
  }

  cout << "\n\nelire: " << endl;
  shared_ptr<vector<shared_ptr<User>>> haaans2 = self.get_hans_debts("elire");
  for (auto &hannes : *haaans2)
  {
    shared_ptr<Contact> huun = dynamic_pointer_cast<Contact>(hannes);
    if (huun)
      cout << huun->username() << huun->debt() << endl;
  }

  shared_ptr<vector<shared_ptr<User>>> minnnna = self.get_update();
  for (auto &x : *minnnna)
  {
    cout << "USER: " << x->username() << endl;
  }

  return 0;
}
