#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "BinTree.h"
#include "SymmetricSecondRankTensor.h"

// -----------------------------------------------------------------------
using TElement = unsigned short;
using TTree = BinTree< TElement >;

// -----------------------------------------------------------------------
TTree* BuildOrdOptTree_BT(
  const SymmetricSecondRankTensor< long long >& B,
  std::vector< bool >& M,
  long long i, long long j
  )
{
  if( j > i && !M[ B( i, j ) ] )
  {
    TTree* r = new TTree( B( i, j ) - 1 );
    M[ B( i, j ) ] = true;
    r->set_left( BuildOrdOptTree_BT( B, M, i - 1, B( i, j ) - 1 ) );
    r->set_right( BuildOrdOptTree_BT( B, M, B( i, j ), j ) );
    return( r );
  }
  else
    return( nullptr );
}

// -----------------------------------------------------------------------
TTree* BuildOrdOptTree(
  const std::vector< double >& P, const std::vector< double >& Q
  )
{
  // Tables
  SymmetricSecondRankTensor< double >    T( Q.size( ) );
  SymmetricSecondRankTensor< long long > B( Q.size( ) );

  // Base cases
  for( long long i = 0; i < Q.size( ); ++i )
  {
    T( i, i ) = Q[ i ];
    B( i, i ) = i + 1;
  } // end for

  // Remaining cases
  for( long long i = P.size( ); 1 <= i; --i )
  {
    for( long long j = i; j <= P.size( ); ++j )
    {
      double w = Q[ i - 1 ];
      for( long long l = i; l <= j; ++l )
        w += P[ l - 1 ] + Q[ l ];

      for( long long r = i; r <= j; ++r )
      {
        double v = w;
        v += T( i - 1, r - 1 );
        v += T( r, j );
        if( v < T( i - 1, j ) || r == i )
        {
          T( i - 1, j ) = v;
          B( i - 1, j ) = r;
        } // end if
      } // end for
    } // end for
  } // end for

  std::vector< bool > M( P.size( ), false );
  return( BuildOrdOptTree_BT( B, M, 0, P.size( ) ) );
}

// -----------------------------------------------------------------------
template< class _T >
void process( const std::string& fname )
{
  std::ifstream fstr( fname.c_str( ), std::ios::binary | std::ios::ate );
  long long size = fstr.tellg( );
  fstr.seekg( 0, std::ios::beg );

  std::vector< _T > data( size / sizeof( _T ) );
  char* buffer = reinterpret_cast< char* >( data.data( ) );
  fstr.read( buffer, data.size( ) * sizeof( _T ) );
  fstr.close( );

  // Build histograms
  std::vector< double > P( 1 << ( sizeof( _T ) << 3 ), 0 );
  for( long long i = 0; i < data.size( ); ++i )
    P[ data[ i ] ] += double( 1 ) / double( data.size( ) );
  std::vector< double > Q( P.size( ) + 1, 0 );

  // Build tree
  TTree* tree = BuildOrdOptTree( P, Q );

  // Get most and least repeated data
  long long most = 0, least = 0;
  for( long long i = 1; i < P.size( ); ++i )
  {
    if( P[ most ] < P[ i ] ) most = i;
    if( P[ i ] < P[ least ] ) least = i;
  } // end for

  // Some data
  std::cout << "==============================" << std::endl;
  std::cout << "Preoder" << std::endl;
  std::cout << "==============================" << std::endl;
  tree->print_preorder( std::cout );
  std::cout << "==============================" << std::endl;
  std::cout << "Number of tokens: " << P.size( ) << std::endl;
  std::cout << "Height: " << tree->height( ) << std::endl;
  std::cout << "Weight: " << tree->weight( ) << std::endl;
  std::cout
    << "Theoretical height: "
    << std::ceil( std::log2( tree->height( ) ) )
    << std::endl;
  std::cout << "Most repeated element: " << most << " (f = " << P[ most ] << ")" << std::endl;
  std::cout << "Least repeated element: " << least << " (f = " << P[ least ] << ")" << std::endl;

  // Finish
  delete tree;
}

// -----------------------------------------------------------------------
int main( int argc, char** argv )
{
  if( argc < 3 )
  {
    std::cerr << "Usage: " << argv[ 0 ] << " input byte/word" << std::endl;
    std::cerr << "==================================" << std::endl;
    std::cerr << "======= Using example data =======" << std::endl;
    std::cerr << "==================================" << std::endl;

    std::vector< double > P( {       0.15, 0.10, 0.05, 0.10, 0.20 } );
    std::vector< double > Q( { 0.05, 0.10, 0.05, 0.05, 0.05, 0.10 } );

    TTree* tree = BuildOrdOptTree( P, Q );

    tree->print_preorder( std::cout );

    std::cerr << "==================================" << std::endl;

    delete tree;
    return( EXIT_FAILURE );
  }
  else
  {
    std::string fname = argv[ 1 ];
    std::string type = argv[ 2 ];
    if( type == "byte" )
      process< unsigned char >( fname );
    else if( type == "word" )
      process< unsigned short >( fname );
    return( EXIT_SUCCESS );
  } // end if
}

// eof - build_OptOrdBinTree.cxx
