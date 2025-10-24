#include <gtest/gtest.h>
#include "tax_calc.cpp"

TEST(TaxAllUses, InputNegativeGrossIncome_ThrowsError) {
    TaxInput in{-1, 0, 0};
    EXPECT_THROW(computeTax(in), std::invalid_argument);
}

TEST(TaxAllUses, GrossZero_TaxIsZero) {
    TaxInput in{0, 0, 0};
    EXPECT_EQ(computeTax(in), 0);
}

TEST(TaxAllUses, SmallTaxableIncome_FirstTaxBracket) {
    TaxInput in{20'000'000, 1, 1'000'000};
    long long expectedTax = 250'000;
    EXPECT_EQ(computeTax(in), expectedTax);
}

TEST(TaxAllUses, DependentsClamped_SecondBracket) {
    TaxInput in{30'000'000, 4, 5'000'000};
    long long expectedTax = 650'000;
    EXPECT_EQ(computeTax(in), expectedTax);
}

TEST(TaxAllUses, HighIncome_ThirdBracket) {
    TaxInput in{100'000'000, 2, 9'000'000};
    long long expectedTax = 13'650'000;
    EXPECT_EQ(computeTax(in), expectedTax);
}

TEST(TaxAllUses, InputNegativeDependents_ThrowsError) {
    TaxInput in{50'000'000, -1, 1'000'000};
    EXPECT_THROW(computeTax(in), std::invalid_argument);
}

TEST(TaxAllUses, InputNegativeInsurance_ThrowsError) {
    TaxInput in{50'000'000, 1, -1'000'000};
    EXPECT_THROW(computeTax(in), std::invalid_argument);
}