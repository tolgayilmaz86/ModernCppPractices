#include "ISPSample.hpp"
#include "BadExample.hpp"
#include "GoodExample.hpp"
#include <iostream>
#include <memory>

// Use anonymous namespace to ensure internal linkage and avoid ODR violations
namespace {

void demonstrate_bad_worker() {
    std::cout << "\n=== BAD: Fat Worker Interface ===" << std::endl;
    std::cout << "Classes forced to implement irrelevant methods\n" << std::endl;

    isp_bad::Developer dev;
    std::cout << "  Developer (implements all 6 methods):" << std::endl;
    dev.work();
    dev.eat();
    
    std::cout << "\n  Developer calling manageTeam():" << std::endl;
    try {
        dev.manageTeam();  // Throws!
    } catch (const std::logic_error& e) {
        std::cout << "    [BAD] Exception: " << e.what() << std::endl;
    }

    std::cout << "\n  RobotWorker (forced to implement human methods):" << std::endl;
    isp_bad::RobotWorker robot;
    robot.work();
    try {
        robot.eat();
    } catch (const std::logic_error& e) {
        std::cout << "    [BAD] Exception: " << e.what() << std::endl;
    }
}

void demonstrate_bad_device() {
    std::cout << "\n=== BAD: Fat Device Interface ===" << std::endl;
    std::cout << "Simple printer forced to implement fax, scan, etc.\n" << std::endl;

    isp_bad::SimplePrinter printer;
    printer.print("document.pdf");

    std::cout << "\n  SimplePrinter calling scan():" << std::endl;
    try {
        printer.scan("document.pdf");
    } catch (const std::logic_error& e) {
        std::cout << "    [BAD] Exception: " << e.what() << std::endl;
    }
}

void demonstrate_good_workers() {
    std::cout << "\n=== GOOD: Segregated Worker Interfaces ===" << std::endl;
    std::cout << "Each class implements only relevant interfaces\n" << std::endl;

    isp_good::Developer dev;
    isp_good::Manager mgr;
    isp_good::RobotWorker robot;

    std::cout << "  Developer (IWorkable, IFeedable, IMeetingAttendee...):" << std::endl;
    dev.work();
    dev.eat();
    dev.attendMeeting();
    // dev.manageTeam() doesn't exist - won't even compile!

    std::cout << "\n  Manager (adds ITeamManager):" << std::endl;
    mgr.work();
    mgr.manageTeam();

    std::cout << "\n  RobotWorker (only IWorkable):" << std::endl;
    robot.work();
    // robot.eat() doesn't exist - won't compile!
}

void demonstrate_good_devices() {
    std::cout << "\n=== GOOD: Segregated Device Interfaces ===" << std::endl;
    std::cout << "Each device implements only relevant interfaces\n" << std::endl;

    isp_good::SimplePrinter printer;
    isp_good::SimpleScanner scanner;
    isp_good::MultiFunctionPrinter mfp;

    std::cout << "  SimplePrinter (IPrinter only):" << std::endl;
    isp_good::printDocument(printer, "simple_doc.pdf");

    std::cout << "\n  SimpleScanner (IScanner only):" << std::endl;
    scanner.scan();

    std::cout << "\n  MultiFunctionPrinter (IPrinter, IScanner, IFax, ICopier):" << std::endl;
    mfp.print("mfp_doc.pdf");
    mfp.scan();
    mfp.fax("urgent.pdf");
    mfp.copy(3);
}

void demonstrate_good_repository() {
    std::cout << "\n=== GOOD: Segregated Repository Interfaces ===" << std::endl;
    std::cout << "Read-only cache only implements IReadable\n" << std::endl;

    isp_good::ReadOnlyCache cache;
    isp_good::FullRepository repo;

    std::cout << "  ReadOnlyCache (IReadable only):" << std::endl;
    isp_good::readFromSource(cache, 1);

    std::cout << "\n  FullRepository (IReadable, IWritable, IBulkOperations, IBackupable):" << std::endl;
    repo.create("item1");
    repo.create("item2");
    isp_good::readFromSource(repo, 0);
    repo.update(0, "updated_item1");
    repo.bulkInsert({"bulk1", "bulk2", "bulk3"});
    repo.backup();
}

void demonstrate_isp_benefits() {
    std::cout << "\n=== Interface Segregation Benefits ===" << std::endl;

    std::cout << "\n  1. NO FORCED IMPLEMENTATIONS" << std::endl;
    std::cout << "     - Classes implement only what they need" << std::endl;
    std::cout << "     - No 'not supported' exceptions" << std::endl;

    std::cout << "\n  2. COMPILE-TIME SAFETY" << std::endl;
    std::cout << "     - Calling unsupported method won't compile" << std::endl;
    std::cout << "     - vs. runtime exceptions in fat interfaces" << std::endl;

    std::cout << "\n  3. EASIER TESTING" << std::endl;
    std::cout << "     - Mock only the interface you depend on" << std::endl;
    std::cout << "     - No need to stub unused methods" << std::endl;

    std::cout << "\n  4. BETTER DOCUMENTATION" << std::endl;
    std::cout << "     - Interface name describes capability" << std::endl;
    std::cout << "     - Easy to see what a class can do" << std::endl;

    std::cout << "\n  5. REDUCED COUPLING" << std::endl;
    std::cout << "     - Changes to one interface don't affect others" << std::endl;
    std::cout << "     - Clients depend only on what they use" << std::endl;
}

} // end anonymous namespace

#include "SampleRegistry.hpp"

void ISPSample::run() {
    std::cout << "Running Interface Segregation Principle Sample..." << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "ISP: No client should be forced to depend on" << std::endl;
    std::cout << "     methods it does not use." << std::endl;

    demonstrate_bad_worker();
    demonstrate_bad_device();
    demonstrate_good_workers();
    demonstrate_good_devices();
    demonstrate_good_repository();
    demonstrate_isp_benefits();

    std::cout << "\n=== Key Takeaways ===" << std::endl;
    std::cout << "1. Split fat interfaces into smaller, focused ones" << std::endl;
    std::cout << "2. Classes should implement only relevant interfaces" << std::endl;
    std::cout << "3. Prefer many small interfaces over one large interface" << std::endl;
    std::cout << "4. Clients should depend on minimal interfaces" << std::endl;

    std::cout << "\nInterface Segregation Principle demonstration completed!" << std::endl;
}

// Auto-register this sample
REGISTER_SAMPLE(ISPSample, "Interface Segregation Principle", 21);
