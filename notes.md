# math-core — Your Complete Guide
### For: [Friend's name]
### Project: MathBoard — local-first handwritten math solver

---

## What You're Building

A C++ library called `math-core` that takes a LaTeX string as input
and returns the evaluated or simplified result as a string.

That's it. You don't touch the app, the UI, the canvas, or anything
related to Windows. Your entire world is:

```
LaTeX string in  →  result string out
```

Example:
```
Input:  "\frac{1}{2} + \frac{1}{3}"
Output: "5/6"

Input:  "x^2 - 5x + 6 = 0"
Output: "x = 2, x = 3"

Input:  "\int x^2 dx"
Output: "x^3/3 + C"
```

The app will call your library. You never call the app.

---

## Tools You Need to Install

### 1. Git
Download from: https://git-scm.com/download/win
During install: keep all defaults. Make sure "Git from the command line"
is selected.

Verify:
```bash
git --version
```

### 2. Visual Studio 2022 — Build Tools Only
You do NOT need the full Visual Studio IDE.
Download "Build Tools for Visual Studio 2022" from:
https://visualstudio.microsoft.com/downloads/
Scroll down to "Tools for Visual Studio" → "Build Tools for Visual Studio 2022"

During install, check ONE workload:
- **C++ build tools**

Inside it, make sure these are selected:
- MSVC v143 compiler
- Windows 11 SDK
- CMake tools for Windows
- Ninja build system

### 3. CMake (standalone)
Download from: https://cmake.org/download
Pick: Windows x64 Installer (.msi)
During install: tick **"Add CMake to system PATH for all users"**

Verify:
```bash
cmake --version
```

### 4. vcpkg (C++ package manager)
Open a terminal (PowerShell or cmd) and run:

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

Then set an environment variable so CMake always finds it:
1. Search "Edit the system environment variables" in Start
2. Click Environment Variables
3. Under System Variables → New:
   - Name:  `VCPKG_ROOT`
   - Value: `C:\vcpkg`

Verify:
```powershell
vcpkg --version
```

### 5. Visual Studio Code (for editing — optional but recommended)
Download from: https://code.visualstudio.com
Install the **C/C++** extension by Microsoft inside VS Code.

---

## Creating the Repository

### Step 1 — Create a GitHub account (if you don't have one)
Go to https://github.com and sign up.

### Step 2 — Create the repository
1. Click the **+** button top right → "New repository"
2. Repository name: `math-core`
3. Description: "Math engine library for MathBoard"
4. Set to **Private** (you'll add the other developer as a collaborator)
5. Check "Add a README file"
6. Click **Create repository**

### Step 3 — Add the other developer as collaborator
1. Go to your repo → Settings → Collaborators
2. Click "Add people"
3. Search by their GitHub username
4. Set role: Write
5. They'll get an email invite

### Step 4 — Clone it to your machine
```bash
git clone https://github.com/YOUR_USERNAME/math-core.git C:\Projects\math-core
cd C:\Projects\math-core
```

---

## Project Structure to Create

Run these commands to create all the folders and empty files:

```powershell
cd C:\Projects\math-core

# Folders
mkdir include
mkdir src
mkdir tests
mkdir docs

# Files — create empty ones
New-Item include\math_core.h     -ItemType File
New-Item src\context.cpp         -ItemType File
New-Item src\lexer.h             -ItemType File
New-Item src\lexer.cpp           -ItemType File
New-Item src\parser.h            -ItemType File
New-Item src\parser.cpp          -ItemType File
New-Item src\ast.h               -ItemType File
New-Item src\evaluator.h         -ItemType File
New-Item src\evaluator.cpp       -ItemType File
New-Item src\symbolic.h          -ItemType File
New-Item src\symbolic.cpp        -ItemType File
New-Item src\solver.h            -ItemType File
New-Item src\solver.cpp          -ItemType File
New-Item src\formatter.h         -ItemType File
New-Item src\formatter.cpp       -ItemType File
New-Item tests\test_lexer.cpp    -ItemType File
New-Item tests\test_parser.cpp   -ItemType File
New-Item tests\test_evaluator.cpp -ItemType File
New-Item tests\test_symbolic.cpp -ItemType File
New-Item CMakeLists.txt          -ItemType File
New-Item docs\SUPPORTED_LATEX.md -ItemType File
```

Your final structure:
```
math-core/
├── CMakeLists.txt
├── include/
│   └── math_core.h          ← THE CONTRACT (the most important file)
├── src/
│   ├── context.cpp
│   ├── lexer.h / lexer.cpp
│   ├── parser.h / parser.cpp
│   ├── ast.h
│   ├── evaluator.h / evaluator.cpp
│   ├── symbolic.h / symbolic.cpp
│   ├── solver.h / solver.cpp
│   └── formatter.h / formatter.cpp
├── tests/
│   ├── test_lexer.cpp
│   ├── test_parser.cpp
│   ├── test_evaluator.cpp
│   └── test_symbolic.cpp
├── docs/
│   └── SUPPORTED_LATEX.md
└── README.md
```

---

## Add a .gitignore

Create a file called `.gitignore` in your repo root with this content:

```
# Build output
build/
out/
CMakeCache.txt
CMakeFiles/
cmake_install.cmake

# Visual Studio
.vs/
*.user
*.suo

# vcpkg
vcpkg_installed/

# Compiled
*.lib
*.exp
*.obj
*.pdb
```

---

## The Contract Header — `include/math_core.h`

This is the most important file in the whole project.
The other developer's app calls exactly these functions.
Once you both agree on this, **never remove or rename anything in it**.
You can only add new functions.

Copy this exactly:

```c
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ── Version ──────────────────────────────────────────────────── */
#define MATH_CORE_VERSION_MAJOR 1
#define MATH_CORE_VERSION_MINOR 0
#define MATH_CORE_VERSION_PATCH 0

/* ── Opaque context ───────────────────────────────────────────── */

/**
 * Opaque handle. Holds all internal state (parser tables, SymEngine
 * session, etc.). Create one per thread. Not thread-safe.
 */
typedef struct MathContext MathContext;

/* ── Lifecycle ────────────────────────────────────────────────── */

/** Create a new context. Returns NULL on failure. */
MathContext* mc_create_context(void);

/** Destroy a context and free all its memory. */
void mc_destroy_context(MathContext* ctx);

/* ── Core operations ──────────────────────────────────────────── */

/**
 * Evaluate a LaTeX expression numerically or symbolically.
 *
 * Examples:
 *   "2 + 3"               → "5"
 *   "\frac{1}{2}+\frac{1}{3}" → "5/6"
 *   "x^2 + 2x + 1"        → "x^2 + 2*x + 1"  (no simplification here)
 *
 * Returns: 0 on success, non-zero on error.
 * On success: *out_result is a heap-allocated string. Free with mc_free_string().
 * On error:   *out_error  is a heap-allocated error message. Free with mc_free_string().
 * The caller always receives exactly one non-NULL pointer; the other is NULL.
 */
int mc_evaluate_latex(
    MathContext* ctx,
    const char*  latex,
    char**       out_result,
    char**       out_error
);

/**
 * Simplify a LaTeX expression algebraically.
 *
 * Examples:
 *   "x^2 + 2*x + 1"  → "(x + 1)^2"
 *   "\frac{x^2-1}{x-1}" → "x + 1"
 *
 * Returns: 0 on success, non-zero on error.
 * Same memory contract as mc_evaluate_latex.
 */
int mc_simplify_latex(
    MathContext* ctx,
    const char*  latex,
    char**       out_result,
    char**       out_error
);

/**
 * Differentiate a LaTeX expression with respect to a variable.
 *
 * Examples:
 *   latex="x^3 + 2*x", variable="x"  → "3*x^2 + 2"
 *   latex="\sin(x)",   variable="x"  → "\cos(x)"
 *
 * Returns: 0 on success, non-zero on error.
 * Same memory contract as mc_evaluate_latex.
 */
int mc_differentiate(
    MathContext* ctx,
    const char*  latex,
    const char*  variable,
    char**       out_result,
    char**       out_error
);

/**
 * Solve a LaTeX equation for a variable.
 *
 * Examples:
 *   latex="2*x + 3 = 7", variable="x"  → "x = 2"
 *   latex="x^2 - 4 = 0", variable="x"  → "x = 2, x = -2"
 *
 * Returns: 0 on success, non-zero on error.
 * Same memory contract as mc_evaluate_latex.
 */
int mc_solve_latex(
    MathContext* ctx,
    const char*  latex,
    const char*  variable,
    char**       out_result,
    char**       out_error
);

/* ── Memory ───────────────────────────────────────────────────── */

/**
 * Free any string returned by this library.
 * Never pass a pointer not returned by this library.
 * Never call twice on the same pointer.
 */
void mc_free_string(char* str);

/* ── Versioning ───────────────────────────────────────────────── */

/** Fill in the version numbers. */
void mc_get_version(int* major, int* minor, int* patch);

#ifdef __cplusplus
}
#endif
```

---

## CMakeLists.txt

Paste this into your root `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20)
project(math_core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── vcpkg integration ──────────────────────────────────────────
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE
        "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# ── Dependencies ───────────────────────────────────────────────
find_package(GTest CONFIG REQUIRED)

# SymEngine — we'll add this in Week 4.
# find_package(symengine CONFIG REQUIRED)

# ── Library target ─────────────────────────────────────────────
add_library(math_core STATIC
    src/context.cpp
    src/lexer.cpp
    src/parser.cpp
    src/evaluator.cpp
    src/symbolic.cpp
    src/solver.cpp
    src/formatter.cpp
)

target_include_directories(math_core
    PUBLIC  include       # exposes math_core.h to anyone linking us
    PRIVATE src           # internal headers only visible inside the lib
)

# ── Tests ──────────────────────────────────────────────────────
enable_testing()

add_executable(math_core_tests
    tests/test_lexer.cpp
    tests/test_parser.cpp
    tests/test_evaluator.cpp
    tests/test_symbolic.cpp
)

target_link_libraries(math_core_tests PRIVATE
    math_core
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(math_core_tests)
```

---

## Install Dependencies via vcpkg

Open PowerShell and run:

```powershell
vcpkg install gtest:x64-windows
```

SymEngine comes later in Week 4. Don't install it yet.

---

## Week 1 Goal — Get It Building

Before writing any real math code, get the project to compile and
run a passing test. This confirms your whole toolchain works.

### Step 1 — Stub implementations

Paste this into `src/context.cpp`:

```cpp
#include "math_core.h"
#include <cstring>
#include <cstdlib>

// Internal state — empty for now, we'll fill this in later
struct MathContext {
    int placeholder;
};

MathContext* mc_create_context() {
    return new MathContext{};
}

void mc_destroy_context(MathContext* ctx) {
    delete ctx;
}

int mc_evaluate_latex(MathContext*, const char*, char** out_result, char** out_error) {
    *out_result = _strdup("TODO");
    *out_error  = nullptr;
    return 0;
}

int mc_simplify_latex(MathContext*, const char*, char** out_result, char** out_error) {
    *out_result = _strdup("TODO");
    *out_error  = nullptr;
    return 0;
}

int mc_differentiate(MathContext*, const char*, const char*, char** out_result, char** out_error) {
    *out_result = _strdup("TODO");
    *out_error  = nullptr;
    return 0;
}

int mc_solve_latex(MathContext*, const char*, const char*, char** out_result, char** out_error) {
    *out_result = _strdup("TODO");
    *out_error  = nullptr;
    return 0;
}

void mc_free_string(char* str) {
    free(str);
}

void mc_get_version(int* major, int* minor, int* patch) {
    if (major) *major = 1;
    if (minor) *minor = 0;
    if (patch) *patch = 0;
}
```

Leave all other `.cpp` files empty for now.

### Step 2 — Write one smoke test

Paste this into `tests/test_evaluator.cpp`:

```cpp
#include <gtest/gtest.h>
#include "math_core.h"

TEST(Lifecycle, CreateAndDestroy) {
    MathContext* ctx = mc_create_context();
    ASSERT_NE(ctx, nullptr);
    mc_destroy_context(ctx);
}

TEST(Evaluate, StubReturnsAnything) {
    MathContext* ctx = mc_create_context();

    char* result = nullptr;
    char* error  = nullptr;
    int   status = mc_evaluate_latex(ctx, "2+3", &result, &error);

    EXPECT_EQ(status, 0);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(error, nullptr);

    mc_free_string(result);
    mc_destroy_context(ctx);
}
```

Leave the other test files empty for now.

### Step 3 — Build and run

```powershell
cd C:\Projects\math-core
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build
cd build
ctest --output-on-failure
```

You should see:
```
[  PASSED  ] 2 tests.
```

If that passes, your entire toolchain is working. Commit everything:

```bash
git add .
git commit -m "Week 1: project setup, stub implementation, smoke tests pass"
git push
```

---

## Week 2 Goal — The Lexer

The lexer reads a raw LaTeX string character by character and
produces a flat list of tokens. No math happens here — this is
purely about recognising what's in the string.

### Token types you need

Paste this into `src/lexer.h`:

```cpp
#pragma once
#include <string>
#include <vector>

namespace mc {

enum class TokenType {
    // Literals
    Number,      // "3", "3.14", "42"
    Variable,    // "x", "y", "n" (single letter not preceded by backslash)

    // Operators
    Plus,        // +
    Minus,       // -
    Star,        // *  (explicit multiplication)
    Slash,       // /  (plain division, not \frac)
    Caret,       // ^  (exponent)
    Equals,      // =

    // Grouping
    LBrace,      // {
    RBrace,      // }
    LParen,      // (
    RParen,      // )
    Underscore,  // _  (subscript, used in \int_a^b)

    // LaTeX commands (backslash keywords)
    CmdFrac,     // \frac
    CmdSqrt,     // \sqrt
    CmdInt,      // \int
    CmdSum,      // \sum
    CmdPi,       // \pi
    CmdSin,      // \sin
    CmdCos,      // \cos
    CmdTan,      // \tan
    CmdLn,       // \ln
    CmdLog,      // \log
    CmdInfty,    // \infty
    CmdCdot,     // \cdot  (multiplication dot)
    CmdTimes,    // \times (multiplication cross)
    CmdDiv,      // \div

    // Special
    EndOfInput,
    Unknown      // anything we don't recognise yet
};

struct Token {
    TokenType   type;
    std::string value;  // raw text for Number and Variable tokens
};

// Tokenise a LaTeX string. Never throws — unknown sequences become
// Token{Unknown, raw_text}.
std::vector<Token> tokenise(const std::string& latex);

} // namespace mc
```

### Lexer implementation — `src/lexer.cpp`

```cpp
#include "lexer.h"
#include <cctype>
#include <unordered_map>

namespace mc {

// Map from LaTeX command name (without backslash) to TokenType
static const std::unordered_map<std::string, TokenType> kCommands = {
    {"frac",  TokenType::CmdFrac},
    {"sqrt",  TokenType::CmdSqrt},
    {"int",   TokenType::CmdInt},
    {"sum",   TokenType::CmdSum},
    {"pi",    TokenType::CmdPi},
    {"sin",   TokenType::CmdSin},
    {"cos",   TokenType::CmdCos},
    {"tan",   TokenType::CmdTan},
    {"ln",    TokenType::CmdLn},
    {"log",   TokenType::CmdLog},
    {"infty", TokenType::CmdInfty},
    {"cdot",  TokenType::CmdCdot},
    {"times", TokenType::CmdTimes},
    {"div",   TokenType::CmdDiv},
};

std::vector<Token> tokenise(const std::string& latex)
{
    std::vector<Token> tokens;
    size_t i = 0;
    const size_t len = latex.size();

    while (i < len)
    {
        char c = latex[i];

        // ── Whitespace — skip ──────────────────────────────────
        if (std::isspace(c)) { ++i; continue; }

        // ── Number ────────────────────────────────────────────
        if (std::isdigit(c) || (c == '.' && i+1 < len && std::isdigit(latex[i+1])))
        {
            std::string num;
            while (i < len && (std::isdigit(latex[i]) || latex[i] == '.'))
                num += latex[i++];
            tokens.push_back({TokenType::Number, num});
            continue;
        }

        // ── LaTeX command (\keyword) ───────────────────────────
        if (c == '\\' && i+1 < len && std::isalpha(latex[i+1]))
        {
            ++i; // skip backslash
            std::string name;
            while (i < len && std::isalpha(latex[i]))
                name += latex[i++];

            auto it = kCommands.find(name);
            if (it != kCommands.end())
                tokens.push_back({it->second, "\\" + name});
            else
                tokens.push_back({TokenType::Unknown, "\\" + name});
            continue;
        }

        // ── Single-letter variable ─────────────────────────────
        if (std::isalpha(c))
        {
            tokens.push_back({TokenType::Variable, std::string(1, c)});
            ++i;
            continue;
        }

        // ── Single-character operators and grouping ────────────
        switch (c)
        {
            case '+': tokens.push_back({TokenType::Plus,       "+"}); break;
            case '-': tokens.push_back({TokenType::Minus,      "-"}); break;
            case '*': tokens.push_back({TokenType::Star,       "*"}); break;
            case '/': tokens.push_back({TokenType::Slash,      "/"}); break;
            case '^': tokens.push_back({TokenType::Caret,      "^"}); break;
            case '=': tokens.push_back({TokenType::Equals,     "="}); break;
            case '{': tokens.push_back({TokenType::LBrace,     "{"}); break;
            case '}': tokens.push_back({TokenType::RBrace,     "}"}); break;
            case '(': tokens.push_back({TokenType::LParen,     "("}); break;
            case ')': tokens.push_back({TokenType::RParen,     ")"}); break;
            case '_': tokens.push_back({TokenType::Underscore, "_"}); break;
            default:  tokens.push_back({TokenType::Unknown,    std::string(1,c)}); break;
        }
        ++i;
    }

    tokens.push_back({TokenType::EndOfInput, ""});
    return tokens;
}

} // namespace mc
```

### Lexer tests — `tests/test_lexer.cpp`

```cpp
#include <gtest/gtest.h>
#include "../src/lexer.h"
using namespace mc;

TEST(Lexer, SimpleArithmetic) {
    auto tokens = tokenise("2 + 3");
    ASSERT_EQ(tokens.size(), 4u); // Number Plus Number EndOfInput
    EXPECT_EQ(tokens[0].type,  TokenType::Number);
    EXPECT_EQ(tokens[0].value, "2");
    EXPECT_EQ(tokens[1].type,  TokenType::Plus);
    EXPECT_EQ(tokens[2].type,  TokenType::Number);
    EXPECT_EQ(tokens[2].value, "3");
    EXPECT_EQ(tokens[3].type,  TokenType::EndOfInput);
}

TEST(Lexer, FracCommand) {
    auto tokens = tokenise("\\frac{1}{2}");
    EXPECT_EQ(tokens[0].type, TokenType::CmdFrac);
    EXPECT_EQ(tokens[1].type, TokenType::LBrace);
    EXPECT_EQ(tokens[2].type, TokenType::Number);
    EXPECT_EQ(tokens[3].type, TokenType::RBrace);
    EXPECT_EQ(tokens[4].type, TokenType::LBrace);
    EXPECT_EQ(tokens[5].type, TokenType::Number);
    EXPECT_EQ(tokens[6].type, TokenType::RBrace);
}

TEST(Lexer, Variable) {
    auto tokens = tokenise("x");
    EXPECT_EQ(tokens[0].type,  TokenType::Variable);
    EXPECT_EQ(tokens[0].value, "x");
}

TEST(Lexer, Exponent) {
    auto tokens = tokenise("x^2");
    EXPECT_EQ(tokens[0].type, TokenType::Variable);
    EXPECT_EQ(tokens[1].type, TokenType::Caret);
    EXPECT_EQ(tokens[2].type, TokenType::Number);
}

TEST(Lexer, TrigFunction) {
    auto tokens = tokenise("\\sin(x)");
    EXPECT_EQ(tokens[0].type, TokenType::CmdSin);
    EXPECT_EQ(tokens[1].type, TokenType::LParen);
    EXPECT_EQ(tokens[2].type, TokenType::Variable);
    EXPECT_EQ(tokens[3].type, TokenType::RParen);
}

TEST(Lexer, DecimalNumber) {
    auto tokens = tokenise("3.14");
    EXPECT_EQ(tokens[0].type,  TokenType::Number);
    EXPECT_EQ(tokens[0].value, "3.14");
}

TEST(Lexer, UnknownCommandDoesNotCrash) {
    auto tokens = tokenise("\\unknowncmd");
    EXPECT_EQ(tokens[0].type, TokenType::Unknown);
}

TEST(Lexer, EmptyString) {
    auto tokens = tokenise("");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, TokenType::EndOfInput);
}

TEST(Lexer, Equation) {
    auto tokens = tokenise("2*x + 3 = 7");
    EXPECT_EQ(tokens[0].type, TokenType::Number);    // 2
    EXPECT_EQ(tokens[1].type, TokenType::Star);      // *
    EXPECT_EQ(tokens[2].type, TokenType::Variable);  // x
    EXPECT_EQ(tokens[3].type, TokenType::Plus);      // +
    EXPECT_EQ(tokens[4].type, TokenType::Number);    // 3
    EXPECT_EQ(tokens[5].type, TokenType::Equals);    // =
    EXPECT_EQ(tokens[6].type, TokenType::Number);    // 7
}
```

Build and run after writing this:
```powershell
cmake --build build
cd build && ctest --output-on-failure
```

All 9 tests should pass. Commit:
```bash
git add .
git commit -m "Week 2: lexer complete, 9 tests passing"
git push
```

---

## Weeks 3–6 — What Comes Next

Here is your full roadmap. Each week builds on the previous one.

### Week 3 — Parser + Numeric Evaluator
Build a Pratt parser (top-down operator precedence) that turns the
token list into an AST. Then walk the AST to evaluate numerically.

Target: `mc_evaluate_latex` works for:
- Arithmetic: `2 + 3*4 → 14`
- Fractions: `\frac{1}{2} + \frac{1}{3} → 5/6`
- Powers: `2^{10} → 1024`
- Trig: `\sin(\pi) → 0`
- Parentheses: `(2+3)*4 → 20`

### Week 4 — SymEngine Integration
Install SymEngine via vcpkg:
```powershell
vcpkg install symengine:x64-windows
```

Use it to implement `mc_simplify_latex`:
- `x^2 + 2x + 1 → (x+1)^2`
- `\frac{x^2-1}{x-1} → x+1`

This is where your math knowledge matters most. SymEngine is a
full symbolic algebra system — learn its expression API first before
trying to wire it up.

### Week 5 — Solver
Implement `mc_solve_latex` using SymEngine's `solve()`:
- Linear: `2x + 3 = 7 → x = 2`
- Quadratic: `x^2 - 4 = 0 → x = 2, x = -2`
- Systems (stretch goal): `x + y = 5, x - y = 1 → x=3, y=2`

### Week 6 — Calculus
Implement `mc_differentiate` using SymEngine's `diff()`:
- `x^3 → 3x^2`
- `\sin(x) → \cos(x)`
- `x^2 \cdot \ln(x) → 2x\ln(x) + x` (product rule, handled by SymEngine)

Stretch goal: definite integration using SymEngine's `integrate()`:
- `\int_0^1 x^2 dx → 1/3`

---

## Rules You Must Follow

### Memory contract
Every string your library returns must be:
- Heap-allocated with `malloc` or `strdup`
- Freed by the caller using `mc_free_string`
- Never a stack address, never a string literal

Symmetry rule: if the function allocates `out_result`, it must set
`out_error = nullptr`. If it allocates `out_error`, it must set
`out_result = nullptr`. Never both non-null, never both null.

### Thread safety
`MathContext` is not thread-safe. Document this clearly in your README.
One context per thread. If the caller wants to use multiple threads,
they create one context per thread. This is fine and expected.

### Error handling
Never crash on bad input. `mc_evaluate_latex("garbage{{{", ...)` must
return an error string, not a segfault. Every public function must
have a try-catch or equivalent guard at its outermost level.

### The header is a contract
Never change the signature of an existing function.
Never remove a function.
Add new functions freely at the bottom.
If you want to change something, add a v2 version (`mc_evaluate_latex_v2`).

---

## docs/SUPPORTED_LATEX.md — Keep This Updated

Every time you add support for a new LaTeX construct, add it here.
The other developer uses this to know what the recognition model
output will be safely handled by your library.

Start it with:

```markdown
# Supported LaTeX

## Arithmetic
- `+` `-` `*` `/`
- `^` (exponent)
- `\frac{a}{b}` (fraction)
- `\sqrt{x}` (square root)
- Parentheses `(` `)`

## Constants
- `\pi`
- `\infty`

## Functions
- `\sin` `\cos` `\tan`
- `\ln` `\log`

## Coming soon
- Algebra / solving
- Derivatives
- Integrals
```

---

## How to Run Tests

Every time you write new code, build and run tests:

```powershell
cd C:\Projects\math-core
cmake --build build
cd build
ctest --output-on-failure
```

Aim to keep all tests green at all times.
Never commit code that breaks a passing test.

---

## How to Commit and Push

After finishing a meaningful chunk of work:

```bash
git add .
git commit -m "Brief description of what you did"
git push
```

Good commit messages:
- "Lexer: add support for \sum and \infty tokens"
- "Parser: handle nested \frac expressions"
- "Evaluator: \sin \cos \tan now evaluate correctly"
- "Fix: mc_free_string crash on nullptr input"

Bad commit messages:
- "stuff"
- "fix"
- "wip"
- "asdfgh"

---

## When You're Stuck

- Parser is hard. If you've never written one: look up "Pratt parser tutorial"
  by Bob Nystrom (craftinginterpreters.com). It's the clearest explanation
  that exists. Your parser will look very similar.
- SymEngine docs: https://docs.symengine.org — read the "Basics" section first.
- If a test is hard to write first, write the implementation and test
  it manually, then write the test after. Don't let TDD block you.
- If SymEngine does something unexpected, check what the raw C++ API
  returns before assuming your LaTeX parser is wrong.

---

## Week 1 Checklist

- [ ] GitHub repo created, collaborator added
- [ ] Repo cloned to your machine
- [ ] All tools installed (Git, Build Tools, CMake, vcpkg)
- [ ] .gitignore in place
- [ ] math_core.h written exactly as shown
- [ ] CMakeLists.txt written
- [ ] Stub context.cpp compiles
- [ ] 2 smoke tests pass
- [ ] Everything pushed to GitHub

Send the other developer your GitHub repo link once it's up.
That's it for Week 1.
