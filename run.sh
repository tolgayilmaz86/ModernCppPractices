#!/bin/bash

# Modern C++ Practices Runner Script
# Usage: ./run <sample_number>
# Examples: ./run 1, ./run 01, ./run RAII

# Function to display usage
show_usage() {
    echo "Usage: $0 <sample_number>"
    echo "Available samples:"
    echo "  1 or 01: RAII - Resource Acquisition Is Initialization"
    echo "  2 or 02: SFINAE - Substitution Failure Is Not An Error"
    echo "  3 or 03: CRTP - Curiously Recurring Template Pattern"
    echo "  4 or 04: PIMPL - Pointer to Implementation"
    echo "  5 or 05: RuleOfFive - Rule of Five"
    echo "  6 or 06: TypeErasure - Type Erasure"
    echo "  7 or 07: VariantVisitor - Variant and Visitor pattern"
    echo "  8 or 08: SmartPointers - Smart Pointers usage"
    echo "  9 or 09: ExceptionSafety - Exception Safety guarantees"
    echo "  10: MoveSemantics - Move Semantics"
    echo ""
    echo "You can also use the sample name directly (case-insensitive):"
    echo "  ./run RAII"
    exit 1
}

# Check if argument is provided
if [ $# -eq 0 ]; then
    show_usage
fi

# Get the argument
arg=$1

# Convert to lowercase for name matching
arg_lower=$(echo "$arg" | tr '[:upper:]' '[:lower:]')

# Map sample names to numbers
case $arg_lower in
    raii|raii)
        num=1
        ;;
    sfinae|sfinae)
        num=2
        ;;
    crtp|crtp)
        num=3
        ;;
    pimpl|pimpl)
        num=4
        ;;
    ruleoffive|ruleof5|rule_of_five)
        num=5
        ;;
    typeerasure|type_erasure)
        num=6
        ;;
    variantvisitor|variant_visitor)
        num=7
        ;;
    smartpointers|smart_pointers)
        num=8
        ;;
    exceptionsafety|exception_safety)
        num=9
        ;;
    movesemantics|move_semantics)
        num=10
        ;;
    *)
        # Try to parse as number, remove leading zeros
        num=$(echo "$arg" | sed 's/^0*//' | grep -E '^[0-9]+$')
        if [ -z "$num" ]; then
            echo "Error: Invalid sample '$arg'"
            show_usage
        fi
        ;;
esac

# Check if executable exists
if [ ! -f "./build/main" ] && [ ! -f "./build/Debug/main.exe" ]; then
    echo "Error: Executable not found. Please build the project first:"
    echo "  vcpkg install"
    echo "  cmake -B build -S ."
    echo "  cmake --build build"
    exit 1
fi

# Run the sample
echo "Running sample $num..."
if [ -f "./build/main" ]; then
    ./build/main $num
elif [ -f "./build/Debug/main.exe" ]; then
    ./build/Debug/main.exe $num
else
    echo "Error: Could not find executable"
    exit 1
fi