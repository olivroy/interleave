#include "interleave/earcut/interleave.hpp"
#include "interleave/primitives/primitives.hpp"

#include <Rcpp.h>

#include "interleave/earcut/earcut.hpp"
#include "interleave/interleave.hpp"

// [[Rcpp::export]]
SEXP rcpp_earcut( Rcpp::List& polygon ) {
  return interleave::earcut::earcut( polygon );
}


// [[Rcpp::export]]
SEXP rcpp_interleave( SEXP& obj, bool attributed = false ) {
  return interleave::interleave( obj, attributed );
}

// [[Rcpp::export]]
SEXP rcpp_interleave_point( SEXP& lst ) {
  return interleave::primitives::interleave_primitive( lst, interleave::primitives::INTERLEAVE_POINT );
}


// [[Rcpp::export]]
SEXP rcpp_interleave_line( SEXP& lst ) {
  return interleave::primitives::interleave_primitive( lst, interleave::primitives::INTERLEAVE_LINE );
}


/*
 * interleave triangle
 *
 * Accepts a list of polygons
 * p1 <- list(mat1, hole1)
 * p2 <- list(mat2)
 * p3 <- list(mat3)
 * list( p1, p2, p3, ... )
 *
 * but NOT multipolygons, nor a single polygon (e.g. sfg_POLYGON)
 * it MUST be a collection of POLYGONS
 *
 * You can use sfheaders::sfc_cast() to convert inputs to POLYGONS
 *
 */
// [[Rcpp::export]]
SEXP rcpp_interleave_triangle( SEXP& obj, Rcpp::List properties ) {
  return interleave::primitives::interleave_triangle( obj, properties );
}


// [[Rcpp::export]]
SEXP rcpp_list_element_count( Rcpp::List lst ) {
  R_xlen_t total_size = 0;
  R_xlen_t n_objects = 1;
  int existing_type = 10;
  return interleave::utils::list_element_count(lst, total_size, n_objects, existing_type);
}

