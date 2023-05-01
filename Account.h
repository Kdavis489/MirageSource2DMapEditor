#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <string>
#include <vector>
class Account
{
public:
	std::string username;
	std::string password;
	std::vector<std::string> characters;

	void createCharacter(const std::string& name)
	{
		characters.push_back(name);
	}

	void deleteCharacter(const std::string& name)
	{
		characters.erase(std::remove(characters.begin(), characters.end(), name), characters.end());
	}
};

class AccountManager
{
public:
	std::vector<Account> accounts;

	void createAccount(const std::string& username)
	{
		accounts.push_back({ username, {} });
	}

	void deleteAccount(const std::string& username)
	{
		accounts.erase(std::remove_if(accounts.begin(), accounts.end(), [&](const Account& account) 
			{
				return account.username == username;
			}), accounts.end());
	}

	Account* getAccount(const std::string& username)
	{
		auto it = std::find_if(accounts.begin(), accounts.end(), [&](const Account& account)
			{
				return account.username == username;
			});
		if (it != accounts.end())
		{
			return &(*it);
		} 
		else
		{
			return nullptr;
		}
	}
};
#endif