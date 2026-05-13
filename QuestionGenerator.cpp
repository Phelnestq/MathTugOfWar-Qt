#include "QuestionGenerator.h"
#include <cstdlib>
#include <string>
#include <cmath>

QuestionGenerator::QuestionGenerator() {}

Question QuestionGenerator::generate(Difficulty difficulty) const {
    switch (difficulty) {
        case Difficulty::PRIMARY_EASY:   return generatePrimaryEasy();
        case Difficulty::PRIMARY_HARD:   return generatePrimaryHard();
        case Difficulty::SECONDARY_EASY: return generateSecondaryEasy();
        case Difficulty::SECONDARY_HARD: return generateSecondaryHard();
    }
    return generatePrimaryEasy();
}

static int rnd(int min, int max) {
    return min + rand() % (max - min + 1);
}

static std::string itos(int n) {
    return std::to_string(n);
}

// ── PRIMARY EASY — KS1/KS2 early (Age 5–8) ────────────────────────────────
Question QuestionGenerator::generatePrimaryEasy() const {
    int op = rand() % 3;

    if (op == 0) {
        int a = rnd(1, 15), b = rnd(1, 15);
        return Question(itos(a) + " + " + itos(b) + " = ?",
                        a + b, Difficulty::PRIMARY_EASY);
    }
    if (op == 1) {
        int a = rnd(5, 20), b = rnd(1, a);
        return Question(itos(a) + " - " + itos(b) + " = ?",
                        a - b, Difficulty::PRIMARY_EASY);
    }
    int a = rnd(2, 5), b = rnd(2, 5);
    return Question(itos(a) + " x " + itos(b) + " = ?",
                    a * b, Difficulty::PRIMARY_EASY);
}

// ── PRIMARY HARD — KS2 late (Age 9–11) ────────────────────────────────────
Question QuestionGenerator::generatePrimaryHard() const {
    int op = rand() % 4;

    if (op == 0) {
        int a = rnd(3, 12), b = rnd(3, 12);
        return Question(itos(a) + " x " + itos(b) + " = ?",
                        a * b, Difficulty::PRIMARY_HARD);
    }
    if (op == 1) {
        int b = rnd(3, 12), c = rnd(2, 10);
        int a = b * c;
        return Question(itos(a) + " / " + itos(b) + " = ?",
                        c, Difficulty::PRIMARY_HARD);
    }
    if (op == 2) {
        int a = rnd(2, 8), b = rnd(2, 6), c = rnd(2, 6);
        if (rand() % 2 == 0) {
            return Question(itos(a) + " + " + itos(b) + " x " + itos(c) + " = ?",
                            a + b * c, Difficulty::PRIMARY_HARD);
        } else {
            return Question(itos(a) + " x " + itos(b) + " - " + itos(c) + " = ?",
                            a * b - c, Difficulty::PRIMARY_HARD);
        }
    }
    int a = rnd(2, 10);
    if (rand() % 2 == 0) {
        return Question(itos(a) + "² = ?",
                        a * a, Difficulty::PRIMARY_HARD);
    }
    return Question("√" + itos(a * a) + " = ?",
                    a, Difficulty::PRIMARY_HARD);
}

// ── SECONDARY EASY — KS3 (Age 11–14) ─────────────────────────────────────
Question QuestionGenerator::generateSecondaryEasy() const {
    int op = rand() % 5;

    if (op == 0) {
        int a = rnd(2, 8), x = rnd(1, 10), b = rnd(1, 15);
        int c = a * x + b;
        return Question(itos(a) + "x + " + itos(b) + " = " + itos(c) + ",  x = ?",
                        x, Difficulty::SECONDARY_EASY);
    }
    if (op == 1) {
        int p = rnd(1, 9) * 10;
        int n = rnd(2, 20) * 10;
        double ans = (p / 100.0) * n;
        return Question("What is " + itos(p) + "% of " + itos(n) + "?",
                        ans, Difficulty::SECONDARY_EASY);
    }
    if (op == 2) {
        int a = rnd(-12, -1), b = rnd(1, 15);
        if (rand() % 2 == 0) {
            return Question(itos(a) + " + " + itos(b) + " = ?",
                            a + b, Difficulty::SECONDARY_EASY);
        }
        return Question(itos(b) + " + (" + itos(a) + ") = ?",
                        a + b, Difficulty::SECONDARY_EASY);
    }
    if (op == 3) {
        int base = rnd(2, 5), exp = rnd(2, 4);
        return Question(itos(base) + "^" + itos(exp) + " = ?",
                        std::pow(base, exp), Difficulty::SECONDARY_EASY);
    }
    int denom = (rand() % 2 == 0) ? 4 : 5;
    int numer = rnd(1, denom - 1);
    int n     = rnd(2, 10) * denom;
    return Question(itos(numer) + "/" + itos(denom) + " of " + itos(n) + " = ?",
                    (double)numer / denom * n, Difficulty::SECONDARY_EASY);
}

// ── SECONDARY HARD — KS4 GCSE (Age 14–16) ────────────────────────────────
Question QuestionGenerator::generateSecondaryHard() const {
    int op = rand() % 5;

    if (op == 0) {
        // Quadratic x² - (r1+r2)x + r1*r2 = 0, find larger root
        int r1 = rnd(1, 8), r2 = rnd(1, 8);
        int b  = -(r1 + r2), c = r1 * r2;
        std::string bStr = (b >= 0) ? "+ " + itos(b) : "- " + itos(-b);
        std::string cStr = (c >= 0) ? "+ " + itos(c) : "- " + itos(-c);
        return Question("x² " + bStr + "x " + cStr + " = 0,  larger x = ?",
                        std::max(r1, r2), Difficulty::SECONDARY_HARD);
    }
    if (op == 1) {
        // Simultaneous: x + y = sum, x - y = diff
        int x = rnd(2, 12), y = rnd(2, 12);
        return Question("x + y = " + itos(x + y) + ",  x - y = " + itos(x - y) + ",  x = ?",
                        x, Difficulty::SECONDARY_HARD);
    }
    if (op == 2) {
        // Pythagoras using known triples
        int triples[][3] = {{3,4,5},{5,12,13},{8,15,17},{6,8,10},{9,12,15}};
        int t = rand() % 5;
        int a = triples[t][0], b = triples[t][1], c = triples[t][2];
        if (rand() % 2 == 0) {
            return Question("Right triangle: sides " + itos(a) + " and " + itos(b) +
                            ", hypotenuse = ?",
                            c, Difficulty::SECONDARY_HARD);
        }
        return Question("Right triangle: hypotenuse " + itos(c) + ", one side " +
                        itos(a) + ", other side = ?",
                        b, Difficulty::SECONDARY_HARD);
    }
    if (op == 3) {
        // Index laws
        int base = rnd(2, 5), m = rnd(2, 5), n = rnd(1, 4);
        if (rand() % 2 == 0) {
            return Question(itos(base) + "^" + itos(m) + " x " +
                            itos(base) + "^" + itos(n) + " = " + itos(base) + "^?",
                            m + n, Difficulty::SECONDARY_HARD);
        }
        int bigger = std::max(m, n), smaller = std::min(m, n);
        return Question(itos(base) + "^" + itos(bigger) + " / " +
                        itos(base) + "^" + itos(smaller) + " = " + itos(base) + "^?",
                        bigger - smaller, Difficulty::SECONDARY_HARD);
    }
    // Ratio: share total in ratio part1:part2, find larger share
    int part1 = rnd(2, 6), part2 = rnd(2, 6);
    int total  = rnd(3, 8) * (part1 + part2);  // guarantees clean division
    int share1 = total * part1 / (part1 + part2);
    int share2 = total - share1;
    return Question("Share " + itos(total) + " in ratio " +
                    itos(part1) + ":" + itos(part2) + ", larger share = ?",
                    std::max(share1, share2), Difficulty::SECONDARY_HARD);
}
