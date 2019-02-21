#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <vector>
#include <string>



using namespace std;

enum type { primary, secondary };

class Database;


ifstream inputFileClients;
ifstream inputFileAccounts;
ofstream outputFileClients;
ofstream outputFileAccounts;


double getPositiveNumber()
{
	double res;
	do
	{
		cout << "Provide the number: ";
		cin >> res;
		if (res < 0)
			cout << "Number has to be positive" << endl;

	} while (res < 0);
	return res;
}

class Person
{
	string name;
	string surname;
	int id;
	static int nextId;
	double balance;

public:

	Person(string name, string surname, double balance)
	{
		this->name = name;
		this->surname = surname;
		this->balance = balance;
		id = nextId;
		nextId++;
	}
	Person(string name, string surname, double balance, int ID)
	{
		this->name = name;
		this->surname = surname;
		this->balance = balance;
		id = ID;
		if (ID >= nextId)
			nextId = ID;
	}
	int getId()
	{
		return id;
	}

	double getBalance()
	{
		return balance;
	}

	void addMoney(double amount)
	{
		balance += amount;
	}

	void takeMoney(double amount)
	{
		balance -= amount;
	}

	virtual void printPerson()
	{
		printf("%s %s with id: %d and balance %f\n", name.c_str(), surname.c_str(), id, balance);
	}

	virtual string getPerson()
	{
		return name + " " + surname + " " + to_string(id) + " " + to_string(balance);
	}

	void updateBalance()
	{
		double b;

		cout << "What is the new balance?: " << endl;
		b = getPositiveNumber();

		balance = b;
	}

};

int Person::nextId = 0;

class BankAccount
{

protected:

	double balance;
	int id;
	static int nextId;
	Person * owner;
	type t = primary;

public:

	BankAccount(Person * owner, double balance)
	{
		this->owner = owner;
		this->balance = balance;
		id = nextId;
		nextId++;
	}

	BankAccount(Person * owner, double balance, int id)
	{
		this->owner = owner;
		this->balance = balance;
		this->id = id;
		if (id >= nextId)
			nextId = id;
	}

	virtual void topUp(double amount)
	{

	}

	double getBalance()
	{
		return balance;
	}

	void takeMoney(double amount)
	{
		balance -= amount;
	}

	void addMoney(double amount)
	{
		balance += amount;
	}

	int getOwnersId()
	{
		return owner->getId();
	}

	type getType()
	{
		return t;
	}

	int getId()
	{
		return id;
	}

	virtual void withdraw(double amount)
	{

	}

	virtual void printAccountInfo()
	{
		cout << "Account of id: " << id << " and balance: " << balance << "   owner is person with id: " << owner->getId();
	}

	virtual string toString()
	{
		return "";
	}

};

int BankAccount::nextId = 0;

class CheckingAccount : public BankAccount
{


public:


	virtual string toString() override
	{
		return to_string(id) + " " + "-1 " + to_string(balance) + " " + to_string(owner->getId());
	}

	CheckingAccount(Person * owner, double balance)
		:BankAccount(owner, balance)
	{

	}

	CheckingAccount(Person *owner, double balance, int id)
		: BankAccount(owner, balance, id)
	{

	}

	virtual void withdraw(double amount) override
	{
		if (balance >= amount)
		{
			balance -= amount;
			owner->addMoney(amount);
		}
		else
		{
			cout << "There is not enough money to withdraw\n";
		}
	}

	virtual void topUp(double amount) override
	{
		if (owner->getBalance() >= amount)
		{
			balance += amount;
			owner->takeMoney(amount);
		}
		else
			cout << "Client doesn't have enough money" << endl;

	}

	virtual void printAccountInfo() override
	{
		BankAccount::printAccountInfo();
		cout << "   this is primary account" << endl;
	}


};

class SavingsAccount : public BankAccount
{

private:

	CheckingAccount * mainAccount;

public:

	SavingsAccount(Person * owner, double balance, CheckingAccount * mainacc)
		:BankAccount(owner, balance)
	{
		mainAccount = mainacc;
		t = secondary;
	}
	SavingsAccount(Person * owner, double balance, CheckingAccount * mainacc, int id)
		:BankAccount(owner, balance, id)
	{
		mainAccount = mainacc;
		t = secondary;
	}

	virtual string toString() override
	{
		return to_string(id) + " " + to_string(mainAccount->getId()) + " " + to_string(balance) + " " + to_string(owner->getId());
	}


	virtual void withdraw(double amount) override
	{
		if (balance >= amount)
		{
			balance -= amount;
			mainAccount->addMoney(amount);
		}
		else
		{
			cout << "There is not enough money to withdraw\n";
		}
	}

	virtual void topUp(double amount) override
	{
		if (mainAccount->getBalance() >= amount)
		{
			balance += amount;
			mainAccount->takeMoney(amount);
		}
		else
			cout << "There is not enough money on primary account, sorry" << endl;
	}

	virtual void printAccountInfo() override
	{
		BankAccount::printAccountInfo();
		cout << "   this is a savings account with primary account of id: " << mainAccount->getId() << endl;
	}


};

class Database
{
private:

	vector <BankAccount*> accounts;
	vector <CheckingAccount*> checkingAccounts;
	vector <SavingsAccount*> savingAccounts;
	vector <Person*> clients;


public:

	Database()
	{
		readFromFiles();
	}

	~Database()
	{
		saveToFiles();
		for (auto i : accounts)
			delete i;
		for (auto i : clients)
			delete i;
	}

	BankAccount * getAccountById(int id)
	{
		for (auto i : accounts)
			if (i->getId() == id)
				return i;

		return nullptr;
	}

	Person * getClientById(int id)
	{
		for (auto i : clients)
			if (i->getId() == id)
				return i;

		return nullptr;
	}

	CheckingAccount * getCheckingByOwnersId(int id)
	{
		for (auto i : checkingAccounts)
		{
			if (i->getOwnersId() == id)
				return i;
		}

		return nullptr;
	}

	CheckingAccount * getCheckingById(int id)
	{
		for (auto i : checkingAccounts)
			if (i->getId() == id)
				return i;

		return nullptr;
	}

	SavingsAccount * getSavingsByOwnersId(int id)
	{
		for (auto i : savingAccounts)
		{
			if (i->getOwnersId() == id)
				return i;
		}

		return nullptr;
	}

	SavingsAccount * getSavingsById(int id)
	{
		for (auto i : savingAccounts)
			if (i->getId() == id)
				return i;

		return nullptr;
	}


	int numberOfClients()
	{
		return clients.size();
	}

	void printAllAccounts()
	{
		cout << "Printing all accounts:\n";
		for (auto i : accounts)
			i->printAccountInfo();
	}

	void printAllClients()
	{
		cout << "Prining all clients: \n";
		for (auto i : clients)
			i->printPerson();
	}

	void printAllSavingsAccounts()
	{
		for (auto account : savingAccounts)
			account->printAccountInfo();
	}

	void printAllCheckingAccounts()
	{
		for (auto account : checkingAccounts)
			account->printAccountInfo();
	}

	void transfer(int idDest, int idSender, double amount)
	{
		BankAccount *sender = getAccountById(idSender);

		if (sender == nullptr)
		{
			cout << "There is no such bank account to send money from!" << endl;
			return;
		}

		BankAccount *destination = getAccountById(idDest);

		if (destination == nullptr)
		{
			cout << "There is no such bank account to send money to!" << endl;
			return;
		}

		if (sender->getBalance() >= amount)
		{
			sender->takeMoney(amount);
			destination->addMoney(amount);
		}
		else
		{
			cout << "Sender has not enought money to send it!" << endl;
		}

	}

	Person * hasClientOfId(int id)
	{

		for (auto client : clients)
		{
			if (client->getId() == id)
				return client;
		}
		return nullptr;
	}

	BankAccount * hasAccountOfId(int id)
	{

		for (auto account : accounts)
		{
			if (account->getId() == id)
				return account;
		}
		return nullptr;
	}

	int numberOfAccounts()
	{
		return accounts.size();
	}

	void addAccount()
	{
		cout << "Select id of person for whom you want to create account: ";

		int idPerson = (int)getPositiveNumber();
		Person * client = getClientById(idPerson);

		if (client == nullptr)
		{
			cout << "There is no client with such id" << endl;
			return;
		}

		cout << "Choose if you want to create checking or savings account, choose 1 or 2: ";
		char choice;
		cin >> choice;

		if (choice != '1' && choice != '2')
		{
			cout << "There is no such option!" << endl;
			return;
		}


		if (choice == '1')
		{
			for (auto i : checkingAccounts)
			{
				if (i->getOwnersId() == idPerson)
				{
					cout << "Client already has an account!" << endl;
					return;
				}
			}
		}

		bool hasChecking = false;

		if (choice == '2')
		{

			for (auto i : accounts)
			{
				if (i->getOwnersId() == idPerson && i->getType() == primary)
					hasChecking = true;

				if (i->getOwnersId() == idPerson && i->getType() == secondary)
				{
					cout << "Client already has a savings account!" << endl;
					return;
				}
			}
		}


		if (choice == '1')
		{

			cout << "Choose the amount of money from client to be the starting balance, you can choose 0 also: ";
			double amount = getPositiveNumber();

			if (amount > client->getBalance())
			{
				cout << "Client has not enough money to do that! Creating account with balance 0" << endl;
				amount = 0;
			}

			client->takeMoney(amount);

			auto * res = new CheckingAccount(client, amount);
			accounts.push_back(res);
			checkingAccounts.push_back(res);
			cout << "Checking account successfully created" << endl;
		}
		if (choice == '2')
		{
			if (!hasChecking)
			{
				cout << "You cannot create savings account for client without checking account" << endl;
				return;
			}
			cout << "Choose the amount of money from client to be the starting balance, you can choose 0 also: ";
			double amount = getPositiveNumber();

			if (amount > client->getBalance())
			{
				cout << "Client has not enough money to do that! Creating account with balance 0" << endl;
				amount = 0;
			}

			client->takeMoney(amount);
			auto * res = new SavingsAccount(client, amount, getCheckingByOwnersId(idPerson));
			accounts.push_back(res);
			savingAccounts.push_back(res);
		}

	}

	void removeSavingAccount(SavingsAccount * savs)
	{
		if (savs != nullptr)
		{
			savs->withdraw(savs->getBalance());

			int i = 0;
			for (auto account : savingAccounts)
			{
				if (account->getId() == savs->getId())
				{
					savingAccounts.erase(savingAccounts.begin() + i);
					break;
				}
				i++;
			}
			i = 0;
			for (auto account : accounts)
			{
				if (account->getId() == savs->getId())
				{
					accounts.erase(accounts.begin() + i);
					break;
				}
				i++;
			}

			delete savs;
		}
	}

	void removeCheckingAccount(CheckingAccount * target)
	{
		SavingsAccount *savs = getSavingsByOwnersId(target->getOwnersId());

		removeSavingAccount(savs);


		target->withdraw(target->getBalance());
		int i = 0;
		for (auto account : checkingAccounts)
		{
			if (account->getId() == target->getId())
			{
				checkingAccounts.erase(checkingAccounts.begin() + i);
			}
			i++;
		}
		i = 0;
		for (auto account : accounts)
		{
			if (account->getId() == target->getId())
			{
				accounts.erase(accounts.begin() + i);
				break;
			}
			i++;
		}

		delete target;
	}

	void removeAccount()
	{
		cout << "If you want to delete checking account corresponding savings account will be deleted too (if present).\n"
			"All money from both accounts will be transfered to owners pocket\n"
			"Choose the ID of account you want to delete: ";
		int choice = (int)getPositiveNumber();

		BankAccount *target = getAccountById(choice);

		if (target == nullptr)
		{
			cout << "There is no account of that ID." << endl;
			return;
		}

		if (target->getType() == primary)
		{
			SavingsAccount *savs = getSavingsByOwnersId(target->getOwnersId());

			removeSavingAccount(savs);

			CheckingAccount *ctarget = getCheckingById(target->getId());

			removeCheckingAccount(ctarget);

		}
		else
			if (target->getType() == secondary)
			{
				auto *savs = getSavingsById(choice);
				removeSavingAccount(savs);
			}
			else
			{

			}

	}

	void addClient()
	{
		string name;
		string surname;
		double balance;

		cout << "Choose name for client: ";
		cin >> name;
		cout << "Choose surname for client: ";
		cin >> surname;
		cout << "Choose balance of client: ";

		balance = getPositiveNumber();

		clients.push_back(new Person(name, surname, balance));

	}

	void removeClient()
	{
		cout << "Provide id of client you want to delete: ";
		int id = (int)getPositiveNumber();

		Person *target = getClientById(id);

		if (target == nullptr)
		{
			cout << "There is no such client" << endl;
			return;
		}

		cout << "Client will be deleted along with all his accounts" << endl;

		removeCheckingAccount(getCheckingByOwnersId(id));

		int i = 0;
		for (auto client : clients)
		{
			if (client->getId() == id)
			{
				clients.erase(clients.begin() + i);
				delete client;
			}
			i++;
		}


	}

	void readFromFiles()
	{
		inputFileClients.open("clients.txt");
		inputFileAccounts.open("accounts.txt");

		if (!inputFileAccounts || !inputFileClients)
			throw exception();


		string name, surname;
		int idClient, id, originId;
		double balance;

		while (inputFileClients >> name >> surname >> idClient >> balance)
		{
			clients.push_back(new Person(name, surname, balance));
		}
		while (inputFileAccounts >> id >> originId >> balance >> idClient)
		{
			if (originId < 0)
			{
				CheckingAccount * target = new CheckingAccount(getClientById(idClient), balance, id);
				checkingAccounts.push_back(target);
				accounts.push_back(target);
			}
			else
			{
				SavingsAccount * target = new SavingsAccount(getClientById(idClient), balance, getCheckingByOwnersId(idClient), id);
				savingAccounts.push_back(target);
				accounts.push_back(target);
			}
		}


		inputFileClients.close();
		inputFileAccounts.close();
	}

	void saveToFiles()
	{
		outputFileAccounts.open("accounts.txt");
		outputFileClients.open("clients.txt");

		for (auto client : clients)
		{
			outputFileClients << client->getPerson() << endl;
		}
		for (auto account : accounts)
		{
			outputFileAccounts << account->toString() << endl;
		}


		outputFileAccounts.close();
		outputFileClients.close();
	}

};

void printMenu()
{
	cout << "//////////////////////MENU//////////////////////" << endl;
	cout << "1. Print whole database" << endl;
	cout << "2. Print all clients" << endl;
	cout << "3. Print all saving accounts" << endl;
	cout << "4. Print all checking acounts" << endl;
	cout << "5. Update client balance" << endl;
	cout << "6. Add client to database" << endl;
	cout << "7. Add account to database" << endl;
	cout << "8. Make transfer" << endl;
	cout << "9. Delete client from database" << endl;
	cout << "10. Delete account from database" << endl;
	cout << "11. Print menu once more" << endl;
	cout << "12. Leave the program" << endl;
	cout << "//////////////////////MENU//////////////////////" << endl << endl;
}

void handleOptions(Database & db)
{
	printMenu();
	int choice = -1;
	do
	{
		cout << "Choose option from menu: ";
		cin >> choice;
		if (choice > 13 || choice < 1)
		{
			cout << "This is not a valid option" << endl;
			continue;
		}

		switch (choice)
		{
		case 1:
		{
			db.printAllClients();
			db.printAllAccounts();
			break;
		}
		case 2:
		{
			db.printAllClients();
			break;
		}
		case 3:
		{
			db.printAllSavingsAccounts();
			break;
		}
		case 4:
		{
			db.printAllCheckingAccounts();
			break;
		}
		case 5:
		{
			if (db.numberOfClients() > 0)
			{
				db.printAllClients();
				cout << "Choose id: ";
				int idChoice;
				cin >> idChoice;
				auto target = db.hasClientOfId(idChoice);
				if (target)
					target->updateBalance();
				else
					cout << "There is no such client" << endl;
			}
			else
			{
				cout << "There are no clients in database" << endl;
			}
			break;
		}
		case 6:
		{
			db.addClient();
			break;
		}
		case 7:
		{
			db.addAccount();
			break;
		}
		case 8:
		{
			db.printAllAccounts();

			int from, to;
			double amount;
			cout << "Select id of account from which u will transfer money" << endl;
			from = (int)getPositiveNumber();

			if (db.hasAccountOfId(from) == nullptr)
			{
				cout << "There is no account of that id";
				break;
			}

			cout << "Select id of account to which u will transfer money" << endl;
			to = (int)getPositiveNumber();

			if (db.hasAccountOfId(to) == nullptr)
			{
				cout << "There is no account of that id";
				break;
			}

			cout << "Select amount of money you want to transfer: " << endl;
			amount = getPositiveNumber();

			if (db.hasAccountOfId(from)->getBalance() < amount)
			{
				cout << "Not enough money on account to transfer" << endl;
				break;
			}

			db.transfer(to, from, amount);
			break;
		}
		case 9:
		{
			if (db.numberOfClients() > 0)
			{
				db.printAllClients();
				db.removeClient();
			}
			else
			{
				cout << "There are no clients in database" << endl;
			}
			break;
		}
		case 10:
		{
			if (db.numberOfAccounts() > 0)
			{
				db.printAllAccounts();
				db.removeAccount();
			}
			else
			{
				cout << "There are no accounts in database" << endl;
			}
			break;
		}
		case 11:
		{
			printMenu();
			break;
		}
		case 12:
		{
			break;
		}
		default:
		{
			break;
		}

		}

	} while (choice != 12);
}


int main()
{


	//TESTY


	try
	{
		Database db;

		handleOptions(db);
	}

	catch (...)
	{
		cerr << "Files didn't open\n";
		outputFileClients.open("clients.txt");
		outputFileAccounts.open("accounts.txt");
		outputFileClients.close();
		outputFileAccounts.close();

		Database db;

		handleOptions(db);

	}


	return 0;
}