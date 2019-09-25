""" See LICENSE.md for copyright details """

import sys


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 ./compare_plain.py ACTUAL.txt SOLL.txt")
        sys.exit(1)
    cli = " ".join(sys.argv)

    lines_actual = []
    lines_soll = []
    with open(sys.argv[1], "r") as file:
        for line in file:
            lines_actual.append(line)
    with open(sys.argv[2], "r") as file:
        for line in file:
            lines_soll.append(line)

    # Strip lines.
    lines_actual = map(lambda x: x.strip(), lines_actual)
    lines_soll = map(lambda x: x.strip(), lines_soll)

    # Strip empty lines from both.
    lines_actual = filter(lambda x: bool(x), lines_actual)
    lines_soll = filter(lambda x: bool(x), lines_soll)

    # Strip comment lines from both.
    lines_actual = filter(lambda x: x[0] != "#", lines_actual)
    lines_soll = filter(lambda x: x[0] != "#", lines_soll)

    # Actually generate the list for the actual lines.
    lines_actual = [x for x in lines_actual]

    # Ensure soll is a subset of actual.
    for soll in lines_soll:
        if len(lines_actual) == 0:
            print("%s not in output" % soll)
            print("\033[91m[ FAIL ]\033[0m %s" % cli)
            sys.exit(1)
        while lines_actual[0] != soll:
            lines_actual = lines_actual[1:]
            if len(lines_actual) == 0:
                print("%s not in output" % soll)
                print("\033[91m[ FAIL ]\033[0m %s" % cli)
                sys.exit(1)
        lines_actual = lines_actual[1:]
    print("\033[92m[  OK  ]\033[0m %s" % cli)
