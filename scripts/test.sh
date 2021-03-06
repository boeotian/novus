#!/usr/bin/env bash
set -e -o pipefail

# --------------------------------------------------------------------------------------------------
# Test the project as configured.
# --------------------------------------------------------------------------------------------------

info()
{
  echo "${1}"
}

error()
{
  echo "ERROR: ${1}" >&2
}

fail()
{
  error "${1}"
  exit 1
}

hasCommand()
{
  [ -x "$(command -v "${1}")" ]
}

testProj()
{
  local dir="${1}"

  # Verify build directory exists.
  test -d "${dir}" || \
    fail "Build directory is missing, please run 'configure.sh' first"

  # Verify Makefile in build directory exists.
  test -f "${dir}/Makefile" || \
    fail "No 'Makefile' found in build directory, please run 'configure.sh' first"

  # Verify that make is present on path.
  hasCommand ctest || fail "'ctest' not found on path, it is required"

  info "Begin testing using ctest"

  cd "${dir}" && ctest --output-on-failure

  info "Succesfully finished testing"
}

printUsage()
{
  echo "Options:"
  echo "-h,--help     Print this usage information"
  echo "-d,--dir      Build directory, default: 'build'"
}

# Defaults.
buildDir="build"

# Parse options.
while [[ $# -gt 0 ]]
do
  case "${1}" in
    -h|--help)
      echo "Novus -- Test"
      printUsage
      exit 0
      ;;
    -d|--dir)
      buildDir="${2}"
      shift 2
      ;;
    *)
      error "Unknown option '${1}'"
      printUsage
      exit 1
      ;;
  esac
done

# Run test.
testProj "${buildDir}"
exit 0
