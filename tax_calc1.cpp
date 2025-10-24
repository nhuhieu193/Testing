// #include <algorithm>
// #include <stdexcept>

// struct TaxInput {
//     long long grossIncome;
//     int       dependents;
//     long long insurancePaid;
// };


// /* L1 */ long long computeTax(TaxInput in) {
// /* L2 */     if (in.grossIncome < 0 | in.dependents < 0 | in.insurancePaid < 0) {
// /* L3 */         throw std::invalid_argument("Inputs cannot be negative.");
//              }

// /* L4 */     long long fixedDeduction = 10'000'000;
// /* L5 */     long long dependentDeduction = std::min(in.dependents, 2) * 4'000'000;
// /* L6 */     long long insuranceDeduction = std::min({(long long)(in.grossIncome * 0.1), 5'000'000LL, in.insurancePaid});
// /* L7 */     long long totalDeductions = fixedDeduction + dependentDeduction + insuranceDeduction;
// /* L8 */     long long taxableIncome = std::max(0LL, in.grossIncome - totalDeductions);

// /* L9 */     if (taxableIncome <= 0) {
// /* L10*/         return 0;
//              }

// /* L11*/     long long tax = 0;
// /* L12*/     if (taxableIncome <= 5'000'000) {
// /* L13*/         tax = taxableIncome * 0.05;
// /* L14*/     } else if (taxableIncome <= 15'000'000) {
// /* L15*/         tax = (5'000'000 * 0.05) + (taxableIncome - 5'000'000) * 0.10;
//              } else {
// /* L16*/         tax = (5'000'000 * 0.05) + (10'000'000 * 0.10) + (taxableIncome - 15'000'000) * 0.20;
//              }
// /* L17*/     return tax;
//          }