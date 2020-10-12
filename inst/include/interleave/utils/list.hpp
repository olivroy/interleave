#ifndef R_INTERLEAVE_UTILS_LIST_H
#define R_INTERLEAVE_UTILS_LIST_H

// adapted from colourvalues & sfheaders

#include <Rcpp.h>

#include "geometries/utils/sexp/sexp.hpp"

namespace interleave {
namespace utils {

  inline int vector_type( int new_type, int existing_type ) {

    // can't change from STRSXP
    if( existing_type == 16 ) {
      return existing_type;
    }

    std::vector< int > valid_types{10, 13, 14, 16};
    bool new_is_valid = ( std::find( valid_types.begin(), valid_types.end(), new_type ) != valid_types.end() );
    bool existing_is_valid = ( std::find( valid_types.begin(), valid_types.end(), existing_type ) != valid_types.end() );

    if( new_type == existing_type && new_is_valid ) {
      return existing_type;
    }

    // convert new type up to existing type
    if( new_type < existing_type && existing_is_valid ) {
      return existing_type;
    }

    if( new_type > existing_type && new_is_valid ) {
      return new_type;
    }

    if( new_type > existing_type && !new_is_valid ) {
      return 16;
    }

    if( existing_is_valid ) {
      return existing_type;
    }

    return 16;
  }


  /*
   * list rows
   *
   * Counts the number of rows in each list element
   * The output list is the same size as the input, where all elements are the
   * count of number of rows
   */
  inline Rcpp::List list_rows(
      SEXP obj,
      R_xlen_t& total_size
  ) {

    // Rcpp::Rcout << "TYPEOF( obj ) " << TYPEOF( obj ) << std::endl;
    // Rcpp::Rcout << "inherits df: " << Rf_inherits( obj, "data.frame" ) << std::endl;
    // Rcpp::Rcout << "isNewList: " << Rf_isNewList( obj ) << std::endl;

    //R_xlen_t n;

    if( Rf_inherits( obj, "data.frame" ) ) {
      // don't loop / recurse...
      Rcpp::stop("interleave - expecting a list input");
    }

    if( !Rf_isNewList( obj ) ) {
      Rcpp::stop("interleave - expecting a list input");
    }

    Rcpp::List lst = Rcpp::as < Rcpp::List > ( obj );

    R_xlen_t n = lst.size();
    Rcpp::List res( n ); // create a list to store the size corresponding to each list element
    R_xlen_t i;
    for( i = 0; i < n; i++ ) {
      // Rcpp::Rcout << i << "/" << n << std::endl;
      SEXP obj = lst[ i ];

      // Rcpp::Rcout << "TYPEOF( obj ) " << TYPEOF( obj ) << std::endl;

      switch( TYPEOF( obj ) ) {
        case VECSXP: {
          if( Rf_isNewList( obj ) && !Rf_inherits( obj, "data.frame" ) ) {
            // Rcpp::Rcout << "new_list" << std::endl;
            res[ i ] = list_rows( obj, total_size );
            break; // break is inside the 'if', because a data.frame needs to fall through to 'default'
          }
        }
        default: {
          // Rcpp::Rcout << "default" << std::endl;
          //SEXP obj = lst[i];
          R_xlen_t n_rows = geometries::utils::sexp_n_row( obj );  // sexp_n_row is non-recursive
          res[i] = n_rows;
          total_size += n_rows;
        }
      }
    }
    return res;
  }

  /*
   * list size
   *
   * Returns a list containing the length of each list element
   *
   */
  inline Rcpp::List list_size(
      const Rcpp::List& lst,
      R_xlen_t& total_size,
      int& existing_type
  ) {
    R_xlen_t n = lst.size();
    Rcpp::List res( n ); // create a list to store the size corresponding to each list element
    R_xlen_t i;
    for( i = 0; i < n; i++ ) {
      switch( TYPEOF( lst[i] ) ) {
        case VECSXP: {
          res[ i ] = list_size( lst[i], total_size, existing_type );
          break;
        }
        default: {
          R_xlen_t n_elements = Rf_length( lst[i] );
          int new_type = TYPEOF( lst[i] );
          existing_type = vector_type( new_type, existing_type );
          res[i] = n_elements;
          total_size += n_elements;
        }
      }
    }
    return res;
  }

  // collapse a vector into a list
  // where line_ids gives the start and end indexes of v to use
  template < int RTYPE >
  inline Rcpp::List fill_list(
      Rcpp::Vector< RTYPE >& v,
      Rcpp::IntegerMatrix& line_positions
  ) {
    R_xlen_t n = line_positions.nrow();  // nrow should also be the row of the final sf object we are creating
    Rcpp::List res( n );
    R_xlen_t i;

    for( i = 0; i < n; ++i ) {
      R_xlen_t start = line_positions(i, 0);
      R_xlen_t end = line_positions(i, 1);
      Rcpp::IntegerVector elements = Rcpp::seq( start, end );
      res[ i ] = v[ elements ];
    }
    return res;
  }

  // TODO - handle dates and factors??
  inline Rcpp::List fill_list(
      SEXP& v,
      Rcpp::IntegerMatrix& line_positions
  ) {
    switch( TYPEOF( v ) ) {
    case LGLSXP: {
      Rcpp::LogicalVector lv = Rcpp::as< Rcpp::LogicalVector >( v );
      return fill_list( lv, line_positions );
    }
    case INTSXP: {
      Rcpp::IntegerVector iv = Rcpp::as< Rcpp::IntegerVector >( v );
      return fill_list( iv, line_positions );
    }
    case REALSXP: {
      Rcpp::NumericVector nv = Rcpp::as< Rcpp::NumericVector >( v );
      return fill_list( nv, line_positions );
    }
    case STRSXP: {
      Rcpp::StringVector sv = Rcpp::as< Rcpp::StringVector >( v );
      return fill_list( sv, line_positions );
    }
    default: {
      Rcpp::stop("sfheaders - unknown column type");
    }
    }
    return Rcpp::List::create(); // #nocov
  }


  /*
   * @param lst - the original input list
   * @param lst_sizes - the dimensions of the list
   * @param values - vector of values to be unlist
   */
  // inline void unlist_list(
  //     const Rcpp::List& lst,
  //     const Rcpp::List& lst_sizes,
  //     Rcpp::LogicalVector& values,
  //     int& list_position
  // ) {
  //   // - iterate through original list
  //   // - extract each element and insert into 'values'
  //   R_xlen_t n = lst.size();
  //   Rcpp::List res( n );
  //   R_xlen_t i;
  //   for( i = 0; i < n; ++i ) {
  //     switch( TYPEOF( lst[ i ] ) ) {
  //     case VECSXP: {
  //       unlist_list( lst[ i ], lst_sizes[ i ], values, list_position );
  //       break;
  //     }
  //     default: {
  //       Rcpp::IntegerVector n_elements = Rcpp::as< Rcpp::IntegerVector >( lst_sizes[ i ] );
  //       int end_position = list_position + n_elements[0] - 1;
  //       Rcpp::IntegerVector elements = Rcpp::seq( list_position, end_position );
  //       values[ elements ] = Rcpp::as< Rcpp::LogicalVector >( lst[ i ] );
  //
  //       list_position = end_position + 1;
  //       break;
  //     }
  //     }
  //   }
  // }
  //
  // inline void unlist_list(
  //     const Rcpp::List& lst,
  //     const Rcpp::List& lst_sizes,
  //     Rcpp::IntegerVector& values,
  //     int& list_position
  // ) {
  //   // - iterate through original list
  //   // - extract each element and insert into 'values'
  //   R_xlen_t n = lst.size();
  //   Rcpp::List res( n );
  //   R_xlen_t i;
  //   for( i = 0; i < n; ++i ) {
  //     switch( TYPEOF( lst[ i ] ) ) {
  //     case VECSXP: {
  //       unlist_list( lst[ i ], lst_sizes[ i ], values, list_position );
  //       break;
  //     }
  //     default: {
  //       Rcpp::IntegerVector n_elements = Rcpp::as< Rcpp::IntegerVector >( lst_sizes[ i ] );
  //       int end_position = list_position + n_elements[0] - 1;
  //       Rcpp::IntegerVector elements = Rcpp::seq( list_position, end_position );
  //       values[ elements ] = Rcpp::as< Rcpp::IntegerVector >( lst[ i ] );
  //
  //       list_position = end_position + 1;
  //       break;
  //     }
  //     }
  //   }
  // }

  template< int RTYPE >
  inline void unlist_list(
      const Rcpp::List& lst,
      const Rcpp::List& lst_sizes,
      Rcpp::Vector< RTYPE >& values,
      int& list_position
  ) {
    // - iterate through original list
    // - extract each element and insert into 'values'
    R_xlen_t n = lst.size();
    Rcpp::List res( n );
    R_xlen_t i;
    for( i = 0; i < n; ++i ) {
      switch( TYPEOF( lst[ i ] ) ) {
      case VECSXP: {
        unlist_list< RTYPE >( lst[ i ], lst_sizes[ i ], values, list_position );
        break;
      }
      default: {
        Rcpp::IntegerVector n_elements = Rcpp::as< Rcpp::IntegerVector >( lst_sizes[ i ] );
        int end_position = list_position + n_elements[0] - 1;
        Rcpp::IntegerVector elements = Rcpp::seq( list_position, end_position );
        values[ elements ] = Rcpp::as< Rcpp::Vector< RTYPE > >( lst[ i ] );

        list_position = end_position + 1;
        break;
      }
      }
    }
  }

  // inline void unlist_list(
  //     const Rcpp::List& lst,
  //     const Rcpp::List& lst_sizes,
  //     Rcpp::StringVector& values,
  //     int& list_position
  // ) {
  //   // - iterate through original list
  //   // - extract each element and insert into 'values'
  //   R_xlen_t n = lst.size();
  //   Rcpp::List res( n );
  //   R_xlen_t i;
  //   for( i = 0; i < n; i++ ) {
  //     switch( TYPEOF( lst[i] ) ) {
  //     case VECSXP: {
  //       unlist_list( lst[ i ], lst_sizes[ i ], values, list_position );
  //       break;
  //     }
  //     default: {
  //       Rcpp::IntegerVector n_elements = Rcpp::as< Rcpp::IntegerVector >( lst_sizes[ i ] );
  //       int end_position = list_position + n_elements[0] - 1;
  //       Rcpp::IntegerVector elements = Rcpp::seq( list_position, end_position );
  //       values[ elements ] = Rcpp::as< Rcpp::StringVector >( lst[ i ] );
  //
  //       list_position = end_position + 1;
  //       break;
  //     }
  //     }
  //   }
  // }

  inline SEXP unlist_list( Rcpp::List& lst ) {

    R_xlen_t total_size = 0;
    int existing_type = 10;
    int position = 0;
    Rcpp::List lst_sizes = list_size( lst, total_size, existing_type );

    switch( existing_type ) {
      case LGLSXP: {
        Rcpp::Vector< LGLSXP > v( total_size );
        unlist_list< LGLSXP >( lst, lst_sizes, v, position );
        return v;
      }
      case INTSXP: {
        Rcpp::Vector< INTSXP > v( total_size );
        unlist_list< INTSXP >( lst, lst_sizes, v, position );
        return v;
      }
      case REALSXP: {
        Rcpp::Vector< REALSXP > v( total_size );
        unlist_list< REALSXP >( lst, lst_sizes, v, position );
        return v;
      }
      default: {
        Rcpp::Vector< STRSXP > v( total_size );
        unlist_list< STRSXP >( lst, lst_sizes, v, position );
        return v;
      }
    }

    Rcpp::stop("sfheaders - couldn't unlist this object");
    return lst; // #nocov - never reaches

  }

} // utils
} // sfheaders

#endif

