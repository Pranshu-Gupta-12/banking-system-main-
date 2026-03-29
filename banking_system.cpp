// ============================================================
//  BANKING SYSTEM — Single File C++17
//  Compile: g++ -std=c++17 banking_system.cpp -o banking
//  Run:     ./banking
// ============================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <ctime>
#include <stdexcept>
#include <limits>

// ============================================================
//  ENUMS & STRUCTS
// ============================================================

enum class AccountType { SAVINGS, CHECKING };

struct Transaction {
    std::string type;
    double amount;
    double balance_after;
    std::time_t timestamp;
};

// ============================================================
//  BASE CLASS: Account
// ============================================================

class Account {
protected:
    std::string account_number;
    std::string owner_name;
    double balance;
    AccountType account_type;
    std::vector<Transaction> history;

public:
    Account(const std::string& acc_num, const std::string& name,
            double initial_balance, AccountType type)
        : account_number(acc_num), owner_name(name),
          balance(initial_balance), account_type(type) {
        if (initial_balance < 0)
            throw std::invalid_argument("Initial balance cannot be negative.");
        history.push_back({"OPEN", initial_balance, balance, std::time(nullptr)});
    }

    virtual ~Account() = default;

    virtual bool deposit(double amount) {
        if (amount <= 0) { std::cerr << "  [!] Deposit must be positive.\n"; return false; }
        balance += amount;
        history.push_back({"DEPOSIT", amount, balance, std::time(nullptr)});
        return true;
    }

    virtual bool withdraw(double amount) {
        if (amount <= 0) { std::cerr << "  [!] Withdrawal must be positive.\n"; return false; }
        if (amount > balance) { std::cerr << "  [!] Insufficient funds.\n"; return false; }
        balance -= amount;
        history.push_back({"WITHDRAW", amount, balance, std::time(nullptr)});
        return true;
    }

    virtual void display_info() const {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  Account : " << account_number << "\n"
                  << "  Owner   : " << owner_name << "\n"
                  << "  Type    : " << (account_type == AccountType::SAVINGS ? "Savings" : "Checking") << "\n"
                  << "  Balance : $" << balance << "\n";
    }

    void print_statement() const {
        std::cout << "\n--- Statement: " << account_number << " (" << owner_name << ") ---\n";
        std::cout << std::left << std::setw(10) << "Type"
                  << std::setw(12) << "Amount"
                  << std::setw(14) << "Balance After"
                  << "Date\n"
                  << std::string(50, '-') << "\n";
        for (const auto& t : history) {
            char buf[20];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&t.timestamp));
            std::cout << std::fixed << std::setprecision(2)
                      << std::left << std::setw(10) << t.type
                      << std::setw(12) << t.amount
                      << std::setw(14) << t.balance_after
                      << buf << "\n";
        }
    }

    std::string get_account_number() const { return account_number; }
    std::string get_owner_name()     const { return owner_name; }
    double      get_balance()        const { return balance; }
    AccountType get_type()           const { return account_type; }
};

// ============================================================
//  DERIVED CLASS: SavingsAccount
// ============================================================

class SavingsAccount : public Account {
private:
    double interest_rate;
    double min_balance;

public:
    SavingsAccount(const std::string& acc_num, const std::string& name,
                   double initial_balance, double rate = 0.04)
        : Account(acc_num, name, initial_balance, AccountType::SAVINGS),
          interest_rate(rate), min_balance(500.0) {}

    // Override: enforce minimum balance on withdrawal
    bool withdraw(double amount) override {
        if (balance - amount < min_balance) {
            std::cerr << "  [!] Cannot withdraw: balance would drop below minimum ($"
                      << min_balance << ").\n";
            return false;
        }
        return Account::withdraw(amount);
    }

    void apply_interest() {
        double interest = balance * interest_rate;
        deposit(interest);
        std::cout << "  Interest of $" << std::fixed << std::setprecision(2)
                  << interest << " applied.\n";
    }

    void display_info() const override {
        Account::display_info();
        std::cout << "  Rate    : " << (interest_rate * 100) << "%"
                  << "  | Min Bal: $" << min_balance << "\n";
    }
};

// ============================================================
//  BANK CLASS
// ============================================================

class Bank {
private:
    std::string bank_name;
    std::map<std::string, std::shared_ptr<Account>> accounts;
    int next_id;

    std::string generate_account_number() {
        std::ostringstream ss;
        ss << "ACC" << std::setw(5) << std::setfill('0') << next_id++;
        return ss.str();
    }

public:
    explicit Bank(const std::string& name) : bank_name(name), next_id(1001) {}

    std::string create_account(const std::string& name, double balance, AccountType type) {
        std::string num = generate_account_number();
        if (type == AccountType::SAVINGS)
            accounts[num] = std::make_shared<SavingsAccount>(num, name, balance);
        else
            accounts[num] = std::make_shared<Account>(num, name, balance, type);
        std::cout << "  [+] Account created: " << num << "\n";
        return num;
    }

    bool delete_account(const std::string& num) {
        if (!accounts.count(num)) { std::cerr << "  [!] Not found.\n"; return false; }
        accounts.erase(num);
        std::cout << "  [x] Account " << num << " deleted.\n";
        return true;
    }

    std::shared_ptr<Account> find(const std::string& num) {
        auto it = accounts.find(num);
        if (it == accounts.end()) { std::cerr << "  [!] Account " << num << " not found.\n"; return nullptr; }
        return it->second;
    }

    bool deposit(const std::string& num, double amt) {
        auto a = find(num); return a && a->deposit(amt);
    }

    bool withdraw(const std::string& num, double amt) {
        auto a = find(num); return a && a->withdraw(amt);
    }

    bool transfer(const std::string& from, const std::string& to, double amt) {
        auto src = find(from), dst = find(to);
        if (!src || !dst) return false;
        if (!src->withdraw(amt)) return false;
        dst->deposit(amt);
        std::cout << "  [$] Transferred $" << std::fixed << std::setprecision(2)
                  << amt << " from " << from << " to " << to << "\n";
        return true;
    }

    void list_all() const {
        std::cout << "\n=== " << bank_name << " | " << accounts.size() << " account(s) ===\n";
        for (const auto& [num, acc] : accounts) { acc->display_info(); std::cout << "\n"; }
    }

    void statement(const std::string& num) {
        auto a = find(num); if (a) a->print_statement();
    }

    void apply_interest_all() {
        for (auto& [num, acc] : accounts)
            if (acc->get_type() == AccountType::SAVINGS)
                std::dynamic_pointer_cast<SavingsAccount>(acc)->apply_interest();
    }
};

// ============================================================
//  MAIN — Interactive Menu
// ============================================================

int main() {
    Bank bank("National Bank of C++");

    auto prompt = [](const std::string& msg) {
        std::cout << msg;
    };

    while (true) {
        std::cout << "\n========== MENU ==========\n"
                  << " 1. Create Account\n"
                  << " 2. Deposit\n"
                  << " 3. Withdraw\n"
                  << " 4. Transfer\n"
                  << " 5. Account Statement\n"
                  << " 6. List All Accounts\n"
                  << " 7. Apply Interest\n"
                  << " 8. Delete Account\n"
                  << " 0. Exit\n"
                  << "Choice: ";

        int choice; std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 0) { std::cout << "Goodbye!\n"; break; }

        if (choice == 1) {
            std::string name; double bal; int t;
            prompt("Owner name: "); std::getline(std::cin, name);
            prompt("Initial balance: $"); std::cin >> bal;
            prompt("Type (0=Savings, 1=Checking): "); std::cin >> t;
            bank.create_account(name, bal, t == 0 ? AccountType::SAVINGS : AccountType::CHECKING);

        } else if (choice == 2) {
            std::string num; double amt;
            prompt("Account: "); std::cin >> num;
            prompt("Amount: $"); std::cin >> amt;
            bank.deposit(num, amt);

        } else if (choice == 3) {
            std::string num; double amt;
            prompt("Account: "); std::cin >> num;
            prompt("Amount: $"); std::cin >> amt;
            bank.withdraw(num, amt);

        } else if (choice == 4) {
            std::string from, to; double amt;
            prompt("From: "); std::cin >> from;
            prompt("To: ");   std::cin >> to;
            prompt("Amount: $"); std::cin >> amt;
            bank.transfer(from, to, amt);

        } else if (choice == 5) {
            std::string num;
            prompt("Account: "); std::cin >> num;
            bank.statement(num);

        } else if (choice == 6) {
            bank.list_all();

        } else if (choice == 7) {
            bank.apply_interest_all();

        } else if (choice == 8) {
            std::string num;
            prompt("Account: "); std::cin >> num;
            bank.delete_account(num);

        } else {
            std::cout << "  Invalid option.\n";
        }
    }
    return 0;
}
