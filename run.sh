#!/bin/bash

# RISim Runner Script
# Convenience wrapper for running scripts in the scripts/ folder

set -e

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to display help
show_help() {
    cat << EOF
Usage: ./run.sh [OPTION] [ARGS...]

RISim Runner - Convenience script for running various build and test configurations

OPTIONS:
    -h, --help              Show this help message
    -l, --local             Build and run locally (scripts/run.sh)
    -u, --unit-test         Run unit tests (scripts/run_unit_test.sh)
    -uv, --unit-valgrind    Run unit tests with Valgrind (scripts/run_unit_test_valgrind.sh)
    -v, --valgrind          Run with Valgrind (scripts/run_valgrind.sh)
    --hpc                   Run HPC version (scripts/run_hpc.sh)

EXAMPLES:
    ./run.sh                        # Display this help
    ./run.sh -l                     # Build and run locally
    ./run.sh --local arg1 arg2      # Run locally with arguments
    ./run.sh -u                     # Run unit tests
    ./run.sh --valgrind             # Run with Valgrind memory analysis

NOTES:
    - Any additional arguments after the option are passed to the underlying script
    - Scripts are located in the scripts/ folder
    - Make sure scripts have execute permissions (chmod +x scripts/*.sh)

EOF
}

# Function to run a script
run_script() {
    local script_path="$1"
    shift

    if [ ! -f "$script_path" ]; then
        echo -e "${RED}Error: Script not found: $script_path${NC}"
        exit 1
    fi

    if [ ! -x "$script_path" ]; then
        echo -e "${YELLOW}Warning: Script is not executable, attempting to run with bash...${NC}"
        bash "$script_path" "$@"
    else
        "$script_path" "$@"
    fi
}

# Main logic
if [ $# -eq 0 ]; then
    show_help
    exit 0
fi

case "$1" in
    -h|--help)
        show_help
        ;;
    -l|--local)
        shift
        echo -e "${GREEN}Running local build...${NC}"
        run_script "scripts/run.sh" "$@"
        ;;
    -u|--unit-test)
        shift
        echo -e "${GREEN}Running unit tests...${NC}"
        run_script "scripts/run_unit_test.sh" "$@"
        ;;
    -uv|--unit-valgrind)
        shift
        echo -e "${GREEN}Running unit tests with Valgrind...${NC}"
        run_script "scripts/run_unit_test_valgrind.sh" "$@"
        ;;
    -v|--valgrind)
        shift
        echo -e "${GREEN}Running with Valgrind...${NC}"
        run_script "scripts/run_valgrind.sh" "$@"
        ;;
    --hpc)
        shift
        echo -e "${GREEN}Running HPC version...${NC}"
        run_script "scripts/run_hpc.sh" "$@"
        ;;
    *)
        echo -e "${RED}Error: Unknown option: $1${NC}"
        echo ""
        show_help
        exit 1
        ;;
esac
