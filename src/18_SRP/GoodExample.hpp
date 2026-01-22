#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <functional>

namespace srp_good {

// ============================================================================
// GOOD EXAMPLE: Each class has a single responsibility
// ============================================================================

// Responsibility 1: User data only
class User {
private:
    std::string name_;
    std::string email_;
    int age_;

public:
    User(const std::string& name, const std::string& email, int age)
        : name_(name), email_(email), age_(age) {}

    // Only data access - nothing else!
    std::string getName() const { return name_; }
    std::string getEmail() const { return email_; }
    int getAge() const { return age_; }

    void setName(const std::string& name) { name_ = name; }
    void setEmail(const std::string& email) { email_ = email; }
    void setAge(int age) { age_ = age; }
};

// Responsibility 2: Validation logic only
class UserValidator {
public:
    struct ValidationResult {
        bool valid;
        std::string message;
    };

    static ValidationResult validateName(const std::string& name) {
        if (name.empty()) {
            return {false, "Name cannot be empty"};
        }
        if (name.length() > 100) {
            return {false, "Name too long (max 100 characters)"};
        }
        return {true, "Valid"};
    }

    static ValidationResult validateEmail(const std::string& email) {
        if (email.find('@') == std::string::npos) {
            return {false, "Invalid email format (missing @)"};
        }
        if (email.find('.') == std::string::npos) {
            return {false, "Invalid email format (missing domain)"};
        }
        return {true, "Valid"};
    }

    static ValidationResult validateAge(int age) {
        if (age < 0) {
            return {false, "Age cannot be negative"};
        }
        if (age > 150) {
            return {false, "Age too high"};
        }
        return {true, "Valid"};
    }

    static ValidationResult validate(const User& user) {
        auto nameResult = validateName(user.getName());
        if (!nameResult.valid) return nameResult;

        auto emailResult = validateEmail(user.getEmail());
        if (!emailResult.valid) return emailResult;

        auto ageResult = validateAge(user.getAge());
        if (!ageResult.valid) return ageResult;

        return {true, "User is valid"};
    }
};

// Responsibility 3: Persistence only (abstract interface)
class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual bool save(const User& user) = 0;
    virtual std::unique_ptr<User> load(const std::string& id) = 0;
};

// Concrete implementation: File-based storage
class FileUserRepository : public IUserRepository {
private:
    std::string basePath_;

public:
    explicit FileUserRepository(const std::string& basePath = "./")
        : basePath_(basePath) {}

    bool save(const User& user) override {
        std::string filename = basePath_ + user.getEmail() + ".txt";
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        
        file << user.getName() << "\n";
        file << user.getEmail() << "\n";
        file << user.getAge() << "\n";
        return true;
    }

    std::unique_ptr<User> load(const std::string& id) override {
        std::string filename = basePath_ + id + ".txt";
        std::ifstream file(filename);
        if (!file.is_open()) return nullptr;
        
        std::string name, email;
        int age;
        std::getline(file, name);
        std::getline(file, email);
        file >> age;
        
        return std::make_unique<User>(name, email, age);
    }
};

// Concrete implementation: In-memory storage (great for testing!)
class InMemoryUserRepository : public IUserRepository {
private:
    std::vector<User> users_;

public:
    bool save(const User& user) override {
        users_.push_back(user);
        return true;
    }

    std::unique_ptr<User> load(const std::string& id) override {
        for (const auto& user : users_) {
            if (user.getEmail() == id) {
                return std::make_unique<User>(user);
            }
        }
        return nullptr;
    }
    
    size_t count() const { return users_.size(); }
};

// Responsibility 4: Email formatting only
class EmailTemplates {
public:
    static std::string welcomeEmail(const User& user) {
        return "Dear " + user.getName() + ",\n\n"
               "Welcome to our service!\n\n"
               "Your registered email is: " + user.getEmail() + "\n\n"
               "Best regards,\nThe Team";
    }

    static std::string passwordResetEmail(const User& user) {
        return "Dear " + user.getName() + ",\n\n"
               "Click here to reset your password.\n\n"
               "Best regards,\nThe Team";
    }

    static std::string promotionalEmail(const User& user, const std::string& offer) {
        return "Dear " + user.getName() + ",\n\n"
               "Special offer just for you: " + offer + "\n\n"
               "Best regards,\nThe Team";
    }
};

// Responsibility 5: Logging only (abstract interface)
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const std::string& message) = 0;
    virtual void error(const std::string& message) = 0;
};

class ConsoleLogger : public ILogger {
public:
    void log(const std::string& message) override {
        std::cout << "    [GOOD][LOG] " << message << std::endl;
    }
    void error(const std::string& message) override {
        std::cout << "    [GOOD][ERROR] " << message << std::endl;
    }
};

class SilentLogger : public ILogger {
public:
    void log(const std::string&) override {}
    void error(const std::string&) override {}
};

// Coordination: UserService combines responsibilities cleanly
class UserService {
private:
    std::unique_ptr<IUserRepository> repository_;
    std::unique_ptr<ILogger> logger_;

public:
    UserService(std::unique_ptr<IUserRepository> repo, 
                std::unique_ptr<ILogger> logger)
        : repository_(std::move(repo)), logger_(std::move(logger)) {}

    bool createUser(const std::string& name, const std::string& email, int age) {
        User user(name, email, age);
        
        auto validation = UserValidator::validate(user);
        if (!validation.valid) {
            logger_->error("Validation failed: " + validation.message);
            return false;
        }

        if (!repository_->save(user)) {
            logger_->error("Failed to save user");
            return false;
        }

        logger_->log("User created: " + name);
        return true;
    }

    std::string getWelcomeEmail(const std::string& email) {
        auto user = repository_->load(email);
        if (!user) {
            logger_->error("User not found: " + email);
            return "";
        }
        return EmailTemplates::welcomeEmail(*user);
    }
};

// Benefits of this design:
// 1. Easy to test - can mock repository, logger
// 2. Easy to reuse - validation works anywhere
// 3. Easy to maintain - changes are isolated
// 4. Follows SRP - each class has one reason to change

} // namespace srp_good
