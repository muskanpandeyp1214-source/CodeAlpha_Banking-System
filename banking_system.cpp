#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <limits>

using namespace std;

// Utility function to get current date-time string
string getCurrentDateTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
       << setw(2) << setfill('0') << ltm->tm_mday << " "
       << setw(2) << setfill('0') << ltm->tm_hour << ":"
       << setw(2) << setfill('0') << ltm->tm_min << ":"
       << setw(2) << setfill('0') << ltm->tm_sec;
    return ss.str();
}

// ------------------- Transaction Class -------------------
class Transaction {
public:
    int transactionId;
    string type;        // Deposit, Withdrawal, Transfer-In, Transfer-Out
    double amount;
    double balanceAfter;
    string dateTime;
    string details;

    Transaction(int id, string t, double amt, double bal, string det = "") {
        transactionId = id;
        type = t;
        amount = amt;
        balanceAfter = bal;
        details = det;
        dateTime = getCurrentDateTime();
    }

    void display() const {
        cout << left << setw(6) << transactionId
             << setw(16) << type
             << setw(12) << fixed << setprecision(2) << amount
             << setw(14) << balanceAfter
             << setw(22) << dateTime
             << details << endl;
    }
};

// ------------------- Account Class -------------------
class Account {
private:
    static int nextAccountNumber;
    static int nextTransactionId;

public:
    int accountNumber;
    int customerId;
    string accountType;   // Savings, Checking
    double balance;
    vector<Transaction> transactions;

    Account() {}

    Account(int custId, string type, double initialDeposit) {
        accountNumber = nextAccountNumber++;
        customerId = custId;
        accountType = type;
        balance = 0.0;
        if (initialDeposit > 0) {
            deposit(initialDeposit, "Initial Deposit");
        }
    }

    bool deposit(double amount, string details = "Cash Deposit") {
        if (amount <= 0) {
            cout << "Error: Deposit amount must be positive.\n";
            return false;
        }
        balance += amount;
        transactions.push_back(Transaction(nextTransactionId++, "Deposit", amount, balance, details));
        return true;
    }

    bool withdraw(double amount, string details = "Cash Withdrawal") {
        if (amount <= 0) {
            cout << "Error: Withdrawal amount must be positive.\n";
            return false;
        }
        if (amount > balance) {
            cout << "Error: Insufficient balance. Available: " << fixed << setprecision(2) << balance << endl;
            return false;
        }
        balance -= amount;
        transactions.push_back(Transaction(nextTransactionId++, "Withdrawal", amount, balance, details));
        return true;
    }

    void addTransferInRecord(double amount, int fromAccount) {
        balance += amount;
        stringstream ss;
        ss << "From Acc#" << fromAccount;
        transactions.push_back(Transaction(nextTransactionId++, "Transfer-In", amount, balance, ss.str()));
    }

    bool doTransferOut(double amount, int toAccount) {
        if (amount <= 0) {
            cout << "Error: Transfer amount must be positive.\n";
            return false;
        }
        if (amount > balance) {
            cout << "Error: Insufficient balance for transfer.\n";
            return false;
        }
        balance -= amount;
        stringstream ss;
        ss << "To Acc#" << toAccount;
        transactions.push_back(Transaction(nextTransactionId++, "Transfer-Out", amount, balance, ss.str()));
        return true;
    }

    void displayInfo() const {
        cout << "\n--------- Account Info ---------\n";
        cout << "Account Number : " << accountNumber << endl;
        cout << "Account Type   : " << accountType << endl;
        cout << "Customer ID    : " << customerId << endl;
        cout << "Balance        : $" << fixed << setprecision(2) << balance << endl;
        cout << "--------------------------------\n";
    }

    void displayRecentTransactions(int count = 5) const {
        cout << "\n--- Recent Transactions for Account #" << accountNumber << " ---\n";
        if (transactions.empty()) {
            cout << "No transactions yet.\n";
            return;
        }
        cout << left << setw(6) << "ID"
             << setw(16) << "Type"
             << setw(12) << "Amount"
             << setw(14) << "Balance"
             << setw(22) << "Date/Time"
             << "Details" << endl;
        cout << string(80, '-') << endl;

        int start = (int)transactions.size() - count;
        if (start < 0) start = 0;
        for (int i = (int)transactions.size() - 1; i >= start; i--) {
            transactions[i].display();
        }
    }
};

int Account::nextAccountNumber = 1001;
int Account::nextTransactionId = 1;

// ------------------- Customer Class -------------------
class Customer {
private:
    static int nextCustomerId;

public:
    int customerId;
    string name;
    string address;
    string phone;
    string email;
    vector<Account> accounts;

    Customer() {}

    Customer(string n, string addr, string ph, string em) {
        customerId = nextCustomerId++;
        name = n;
        address = addr;
        phone = ph;
        email = em;
    }

    Account* createAccount(string type, double initialDeposit) {
        Account acc(customerId, type, initialDeposit);
        accounts.push_back(acc);
        return &accounts.back();
    }

    Account* findAccount(int accountNumber) {
        for (auto &acc : accounts) {
            if (acc.accountNumber == accountNumber) return &acc;
        }
        return nullptr;
    }

    void displayInfo() const {
        cout << "\n--------- Customer Info ---------\n";
        cout << "Customer ID : " << customerId << endl;
        cout << "Name        : " << name << endl;
        cout << "Address     : " << address << endl;
        cout << "Phone       : " << phone << endl;
        cout << "Email       : " << email << endl;
        cout << "Accounts    : " << accounts.size() << endl;
        cout << "---------------------------------\n";
    }

    void listAccounts() const {
        if (accounts.empty()) {
            cout << "No accounts under this customer.\n";
            return;
        }
        cout << "\nAccounts of " << name << ":\n";
        cout << left << setw(15) << "AccountNo" << setw(15) << "Type" << "Balance" << endl;
        cout << string(45, '-') << endl;
        for (const auto &acc : accounts) {
            cout << left << setw(15) << acc.accountNumber
                 << setw(15) << acc.accountType
                 << "$" << fixed << setprecision(2) << acc.balance << endl;
        }
    }
};

int Customer::nextCustomerId = 1;

// ------------------- Bank Class -------------------
class Bank {
public:
    string bankName;
    vector<Customer> customers;

    Bank(string name) : bankName(name) {}

    Customer* createCustomer(string name, string addr, string phone, string email) {
        Customer c(name, addr, phone, email);
        customers.push_back(c);
        cout << "Customer created successfully. Customer ID: " << customers.back().customerId << endl;
        return &customers.back();
    }

    Customer* findCustomer(int customerId) {
        for (auto &c : customers) {
            if (c.customerId == customerId) return &c;
        }
        return nullptr;
    }

    Account* findAccountGlobal(int accountNumber, Customer** ownerOut = nullptr) {
        for (auto &c : customers) {
            for (auto &acc : c.accounts) {
                if (acc.accountNumber == accountNumber) {
                    if (ownerOut) *ownerOut = &c;
                    return &acc;
                }
            }
        }
        return nullptr;
    }

    bool transferFunds(int fromAcc, int toAcc, double amount) {
        if (fromAcc == toAcc) {
            cout << "Error: Cannot transfer to the same account.\n";
            return false;
        }
        Account* src = findAccountGlobal(fromAcc);
        Account* dst = findAccountGlobal(toAcc);
        if (!src) { cout << "Error: Source account not found.\n"; return false; }
        if (!dst) { cout << "Error: Destination account not found.\n"; return false; }

        if (!src->doTransferOut(amount, toAcc)) return false;
        dst->addTransferInRecord(amount, fromAcc);
        cout << "Transfer of $" << fixed << setprecision(2) << amount
             << " from Account #" << fromAcc << " to Account #" << toAcc << " successful.\n";
        return true;
    }

    void listCustomers() const {
        if (customers.empty()) {
            cout << "No customers in the bank.\n";
            return;
        }
        cout << "\n--- Customers of " << bankName << " ---\n";
        cout << left << setw(8) << "ID" << setw(20) << "Name" << setw(15) << "Phone" << "Accounts" << endl;
        cout << string(55, '-') << endl;
        for (const auto &c : customers) {
            cout << left << setw(8) << c.customerId
                 << setw(20) << c.name
                 << setw(15) << c.phone
                 << c.accounts.size() << endl;
        }
    }
};

// ------------------- Helpers for menu -------------------
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int readInt(const string &prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) { clearInput(); return val; }
        cout << "Invalid input, please enter a number.\n";
        clearInput();
    }
}

double readDouble(const string &prompt) {
    double val;
    while (true) {
        cout << prompt;
        if (cin >> val) { clearInput(); return val; }
        cout << "Invalid input, please enter a number.\n";
        clearInput();
    }
}

string readLine(const string &prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

// ------------------- Main Menu -------------------
int main() {
    Bank bank("My Bank");

    // Pre-load some demo data
    bank.customers.reserve(100);
    Customer* c1 = bank.createCustomer("Alice Johnson", "123 Maple St", "555-0101", "alice@example.com");
    c1->accounts.reserve(10);
    c1->createAccount("Savings", 1000.00);
    c1->createAccount("Checking", 500.00);
    Customer* c2 = bank.createCustomer("Bob Smith", "456 Oak Ave", "555-0202", "bob@example.com");
    c2->accounts.reserve(10);
    c2->createAccount("Savings", 2500.00);

    while (true) {
        cout << "\n========== " << bank.bankName << " ==========\n";
        cout << "1.  Create Customer\n";
        cout << "2.  Create Account\n";
        cout << "3.  Deposit\n";
        cout << "4.  Withdraw\n";
        cout << "5.  Transfer Funds\n";
        cout << "6.  View Account Info\n";
        cout << "7.  View Recent Transactions\n";
        cout << "8.  View Customer Info\n";
        cout << "9.  List Customer's Accounts\n";
        cout << "10. List All Customers\n";
        cout << "0.  Exit\n";
        int choice = readInt("Enter choice: ");

        if (choice == 0) {
            cout << "Thank you for banking with " << bank.bankName << ". Goodbye!\n";
            break;
        }
        else if (choice == 1) {
            string name = readLine("Name: ");
            string addr = readLine("Address: ");
            string phone = readLine("Phone: ");
            string email = readLine("Email: ");
            bank.createCustomer(name, addr, phone, email);
        }
        else if (choice == 2) {
            int cid = readInt("Customer ID: ");
            Customer* c = bank.findCustomer(cid);
            if (!c) { cout << "Customer not found.\n"; continue; }
            string type = readLine("Account Type (Savings/Checking): ");
            double dep = readDouble("Initial Deposit: ");
            Account* acc = c->createAccount(type, dep);
            cout << "Account created. Account Number: " << acc->accountNumber << endl;
        }
        else if (choice == 3) {
            int accNo = readInt("Account Number: ");
            Account* acc = bank.findAccountGlobal(accNo);
            if (!acc) { cout << "Account not found.\n"; continue; }
            double amt = readDouble("Deposit Amount: ");
            if (acc->deposit(amt)) {
                cout << "Deposit successful. New Balance: $"
                     << fixed << setprecision(2) << acc->balance << endl;
            }
        }
        else if (choice == 4) {
            int accNo = readInt("Account Number: ");
            Account* acc = bank.findAccountGlobal(accNo);
            if (!acc) { cout << "Account not found.\n"; continue; }
            double amt = readDouble("Withdrawal Amount: ");
            if (acc->withdraw(amt)) {
                cout << "Withdrawal successful. New Balance: $"
                     << fixed << setprecision(2) << acc->balance << endl;
            }
        }
        else if (choice == 5) {
            int from = readInt("From Account Number: ");
            int to = readInt("To Account Number: ");
            double amt = readDouble("Transfer Amount: ");
            bank.transferFunds(from, to, amt);
        }
        else if (choice == 6) {
            int accNo = readInt("Account Number: ");
            Account* acc = bank.findAccountGlobal(accNo);
            if (!acc) { cout << "Account not found.\n"; continue; }
            acc->displayInfo();
        }
        else if (choice == 7) {
            int accNo = readInt("Account Number: ");
            Account* acc = bank.findAccountGlobal(accNo);
            if (!acc) { cout << "Account not found.\n"; continue; }
            int n = readInt("How many recent transactions? ");
            acc->displayRecentTransactions(n);
        }
        else if (choice == 8) {
            int cid = readInt("Customer ID: ");
            Customer* c = bank.findCustomer(cid);
            if (!c) { cout << "Customer not found.\n"; continue; }
            c->displayInfo();
        }
        else if (choice == 9) {
            int cid = readInt("Customer ID: ");
            Customer* c = bank.findCustomer(cid);
            if (!c) { cout << "Customer not found.\n"; continue; }
            c->listAccounts();
        }
        else if (choice == 10) {
            bank.listCustomers();
        }
        else {
            cout << "Invalid choice, please try again.\n";
        }
    }

    return 0;
}
