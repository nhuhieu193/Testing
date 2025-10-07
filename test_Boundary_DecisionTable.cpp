#include <gtest/gtest.h>
#include "tax_calc.cpp"

TaxInput makeInput(long long gross, int dep, long long ins) {
    return TaxInput{gross, dep, ins};
}

// === Kiểm thử Giá trị biên (4n + 1 tests) ===
const long long GI_MIN   = 0;
const long long GI_MINP1 = 1;
const long long GI_NOM   = 20'000'000;
const long long GI_MAXM1 = 999'999'999'999LL;
const long long GI_MAX   = 1'000'000'000'000LL;

const int DEP_MIN   = -1;
const int DEP_MINP1 = 0;
const int DEP_NOM   = 1;
const int DEP_MAXM1 = 2;
const int DEP_MAX   = 3;

const long long INS_MIN   = 0;
const long long INS_MINP1 = 1;
const long long INS_NOM   = 3'000'000;
const long long INS_MAXM1 = 999'999'999LL;
const long long INS_MAX   = 1'000'000'000LL;

// === 4 test cho grossIncome ===
TEST(TaxBoundary, GrossIncome_Min) {
    EXPECT_EQ(computeTax(makeInput(GI_MIN, DEP_NOM, INS_NOM)), 0);
}
TEST(TaxBoundary, GrossIncome_MinPlus1) {
    EXPECT_EQ(computeTax(makeInput(GI_MINP1, DEP_NOM, INS_NOM)), 0);
}
TEST(TaxBoundary, GrossIncome_MaxMinus1) {
    // deductions = 17,000,000 -> taxable = 999,999,999,999 - 17,000,000 = 999,982,999,999
    // expected = 199,994,850,000
    EXPECT_EQ(computeTax(makeInput(GI_MAXM1, DEP_NOM, INS_NOM)), 199'994'850'000);
}
TEST(TaxBoundary, GrossIncome_Max) {
    // taxable = 1,000,000,000,000 - 17,000,000 = 999,983,000,000
    // expected = 199,994,850,000
    EXPECT_EQ(computeTax(makeInput(GI_MAX, DEP_NOM, INS_NOM)), 199'994'850'000);
}

// === 4 test cho dependents ===
TEST(TaxBoundary, Dependents_Min) {
    // dependents < 0 -> phải throw invalid_argument
    EXPECT_THROW(computeTax(makeInput(GI_NOM, DEP_MIN, INS_NOM)), std::invalid_argument);
}
TEST(TaxBoundary, Dependents_MinPlus1) {
    EXPECT_EQ(computeTax(makeInput(GI_NOM, DEP_MINP1, INS_NOM)), 550'000);
}
TEST(TaxBoundary, Dependents_MaxMinus1) {
    EXPECT_EQ(computeTax(makeInput(GI_NOM, DEP_MAXM1, INS_NOM)), 0);
}
TEST(TaxBoundary, Dependents_Max) {
    EXPECT_EQ(computeTax(makeInput(GI_NOM, DEP_MAX, INS_NOM)), 0);
}

// === 4 test cho insurancePaid ===
TEST(TaxBoundary, Insurance_Min) {
    EXPECT_EQ(computeTax(makeInput(GI_NOM, DEP_NOM, INS_MIN)), 350'000);
}
TEST(TaxBoundary, Insurance_MinPlus1) {
    EXPECT_EQ(computeTax(makeInput(GI_NOM, DEP_NOM, INS_MINP1)), 350'000);
}
TEST(TaxBoundary, Insurance_MaxMinus1) {
    EXPECT_EQ(computeTax(makeInput(GI_NOM, DEP_NOM, INS_MAXM1)), 200'000);
}
TEST(TaxBoundary, Insurance_Max) {
    EXPECT_EQ(computeTax(makeInput(GI_NOM, DEP_NOM, INS_MAX)), 200'000);
}

// === 1 test all nominal ===
TEST(TaxBoundary, AllNominal) {
    EXPECT_EQ(computeTax(makeInput(GI_NOM, DEP_NOM, INS_NOM)), 200'000);
}



// === Kiểm thử theo bảng quyết định ===

// R1: Gross < 0  -> lỗi input
TEST(DecisionTable, R1_InvalidGross) {
    EXPECT_THROW(computeTax(makeInput(-1, 1, 0)), std::invalid_argument);
}


// R2: InsurancePaid < 0 -> lỗi input
TEST(DecisionTable, R2_InvalidInsurance) {
    EXPECT_THROW(computeTax(makeInput(20000000, 1, -1)), std::invalid_argument);
}

// R3: Dependent < 0
// - SPEC_STRICT: theo bảng quyết định -> lỗi input
// - default: theo code hiện tại (clamp về 0) -> thuế = 550000
TEST(DecisionTable, R3_NegativeDependent) {
    EXPECT_THROW(computeTax(makeInput(20000000, -1, 3000000)), std::invalid_argument);
    // EXPECT_EQ(computeTax(makeInput(20000000, -1, 3000000)), 550000);
}

// R4: Taxable < 0 -> thuế = 0
TEST(DecisionTable, R4_TaxableNegativeOrZero) {
    // gross=14M, dep=1, ins=3M -> insAllow=min(3M, 10%*14M=1.4M)=1.4M
    // deductions=10M + 4M + 1.4M = 15.4M -> taxable<=0
    EXPECT_EQ(computeTax(makeInput(14000000, 1, 3000000)), 0);
}

// R5: 0 <= Taxable <= 5M -> 5%
TEST(DecisionTable, R5_FirstBracket) {
    // gross=20M, dep=1, ins=3M -> deductions=16M -> taxable=4M
    EXPECT_EQ(computeTax(makeInput(20000000, 1, 3000000)), 200000);
}

// R6: 5M < Taxable <= 15M -> 5% cho 5M đầu + 10% phần còn lại
TEST(DecisionTable, R6_SecondBracket) {
    // gross=28M, dep=1, ins=0 -> deductions=14M -> taxable=14M
    // tax = 5%*5M + 10%*(9M) = 250000 + 900000 = 1150000
    EXPECT_EQ(computeTax(makeInput(28000000, 1, 0)), 1150000);
}

// R7: Taxable > 15M -> 5%*5M + 10%*10M + 20% phần >15M
TEST(DecisionTable, R7_ThirdBracket) {
    // gross=35M, dep=1, ins=0 -> deductions=14M -> taxable=21M
    // tax = 250000 + 1000000 + 20%*(6M) = 250000 + 1000000 + 1200000 = 2450000
    EXPECT_EQ(computeTax(makeInput(35000000, 1, 0)), 2450000);
}