#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace srp_bad {

// ============================================================================
// BAD EXAMPLE: Class with multiple responsibilities
// ============================================================================
// This User class violates SRP by handling:
// 1. User data management
// 2. Validation logic
// 3. Persistence (saving to file)
// 4. Email formatting
// 5. Logging

class User {
private:
    std::string name_;
    std::string email_;
    int age_;
    std::vector<std::string> logs_;

public:
    User(const std::string& name, const std::string& email, int age)
        : name_(name), email_(email), age_(age) {
        log("User created: " + name);
    }

    // Responsibility 1: Data access
    std::string getName() const { return name_; }
    std::string getEmail() const { return email_; }
    int getAge() const { return age_; }

    void setName(const std::string& name) {
        if (validateName(name)) {
            log("Name changed from " + name_ + " to " + name);
            name_ = name;
        }
    }

    void setEmail(const std::string& email) {
        if (validateEmail(email)) {
            log("Email changed from " + email_ + " to " + email);
            email_ = email;
        }
    }

    // Responsibility 2: Validation (should be separate)
    bool validateName(const std::string& name) {
        if (name.empty()) {
            log("Validation failed: name is empty");
            return false;
        }
        if (name.length() > 100) {
            log("Validation failed: name too long");
            return false;
        }
        return true;
    }

    bool validateEmail(const std::string& email) {
        if (email.find('@') == std::string::npos) {
            log("Validation failed: invalid email format");
            return false;
        }
        return true;
    }

    bool validateAge(int age) {
        return age >= 0 && age <= 150;
    }

    // Responsibility 3: Persistence (should be separate)
    bool saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            log("Failed to open file: " + filename);
            return false;
        }
        file << "Name: " << name_ << "\n";
        file << "Email: " << email_ << "\n";
        file << "Age: " << age_ << "\n";
        log("User saved to file: " + filename);
        return true;
    }

    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            log("Failed to open file: " + filename);
            return false;
        }
        // Simplified loading logic
        log("User loaded from file: " + filename);
        return true;
    }

    // Responsibility 4: Email formatting (should be separate)
    std::string formatWelcomeEmail() {
        return "Dear " + name_ + ",\n\n"
               "Welcome to our service!\n\n"
               "Your registered email is: " + email_ + "\n\n"
               "Best regards,\nThe Team";
    }

    std::string formatPasswordResetEmail() {
        return "Dear " + name_ + ",\n\n"
               "Click here to reset your password.\n\n"
               "Best regards,\nThe Team";
    }

    // Responsibility 5: Logging (should be separate)
    void log(const std::string& message) {
        logs_.push_back("[LOG] " + message);
        std::cout << "    [BAD] " << message << std::endl;
    }

    void printLogs() {
        for (const auto& entry : logs_) {
            std::cout << "    " << entry << std::endl;
        }
    }
};

// Problems with this design:
// 1. Hard to test - need file system for persistence tests
// 2. Hard to reuse - validation logic stuck in User class
// 3. Hard to maintain - changes to logging affect User class
// 4. Violates SRP - multiple reasons to change this class

} // namespace srp_bad
