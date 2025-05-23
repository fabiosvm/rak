#!/usr/bin/env python3

import subprocess
import re
import sys
import os
import argparse
import pathlib

# External dependencies
try:
    import yaml
except ImportError:
    raise ImportError(
        "The 'pyyaml' package is required.\n"
        "Please install it with: pip install pyyaml"
    )

executable_path = None


def run_one_test(test_case: dict) -> dict:
    """Runs a single test case and returns its result.

    Args:
        Dictionary containing test specification (see `examples.yaml`).

    Returns:
        dict: A dictionary containing test results with the following keys:
            - passed (bool): True if the test succeeded, False otherwise
            - test_name (str): Name/description of the test that was run
            - message (str): Error details (empty if passed)
    """
    source_code = test_case.get("source", "").encode("utf-8")
    expected_output = test_case.get("out", "")
    expected_exit_code = test_case.get("exit_code", 0)
    test_name = test_case.get("test", "No description")
    command_args = test_case.get("args", "")

    try:
        process = subprocess.Popen(
            [executable_path] + command_args.split(),
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        stdout, stderr = process.communicate(input=source_code)
        exit_code = process.returncode

        output = stdout.decode("utf-8").replace("\r\n", "\n") + stderr.decode(
            "utf-8"
        ).replace("\r\n", "\n")

        if exit_code != expected_exit_code:
            return {
                "passed": False,
                "message": f"Wrong exit code. Expected: {expected_exit_code}. Got: {exit_code}.\n{output}\n**********************************",
                "test_name": test_name,
            }

        # Exact match
        if isinstance(expected_output, str):
            if output != expected_output:
                return {
                    "passed": False,
                    "message": f"** Expected: *********************\n{expected_output}\n** Got: **************************\n{output}\n**********************************",
                    "test_name": test_name,
                }
            return {"passed": True, "message": "", "test_name": test_name}

        # Assume regular expression
        if not re.search(
            expected_output["regex"] + "z", output, flags=re.MULTILINE | re.DOTALL
        ):
            return {
                "passed": False,
                "message": f"** Expected pattern: *************\n{expected_output['regex']}\n** Got: **************************\n{output}\n**********************************",
                "test_name": test_name,
            }
        return {"passed": True, "message": "", "test_name": test_name}

    except Exception as e:
        return {
            "passed": False,
            "message": f"Exception running test: {str(e)}",
            "test_name": test_name,
        }


def run_test_file(test_file: str) -> list[tuple[bool, str, str]]:
    """Open one test file and run all its tests.

    Args:
        test_file (str): Test file path.

    Returns:
        list[tuple[bool, str, str]]: A list of test results where each result is a tuple containing:
            - bool: True if the test passed, False otherwise
            - str: The name of the test
            - str: Error message if the test failed, empty string if it passed
    """
    try:
        with open(test_file, 'r', encoding='utf-8') as f:
            tests = yaml.safe_load(f)
    except Exception as e:
        return [(False, test_file, f"Error reading {test_file}: {str(e)}")]

    results = []
    for test_case in tests:
        result = run_one_test(test_case)
        results.append(
            (
                result["passed"],
                f"{test_file} -> {result['test_name']}",
                f"{result['message']}",
            )
        )
    return results


def main():
    global executable_path

    parser = argparse.ArgumentParser(description="Integration test runner -- rak")
    parser.add_argument(
        "-q", "--quiet", action="store_true", help="Print only summary and errors."
    )
    parser.add_argument(
        "-e", "--executable", required=True, help="Path to executable to test"
    )
    parser.add_argument("filter", nargs="?", default=None, help="Testfiles filter")
    args = parser.parse_args()
    executable_path = args.executable

    if not os.path.isfile(executable_path):
        sys.stderr.write(
            f"Error: Executable file '{executable_path}' does not exist. Please build project first.\n"
        )
        sys.exit(1)

    testfiles = []
    for file in pathlib.Path("tests").rglob("*.yaml"):
        testfiles.append(file)
    if args.filter:
        testfiles = [f for f in testfiles if args.filter in str(f)]
    if not testfiles:
        print(" :-|   No testfiles found.")
        sys.exit(1)

    passed_count = 0
    total_tests = 0
    failed_tests = []
    if not args.quiet:
        print(f"Running {len(testfiles)} testfiles:")

    for test_file in testfiles:
        results = run_test_file(test_file)
        for passed, test_id, message in results:
            total_tests += 1
            if passed:
                passed_count += 1
                if not args.quiet:
                    print(f"OK {test_id}")
                continue
            failed_tests.append((test_id, message))
            if not args.quiet:
                print(f":( {test_id}")

    if failed_tests:
        print("\n********************* Failed tests *********************")
        for test, msg in failed_tests:
            print(f"   {test}")
            print(f"{msg}")
        print(f">:(   {total_tests - passed_count} of {total_tests} tests failed.")
        sys.exit(1)

    print(f":D   All {total_tests} tests OK.")
    sys.exit(0)


if __name__ == "__main__":
    main()
