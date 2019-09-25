# Unit Testing

## Terms

We use the terms that googletest uses. The smallest unit is a *test* which is a
single test function possibly containing multiple assertions. The next bigger
unit is a *test case*, possibly containing multiple tests.

For the biggest unit, there does not seem to be a term used by googletest. We
call the biggest unit a *test program*, which is a single XYZTest.cpp file that
is compiled to an executable XYZTest binary. A test program possibly contains
multiple test cases.

## Test programs

We create one test program per module. For example, we consider the ANL and ANL
semantics to be two distinct modules. Thus we have two test programs, ANLTest
and ANLAlgorithmTest.

Names of test programs are chosen to clearly indicate which module they belong
to. For technical reasons, source files of test programs must end in `Test.cpp`.

## Quantity

We create at least one test per function or public method. Exceptions to this
are functions that expose no easily testable behavior (e.g. functions that only
output to stdout).

## Quality

The test(s) for one function or public method cover at least one regular case
and one representative per distinct corner case, if any. For example, a function
that takes an `int` between 1 and 5 and computes the square would be tested for
at least one value from the interval (MinInt, 1), at least one value from the
interval [1, 5], and at least one value from the interval (5, MaxInt).

## Coverage

We do not use common code coverage metrics like line coverage or term coverage.
Instead, if a function has abnormal exit points (function exits like errors and
exceptions) we create a test for each such exit point. If the exit point is in
another tested method or function, we do not consider the exit point in the
"outer" method. See the example below.

```
// Pseudo-C example of above exception.
int sqrt_plus_one(int x) {
  // For sqrt_plus_one's unit test, we do NOT test the exit point inside sqrt.
  return sqrt(x) + 1;
}

int sqrt(int x) {
  // For sqrt's unit test, we test this exit point.
  error_if(x < 0);

  // Compute it somehow
  return sqrt_impl(x);
}
```

If an exit point is not reachable we do not test it. One example of this is an
assertion that validates that the internal state of an object is well-formed. As
we are not able to invalidate the internal state of an object through its public
interface we can not test an assertion failure of this assertion.

## Documentation

For each scenario inside a test we document why this scenario was chosen.

## Naming

We will name *tests* either by the behavior that is tested
(e.g. `nullptrAsComponentFails`) or by the method/function that is tested
(e.g. `toString`).

We will name *test cases* by the name of the class/feature that is tested
together with one of the suffixes `Test` or `DeathTest`. `DeathTest` should be
used when the assertion `ASSERT_DEATH` (or similar assertions) is used, as
recommended by the googletest manual.
