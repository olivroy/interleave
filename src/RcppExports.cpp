// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// rcpp_earcut
SEXP rcpp_earcut(Rcpp::List& polygon);
RcppExport SEXP _interleave_rcpp_earcut(SEXP polygonSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::List& >::type polygon(polygonSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_earcut(polygon));
    return rcpp_result_gen;
END_RCPP
}
// rcpp_interleave
SEXP rcpp_interleave(SEXP& obj);
RcppExport SEXP _interleave_rcpp_interleave(SEXP objSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP& >::type obj(objSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_interleave(obj));
    return rcpp_result_gen;
END_RCPP
}
// rcpp_interleave_point
SEXP rcpp_interleave_point(SEXP& obj, int stride);
RcppExport SEXP _interleave_rcpp_interleave_point(SEXP objSEXP, SEXP strideSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP& >::type obj(objSEXP);
    Rcpp::traits::input_parameter< int >::type stride(strideSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_interleave_point(obj, stride));
    return rcpp_result_gen;
END_RCPP
}
// rcpp_interleave_line
SEXP rcpp_interleave_line(Rcpp::List& obj, int stride);
RcppExport SEXP _interleave_rcpp_interleave_line(SEXP objSEXP, SEXP strideSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::List& >::type obj(objSEXP);
    Rcpp::traits::input_parameter< int >::type stride(strideSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_interleave_line(obj, stride));
    return rcpp_result_gen;
END_RCPP
}
// rcpp_interleave_triangle
SEXP rcpp_interleave_triangle(Rcpp::List& obj);
RcppExport SEXP _interleave_rcpp_interleave_triangle(SEXP objSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::List& >::type obj(objSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_interleave_triangle(obj));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_interleave_rcpp_earcut", (DL_FUNC) &_interleave_rcpp_earcut, 1},
    {"_interleave_rcpp_interleave", (DL_FUNC) &_interleave_rcpp_interleave, 1},
    {"_interleave_rcpp_interleave_point", (DL_FUNC) &_interleave_rcpp_interleave_point, 2},
    {"_interleave_rcpp_interleave_line", (DL_FUNC) &_interleave_rcpp_interleave_line, 2},
    {"_interleave_rcpp_interleave_triangle", (DL_FUNC) &_interleave_rcpp_interleave_triangle, 1},
    {NULL, NULL, 0}
};

RcppExport void R_init_interleave(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
