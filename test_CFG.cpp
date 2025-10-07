#include <gtest/gtest.h>
#include "tax_calc.cpp"

// Test 1: input âm → kiểm tra nhánh C1(True)
TEST(TaxControlFlow, InputNegative_ThrowsError) {
    TaxInput in{-1, 0, 0};
    EXPECT_THROW(computeTax(in), std::invalid_argument);
}

// Test 2: gross = 0 → gross <= 0 (C2 True), taxable <= 0 (C3 True)
TEST(TaxControlFlow, GrossZero_TaxableZero) {
    TaxInput in{0, 0, 0};
    EXPECT_EQ(computeTax(in), 0);
}

// Test 3: taxable ≤ 5.000.000 (C4 True), remain ≤ 10.000.000 (C5 True)
TEST(TaxControlFlow, SmallIncome_FirstTaxBracket) {
    TaxInput in{20'000'000, 1, 1'000'000};
    long long expectedTax = 250'000; // taxable = 5.000.000 → 5% = 250k
    EXPECT_EQ(computeTax(in), expectedTax);
}

// Test 4: dependents > 2 (bị clamp), taxable trong khoảng (5–15] → C4 False, C5 True
TEST(TaxControlFlow, DependentsClamped_SecondBracket) {
    TaxInput in{30'000'000, 4, 5'000'000};
    long long expectedTax = 650'000; // taxable = 9.000.000 → 250k + 400k = 650k
    EXPECT_EQ(computeTax(in), expectedTax);
}

// Test 5: taxable > 15.000.000 → C4 False, C5 False (bậc 3)
TEST(TaxControlFlow, HighIncome_ThirdBracket) {
    TaxInput in{100'000'000, 2, 9'000'000};
    long long expectedTax = 13'650'000; // 5tr*5% + 10tr*10% + 62tr*20%
    EXPECT_EQ(computeTax(in), expectedTax);
}